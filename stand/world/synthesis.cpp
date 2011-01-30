/*
 *
 * Synthesis API for WORLD 0.0.1 by M. Morise
 *
 *    synthesis.cpp
 *                        (c) M. Morise 2010-
 *                            edit and comment by HAL, kbinani
 *
 *  This file is a part of WORLD system.
 * WORLD needs FFTW. Please install FFTW to use these files.
 * FFTW is here; http://www.fftw.org/
 *
 * notice: this comment is added by HAL.
 *         Original files are on the web page below;
 *         http://www.aspl.is.ritsumei.ac.jp/morise/world/
 * (this file is from WORLD 0.0.1)
 *
 *  synthesis.cpp includes a set of functions
 * that supports synthesis on WORLD 0.0.1.
 *  Notice that aperiodicity of 0.0.1 is much different
 * from that of 0.0.4. Please use platinum function
 * for synthesis function.
 *
 */
#include "world.h"

#include <stdio.h> // for debug
#include <stdlib.h>

// spectrum, cepstrumは毎回malloc, freeするのが面倒だから．
void getOneFrameSegment(double *f0, int tLen, double **specgram, double **aperiodicity, int fftl, double framePeriod, double currentTime, int fs, double defaultF0,
                        fftw_complex *spectrum, fftw_complex *cepstrum, 
                        double *response, int xLen);

// xorshift法と中心極限定理との組み合わせ
/*double randn(void) 
{
    static unsigned int x = 123456789;
    static unsigned int y = 362436069;
    static unsigned int z = 521288629;
    static unsigned int w = 88675123; 
    unsigned int t;
     t = x ^ (x << 11);
    x = y; y = z; z = w;

    int i;
    double ans;
    unsigned int tmp;

    tmp = 0;
    ans = 0.0;
    for(i = 0;i < 12;i++)
    {
         t = x ^ (x << 11);
        x = y; y = z; z = w;
        w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
        tmp += w>>4;
    }
    ans = (double)tmp / 268435456.0 - 6.0;
    return ans;
}*/

void getMinimumPhaseSpectrum(double *inputSpec, fftw_complex *spectrum, fftw_complex *cepstrum, int fftl)
{
    int i;
    double real, imag;
    fftw_plan forwardFFT, inverseFFT;
#ifdef STND_MULTI_THREAD
    if( hFFTWMutex ){
        stnd_mutex_lock( hFFTWMutex );
    }

    forwardFFT = fftw_plan_dft_1d(fftl, spectrum, cepstrum, FFTW_FORWARD, FFTW_ESTIMATE);
    inverseFFT = fftw_plan_dft_1d(fftl, cepstrum, spectrum, FFTW_BACKWARD, FFTW_ESTIMATE);

    if( hFFTWMutex ){
        stnd_mutex_unlock( hFFTWMutex );
    }
#else
    forwardFFT = fftw_plan_dft_1d(fftl, spectrum, cepstrum, FFTW_FORWARD, FFTW_ESTIMATE);
    inverseFFT = fftw_plan_dft_1d(fftl, cepstrum, spectrum, FFTW_BACKWARD, FFTW_ESTIMATE);
#endif

    // 値を取り出す
    for(i = 0;i <= fftl/2;i++)    
    {
        // 加算は暫定処置
        spectrum[i][0] = log(inputSpec[i] + 1.0e-17)/2.0;
        spectrum[i][1] = 0.0;
    }
    for(;i < fftl;i++)
    {
        spectrum[i][0] = spectrum[fftl-i][0];
        spectrum[i][1] = 0.0;
    }
    fftw_execute(forwardFFT);
    for(i = 1;i < fftl/2;i++)
    {
        cepstrum[i][0] = 0.0;
        cepstrum[i][1] = 0.0;
    }
    for(;i < fftl;i++)
    {
        cepstrum[i][0] *= 2.0;
        cepstrum[i][1] *= 2.0;
    }
    fftw_execute(inverseFFT);
    for(i = 0;i < fftl;i++)
    {
        real = exp(spectrum[i][0]/(double)fftl)*cos(spectrum[i][1]/(double)fftl);
        imag = exp(spectrum[i][0]/(double)fftl)*sin(spectrum[i][1]/(double)fftl);
        spectrum[i][0] = real;
        spectrum[i][1] = imag;
    }
#ifdef STND_MULTI_THREAD
    if( hFFTWMutex ){
        stnd_mutex_lock( hFFTWMutex );
    }
#endif
    fftw_destroy_plan(forwardFFT);
    fftw_destroy_plan(inverseFFT);
#ifdef STND_MULTI_THREAD
    if( hFFTWMutex ){
        stnd_mutex_unlock( hFFTWMutex );
    }
#endif

}

// 特定時刻の応答を取得する．
void getOneFrameSegment(double *f0, int tLen, double **specgram, double **aperiodicity, int fftl, double framePeriod, double currentTime, int fs, double defaultF0,
                        fftw_complex *spectrum, fftw_complex *cepstrum, 
                        double *response, int xLen)
{
    int i, offset, noiseSize;
    double real, imag, tmp;
    fftw_plan    inverseFFT_RP;                // FFTセット
    fftw_plan    inverseFFT_RA;                // FFTセット
    fftw_plan    forwardFFT_R;                // FFTセット

    int currentFrame, currentPosition;
    double *periodicSpec, *aperiodicSpec, *aperiodicRatio;
    periodicSpec   = (double *)malloc(sizeof(double)* fftl);
    aperiodicSpec  = (double *)malloc(sizeof(double)* fftl);
    aperiodicRatio = (double *)malloc(sizeof(double)* fftl);

    fftw_complex *noiseSpec;
    noiseSpec   = (fftw_complex *)malloc(sizeof(fftw_complex)* fftl);

    double *aperiodicResponse, *periodicResponse;
    aperiodicResponse = (double *)malloc(sizeof(double)* fftl);
    periodicResponse  = (double *)malloc(sizeof(double)* fftl);

#ifdef STND_MULTI_THREAD
    if( hFFTWMutex ){
        stnd_mutex_lock( hFFTWMutex );
    }

    forwardFFT_R = fftw_plan_dft_r2c_1d(fftl, aperiodicResponse, noiseSpec, FFTW_ESTIMATE);
    inverseFFT_RP = fftw_plan_dft_c2r_1d(fftl, spectrum, periodicResponse ,  FFTW_ESTIMATE);
    inverseFFT_RA = fftw_plan_dft_c2r_1d(fftl, spectrum, aperiodicResponse,  FFTW_ESTIMATE);

    if( hFFTWMutex ){
        stnd_mutex_unlock( hFFTWMutex );
    }
#else
    forwardFFT_R = fftw_plan_dft_r2c_1d(fftl, aperiodicResponse, noiseSpec, FFTW_ESTIMATE);
    inverseFFT_RP = fftw_plan_dft_c2r_1d(fftl, spectrum, periodicResponse ,  FFTW_ESTIMATE);
    inverseFFT_RA = fftw_plan_dft_c2r_1d(fftl, spectrum, aperiodicResponse,  FFTW_ESTIMATE);
#endif

    currentFrame = (int)(currentTime/(framePeriod/1000.0) + 0.5);    
    currentPosition = (int)(currentTime*(double)fs);

    tmp = currentTime + 1.0/(f0[currentFrame] == 0.0 ? defaultF0 : f0[currentFrame]);
    noiseSize = (int)(tmp*(double)fs) - currentPosition;

    if(f0[currentFrame] == 0.0)
    { // 無声音として扱う(波形の再利用)
        offset = currentPosition - (int)((double)currentFrame*framePeriod/1000.0*(double)fs);
        if(offset < 0)
        {
            currentFrame-=1;
            offset = currentPosition - (int)((double)currentFrame*framePeriod/1000.0*(double)fs);
        }
        for(i = 0;i < (int)((double)fs/defaultF0);i++)
        {
            if(i+currentPosition >= xLen || i+offset > fftl/2) break;
            response[i] = specgram[currentFrame][i+offset]; 
        }
    }
    else
    { // 有声音として扱う

        // 非周期性指標の計算
        calculateAperiodicity(aperiodicity[currentFrame], fftl, fs, aperiodicRatio);

        // 値を取り出す
        for(i = 0;i <= fftl/2;i++)
        {
            periodicSpec[i] = specgram[currentFrame][i] * aperiodicRatio[i];
        }
        getMinimumPhaseSpectrum(periodicSpec, spectrum, cepstrum, fftl);

        fftw_execute(inverseFFT_RP);

        // ここから非周期音の合成
        for(i = 0;i <= fftl/2;i++)
        {
            aperiodicSpec[i] = specgram[currentFrame][i] * ((1-aperiodicRatio[i])+0.000000000000001);
        }
        getMinimumPhaseSpectrum(aperiodicSpec, spectrum, cepstrum, fftl);
        for(i = 0;i < noiseSize;i++)
            aperiodicResponse[i] = randn();
        for(;i < fftl;i++)
            aperiodicResponse[i] = 0.0;
        fftw_execute(forwardFFT_R);

        for(i = 0;i <= fftl/2;i++)
        {
            real = spectrum[i][0]*noiseSpec[i][0] - spectrum[i][1]*noiseSpec[i][1];
            imag = spectrum[i][0]*noiseSpec[i][1] + spectrum[i][1]*noiseSpec[i][0];
            spectrum[i][0] = real;
            spectrum[i][1] = imag;
        }
        fftw_execute(inverseFFT_RA);

        // 1.633はエネルギー調整用のマジックナンバー
        // 基本周期の３倍のハニング窓で波形を切り出した際のエネルギー損失を補償している．
        // *8.0は完全にマジックナンバー
        // ここは，将来リリースでの調整が必須です．
        for(i = 0;i < fftl;i++)
        {
            //response[i] = (periodicResponse[i]*sqrt((double)noiseSize) + aperiodicResponse[i])*1.633/(double)fftl*12.0/sqrt((double)noiseSize);

            // IFFT->FFT の係数による補正は入力波形の正規化の段階で行っている． byHAL
            response[i] = (periodicResponse[i]*sqrt((double)noiseSize) + aperiodicResponse[i])*1.633*12.0/sqrt((double)noiseSize);

//            response[i] = (periodicResponse[i]/sqrt((double)noiseSize))*1.633/(double)fftl*12.0;
//                aperiodicResponse[i]/(double)noiseSize;
//            response[i] = aperiodicResponse[i]/(double)fftl/sqrt((double)noiseSize);
        }

    }

#ifdef STND_MULTI_THREAD
    if( hFFTWMutex ){
        stnd_mutex_lock( hFFTWMutex );
    }

    fftw_destroy_plan(inverseFFT_RP);
    fftw_destroy_plan(inverseFFT_RA);
    fftw_destroy_plan(forwardFFT_R);

    if( hFFTWMutex ){
        stnd_mutex_unlock( hFFTWMutex );
    }
#else
    fftw_destroy_plan(inverseFFT_RP);
    fftw_destroy_plan(inverseFFT_RA);
    fftw_destroy_plan(forwardFFT_R);
#endif
    free(periodicSpec);
    free(aperiodicSpec);
    free(periodicResponse);
    free(aperiodicResponse);
    free(aperiodicRatio);
    free(noiseSpec);
}



void synthesis(double *f0, int tLen, double **specgram, double **aperiodicity, int fftl, double framePeriod, int fs, 
               double *synthesisOut, int xLen)
{
    int i,j;
//    double defaultF0 = 300.0; // うーん．
    double defaultF0 = DEFAULT_F0; // うーん．
    double *impulseResponse;
    impulseResponse = (double *)malloc(sizeof(double) * fftl);
    fftw_complex        *cepstrum, *spectrum;    // ケプストラムとスペクトル
    cepstrum = (fftw_complex *)malloc(sizeof(fftw_complex) * fftl);
    spectrum = (fftw_complex *)malloc(sizeof(fftw_complex) * fftl);

    double currentTime = 0.0;
    int currentPosition = 0;//currentTime / framePeriod;
    int currentFrame = 0;
    for(i = 0;;i++)
    {
        // 完全な無音は飛ばす．
        if(currentFrame < tLen && f0[currentFrame] < 0.0){
            currentFrame++;
            currentTime = (double)currentFrame * framePeriod / 1000.0;
            currentPosition = (int)(currentTime * (double)fs);
            if(currentFrame == tLen) break;
            continue;
        }
        for(j = 0;j < fftl;j++) impulseResponse[j] = 0.0; // 配列は毎回初期化

        getOneFrameSegment(f0, tLen, specgram, aperiodicity, fftl, framePeriod, currentTime, fs, defaultF0,
                        spectrum, cepstrum, impulseResponse, xLen);

        currentPosition = (int)(currentTime*(double)fs);

        for(j = 0;j < fftl/2;j++)
        {
            if(j+currentPosition >= xLen) break;
            synthesisOut[j+currentPosition] += impulseResponse[j];
        }

        // 更新
        currentTime += 1.0/(f0[currentFrame] == 0.0 ? defaultF0 : f0[currentFrame]);
        currentFrame = (int)(currentTime/(framePeriod/1000.0) + 0.5);
        currentPosition = (int)(currentTime*(double)fs);
        if(j+currentPosition >= xLen || currentFrame >= tLen) break;
    }

    free(cepstrum); free(spectrum);
    free(impulseResponse);
    return;
}
