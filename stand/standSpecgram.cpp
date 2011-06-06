/*
 *
 *    standSpecgram.cpp                                         
 *                              (c) HAL 2010-           
 *
 *  This file is a part of STAND Library.
 * standSpecgram is a structure that contains
 * STAR specgram, PLATINUM aperiodicities and pitch contour.
 * This class provides analysis and synthesis
 * between WORLD and WAVE files.
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#include "stand.h"
#include "standSpecgram.h"

#ifdef STND_MULTI_THREAD

struct computeArg{
    double *f0;
    int tLen;
    double **specgram;
    double **aperiodicity;
    int fftl;
    double framePeriod;
    int fs;
    double *synthWave;
    long waveLength;
    bool fast;
};

__stnd_thread_start_retval __stnd_declspec compute_multithread_win32( void *arg )
{
    computeArg *p = (computeArg *)arg;

    if( p->fast )
    {
        synthesis( p->f0, p->tLen, p->specgram, p->aperiodicity, p->fftl, p->framePeriod, p->fs, p->synthWave, p->waveLength );
    }
    else
    {
        synthesis_v4( p->f0, p->tLen, p->specgram, p->aperiodicity, p->fftl, p->framePeriod, p->fs, p->synthWave, p->waveLength );
    }

#ifndef USE_PTHREADS
    _endthreadex( 0 );
#endif
    return 0;
}

void standSpecgram::setArguments( computeArg &arg1, computeArg &arg2 )
{
    arg1.f0 = f0;
    arg1.specgram = specgram;
    arg1.aperiodicity = aperiodicity;
    arg1.fftl = fftl;
    arg1.framePeriod = framePeriod;
    arg1.fs = fs;
    arg1.synthWave = synthWave;
    arg1.waveLength = waveLength;
    arg1.fast = isFast;

    // ここで波形の位置を正しく計算しておきたい．
    // wave のバッファが重なる fft / 2 の長さだけスレッドセーフでない．
    double currentTime = 0.0;
    int currentPosition = 0;//currentTime / framePeriod;
    int currentFrame = 0;
    for(; ; )
    {
        if( f0[currentFrame] < 0.0 )
        {
            currentFrame++;
            currentTime = (double)currentFrame * framePeriod / 1000.0;
            currentPosition = (int)(currentTime * (double)fs);
            continue;
        }
        currentPosition = (int)(currentTime * (double)fs);
        currentTime += 1.0 / (f0[currentFrame] == 0.0 ? DEFAULT_F0 : f0[currentFrame]);
        currentFrame = (int)(currentTime / (framePeriod / 1000.0) + 0.5);
        currentPosition = (int)(currentTime * (double)fs);
        if( currentFrame >= tLen / 2 ) break; 
    }
    arg1.tLen = currentFrame;

    arg2.f0 = &f0[currentFrame];
    arg2.tLen = tLen - currentFrame;
    arg2.specgram = &specgram[currentFrame];
    arg2.aperiodicity = &aperiodicity[currentFrame];
    arg2.fftl = fftl;
    arg2.framePeriod = framePeriod;
    arg2.fs = fs;
    arg2.synthWave = &synthWave[currentPosition];
    arg2.waveLength = waveLength - currentPosition;
    arg2.fast = isFast;

}
#endif

standSpecgram::standSpecgram()
{
    aperiodicity = NULL;
    specgram = NULL;
    f0 = NULL;
    t = NULL;
    targetF0 = 200.0;    // とりあえず
    tLen = 0;
    fftl = 0;
    isFast = true;
    aperiodicityLength = 4;

	synthWave = NULL;
	waveLength = 0;
}

standSpecgram::~standSpecgram()
{
    destroy();
}

int standSpecgram::computeWaveFile( string_t input, bool fast )
{
    int ret = 0;
    waveFileEx wave;
    double* tmp;
    unsigned long length;

    string t_input;
    mb_conv( input, t_input );
    int check;
    check = wave.readWaveFile( t_input );

    if( check == 1 )
    {
        length = wave.getWaveLength();
        tmp = new double[length];
        wave.getWaveBuffer( tmp, &length );
        tLen = getSamplesForDIO( fs, (int)length, framePeriod );
        isFast = fast;
        if( fast )
        {
            aperiodicityLength =4;
        }
        else
        {
            aperiodicityLength = fftl;
        }
        f0 = new double[tLen];
        t  = new double[tLen];
        dio( tmp, (int)length, fs, framePeriod, t, f0 );

        fftl = getFFTLengthForStar( fs );
        specgram = new double*[tLen];
        aperiodicity = new double*[tLen];
        for( int i = 0; i < tLen; i++ )
        {
            specgram[i] = new double[fftl];
            aperiodicity[i] = new double[aperiodicityLength];
            memset( specgram[i], 0, sizeof(double) * fftl );
            memset( aperiodicity[i], 0, sizeof(double) * aperiodicityLength );
        }
        star( tmp, (int)length, fs, t, f0, specgram, isFast );
        if( isFast )
        {
            platinum( fs, f0, tLen, aperiodicity );
        }
        else
        {
            platinum_v4( tmp, (int)length, fs, t, f0, specgram, aperiodicity );
        }

        SAFE_DELETE_ARRAY( tmp );
        ret = 1;
    }
    return ret;
}

int standSpecgram::computeWaveFile( string_t input, utauParameters& parameters, bool fast )
{
    int ret = 0;
    waveFileEx wave;
    double *tmp;
    unsigned long length;

    string t_input;
    mb_conv( input, t_input );
    if( wave.readWaveFile( t_input + ".wav" ) == 1 ){

        // 諸々変数の準備＆計算
        length = wave.getWaveLength( parameters.msLeftBlank, parameters.msRightBlank );
        if( length < 1 ) length = 1;
        tmp = new double[length];
        wave.getWaveBuffer( tmp, parameters.msLeftBlank, parameters.msRightBlank, length );

        tLen = getSamplesForDIO( fs, (int)length, framePeriod );
        fftl = getFFTLengthForStar( fs );
        isFast = fast;

        f0 = new double[tLen];
        t  = new double[tLen];

        // 音量の正規化を行う．
        long index = (long)((double)fs * parameters.msFixedLength / 1000.0);
        double sum1 = 0.0, sum2 = 0.0;
        // 固定長終了位置の音量を得る．
        for( int i = index - 1024; 0 <= i && i < index + 1024 && i < length; i++ )
        {
            sum1 += tmp[i] * tmp[i];
        }
        // 左ブランク終了位置の音量を得る．
        for( int i = 0; i < 2048 && i < length; i++ )
        {
            sum2 += tmp[i] * tmp[i];
        }
        // 大きい方が正規化のための音量．
        sum1 = max(sum1, sum2);
        sum1 = VOL_NORMALIZE / sqrt( ( sum1 / (double)fftl ) );

        // WORLD 0.0.4 使用時，音量はfftl倍されてしまう．2^11 倍程度なので 16bit Audio なら許容範囲だろうか．
        sum1 *= 1 / (double)fftl;

        for( int i = 0; i < length; i++ )
        {
            tmp[i] *= sum1;
        }

        dio( tmp, (int)length, fs, framePeriod, t, f0 );

        if( isFast )
        {
            aperiodicityLength = 4;
        }
        else
        {
            aperiodicityLength = fftl;
        }
        specgram = new double*[tLen];
        aperiodicity = new double*[tLen];
        for( int i = 0; i < tLen; i++ )
        {
            specgram[i] = new double[fftl];
            aperiodicity[i] = new double[aperiodicityLength];
            memset( specgram[i], 0, sizeof(double) * fftl );
            memset( aperiodicity[i], 0, sizeof(double) * aperiodicityLength );
        }
        star( tmp, (int)length, fs, t, f0, specgram, isFast );
        if( isFast )
        {
            platinum( fs, f0, tLen, aperiodicity );
        }
        else
        {
            platinum_v4( tmp, (int)length, fs, t, f0, specgram, aperiodicity );
        }

        SAFE_DELETE_ARRAY( tmp );
        ret = 1;
    }
    return ret;
}

int standSpecgram::writeWaveFile( string_t output, long beginFrame, vector<double>* dynamics )
{
    int ret = 0;
    waveFileEx wave;

	if( synthWave )
    {
        double secOffset = (double)beginFrame * framePeriod / 1000.0;

        wave.setWaveBuffer( synthWave, (unsigned long)waveLength );
        if( dynamics )
        {
            wave.applyDynamics( *dynamics, fs, framePeriod );
        }
        wave.setOffset( secOffset );
        wave.normalize();
        string t_output;
        mb_conv( output, t_output );
        wave.writeWaveFile( t_output );
        ret = 1;
    }

    return ret;
}

double* standSpecgram::synthesizeWave( long *length )
{
    waveLength = (int)((double)tLen * fs * framePeriod / 1000.0);
    SAFE_DELETE_ARRAY( synthWave );
    synthWave = new double[waveLength];

    if( synthWave && waveLength > 0 )
    {
        // 教訓： メモリのゼロクリアは重要である．
        memset( synthWave, 0, sizeof( double ) * waveLength );
#ifdef STND_MULTI_THREAD
        // マルチスレッドが宣言されていてかつ mutex が準備されていれば２スレッドで合成．
        if( hFFTWMutex )
        {
            // 十分に処理が遅ければ破綻しない．
            computeArg arg1, arg2;
            thread_t hThread[2];
#ifdef _DEBUG
            cout << "standSpecgram::synthesizeWave; mutex created: hFFTWMutex" << endl;
#endif

            this->setArguments( arg1, arg2 );

            hThread[0] = stnd_thread_create( compute_multithread_win32, &arg1 );
            hThread[1] = stnd_thread_create( compute_multithread_win32, &arg2 );

            //WaitForMultipleObjects(2, hThread, TRUE, INFINITE);
            stnd_thread_join( hThread[0] );
            stnd_thread_join( hThread[1] );

            stnd_thread_destroy( hThread[0] );
            stnd_thread_destroy( hThread[1] );
        }
        else
        {
            if( isFast )
            {
                synthesis( f0, tLen, specgram, aperiodicity, fftl, framePeriod, fs, synthWave, waveLength );
            }
            else
            {
                synthesis_v4( f0, tLen, specgram, aperiodicity, fftl, framePeriod, fs, synthWave, waveLength );
            }
        }
#else
        if( isFast )
        {
            synthesis( f0, tLen, specgram, aperiodicity, fftl, framePeriod, fs, synthWave, waveLength );
        }
        else
        {
            synthesis_v4( f0, tLen, specgram, aperiodicity, fftl, framePeriod, fs, synthWave, waveLength );
        }
#endif
        *length = waveLength;
    }
    else
    {
        *length = 0;
    }
    return synthWave;
}



void standSpecgram::destroy()
{
    for( int i = 0; i < tLen; i++ )
    {
        SAFE_DELETE_ARRAY( aperiodicity[i] );
        SAFE_DELETE_ARRAY( specgram[i] );
    }
    SAFE_DELETE_ARRAY( aperiodicity );
    SAFE_DELETE_ARRAY( specgram );
    SAFE_DELETE_ARRAY( t );
    SAFE_DELETE_ARRAY( f0 );
    SAFE_DELETE_ARRAY( synthWave );
    tLen = 0;
    waveLength = 0;
    aperiodicityLength = 0;
    fftl = 0;
    isFast = true;
}

void standSpecgram::setFrameLength( long length, bool fast )
{
    destroy();

    tLen = length; //getSamplesForDIO( fs, (int)length, framePeriod );
    // 配列の長さが0以下だと死ぬ
    if( tLen <= 0 ) return;
    
    fftl = getFFTLengthForStar( fs );
    if( fftl <= 0 ) return;
    
    // this function does NOT initialize array values.
    f0 = new double[tLen];
    t = new double[tLen];
    aperiodicity = new double*[tLen];
    specgram = new double*[tLen];
    isFast = fast;
    if( isFast )
    {
        aperiodicityLength = 4;
    }
    else
    {
        aperiodicityLength = fftl;
    }
    for( int i = 0; i < tLen; i++ )
    {
        aperiodicity[i] = new double[aperiodicityLength];
        specgram[i] = new double[fftl];
        memset( specgram[i], 0, sizeof(double) * fftl );
        memset( aperiodicity[i], 0, sizeof(double) * aperiodicityLength );
    }
}

void standSpecgram::getFramePointer( long frame, standFrame& target )
{
    // きわどい原音設定があると範囲外になる可能性がある．
    if( frame < 0 )
    {
        frame = 0;
    }
    else if( frame >= this->tLen )
    {
        frame = this->tLen - 1;
    }
    target.aperiodicity = aperiodicity[frame];
    target.aperiodicityLength = aperiodicityLength;
    target.isFast = isFast;
    target.spectrum = specgram[frame];
    target.f0 = &(f0[frame]);
    target.t = &(t[frame]);
    target.fftl = this->fftl;
}

void standSpecgram::setFrame( long frame, standFrame& src )
{
    if( frame < 0 || frame >= this->tLen )
    {
        return;
    }
    memcpy( specgram[frame], src.spectrum, sizeof(double)*src.fftl );
    memcpy( aperiodicity[frame], src.aperiodicity, sizeof(double)*src.aperiodicityLength );

    f0[frame] = *(src.f0);
    t[frame] = *(src.t);
}
