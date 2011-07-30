#include "vConnectUtility.h"

#include <math.h>
#include <float.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <vorbis/vorbisenc.h>

#include <list>
using namespace std;

const int maxBitrate = -1;
const int minBitrate = -1;
const int averageBitrate = 80000;

const int bufSize = 4096;

/* 許容する傾きの最大値 */
const double GRAD = 2.0;
/* 変形関数の傾きに対する重み */
const double G_WEIGHT = 1.5;

const char ENCODER_TAG[] = "vConnet";

struct vorbisBuffer {
    char *p;
    int size;
};

void vConnectUtility::extractMelCepstrum(double *dst, int fftl, float *src, int cepl, fftw_complex *in, double *out, fftw_plan plan, int fs)
{
    int i;
    for(i = 0; i < cepl; i++)
    {
        in[i][0] = src[i];
        in[i][1] = 0.0;
    }
    for(; i <= fftl / 2; i++)
    {
        in[i][0] = in[i][1] = 0.0;
    }

    fftw_execute(plan);

    for(i = 0; i <= fftl / 2; i++)
    {
        out[i] = exp(out[i]);
    }

    vConnectUtility::stretchFromMelScale(dst, out, fftl / 2 + 1, fs / 2);
}

void vConnectUtility::stretchToMelScale(double *melSpectrum, const double *spectrum, int spectrumLength, int maxFrequency)
{
    double tmp = getMelScale(maxFrequency);
    for(int i = 0; i < spectrumLength; i++)
    {
        double dIndex = getFrequency((double)i / (double)spectrumLength * tmp) / (double)maxFrequency * (double)spectrumLength;
        if(dIndex <= spectrumLength-1.0){
            melSpectrum[i] = interpolateArray(dIndex, spectrum);
        }else{
            melSpectrum[i] = spectrum[spectrumLength - 1];
        }
    }
}

void vConnectUtility::stretchFromMelScale(double *spectrum, const double *melSpectrum, int spectrumLength, int maxFrequency)
{
    double tmp = getMelScale(maxFrequency);
    for(int i = 0; i < spectrumLength; i++)
    {
        double dIndex = getMelScale((double)i / (double)spectrumLength * (double)maxFrequency) / tmp * (double)spectrumLength;
        if(dIndex <= spectrumLength-1.0){
            spectrum[i] = interpolateArray(dIndex, melSpectrum);
        }else{
            spectrum[i] = melSpectrum[spectrumLength - 1];
        }
    }
}

void vConnectUtility::linearStretch(double *dst, const double *src, double ratio, int length)
{
    for(int i = 0; i < length; i++)
    {
        double dIndex = (double)i * ratio;
        int iIndex = dIndex;
        double r = dIndex - (double)iIndex;
        if(iIndex < 0 || iIndex >= length - 1) {
            dst[i] = 0.0;
        } else {
            dst[i] = pow(src[iIndex], 1.0 - r) * pow(src[iIndex], r);
        }
    }
}

double vConnectUtility::interpolateArray( double x, const double *p )
{
    int t = (int)x;
    double r = x - (double)t;
    return ( p[t] * ( 1.0 - r ) + p[t+1] * r );
}

double  vConnectUtility::getMelScale(double freq){
    double ret = 1127.01048 * log(1 + freq / 700);
    return ret;
}

double  vConnectUtility::getFrequency(double melScale){
    double ret = 700 * (exp(melScale / 1127.01048) - 1);
    return ret;
}

void vConnectUtility::extractResidual(fftw_complex *dst, const double *src, int fftLength)
{
    dst[0][0] = src[0];
    dst[0][1] = 0.0;
    for(int i = 1; i < fftLength / 2; i++) {
        dst[i][0] = src[2*i-1];
        dst[i][1] = src[2*i];
    }
    dst[fftLength/2][0] = src[fftLength-1];
    dst[fftLength/2][1] = 0.0;
}

void vConnectUtility::newOggVorbis(char **dst, int *size, const double *wave, int length, int fs)
{
    if(!dst || !size || !wave || length <= 0){ return; }

    list<vorbisBuffer*> bufferList;
    vorbisBuffer *p;
    int eos = 0;
    ogg_stream_state streamState;
    ogg_page         page;
    ogg_packet       packet, header, headerComment, headerCode;

    vorbis_info      info;
    vorbis_comment   comment;

    vorbis_dsp_state dspState;
    vorbis_block     block;

    vorbis_info_init(&info);

    if(vorbis_encode_init(&info, 1, fs, maxBitrate, averageBitrate, minBitrate))
    {
        vorbis_info_clear(&info);
        return;
    }

    vorbis_comment_init(&comment);
    vorbis_comment_add_tag(&comment, "ENCODER", ENCODER_TAG);

    vorbis_analysis_init(&dspState, &info);
    vorbis_block_init(&dspState, &block);

    srand(time(NULL));
    ogg_stream_init(&streamState, rand());

    vorbis_analysis_headerout(&dspState, &comment, &header, &headerComment, &headerCode);
    ogg_stream_packetin(&streamState, &header);
    ogg_stream_packetin(&streamState, &headerComment);
    ogg_stream_packetin(&streamState, &headerCode);

    while(!eos){
        int bytes = ogg_stream_flush(&streamState, &page);
        if(bytes == 0){ break; }

        p = new vorbisBuffer;
        p->size = page.header_len + page.body_len;
        p->p = new char[p->size];
        memcpy(p->p, page.header, page.header_len);
        memcpy(p->p + page.header_len, page.body, page.body_len);
        bufferList.push_back(p);
    }

    long i, pos = 0;
    while(!eos){
        float **buffer = vorbis_analysis_buffer(&dspState, bufSize);
        for(i = 0; i < bufSize && pos < length; i++, pos++){
            buffer[0][i] = wave[pos];
        }
        vorbis_analysis_wrote(&dspState, i);

        while(vorbis_analysis_blockout(&dspState, &block) == 1){
            vorbis_analysis(&block, NULL);
            vorbis_bitrate_addblock(&block);

            while(vorbis_bitrate_flushpacket(&dspState, &packet)){
                ogg_stream_packetin(&streamState, &packet);

                while(!eos){
                    int result = ogg_stream_pageout(&streamState, &page);
                    if(result == 0)break;

                    p = new vorbisBuffer;
                    p->size = page.header_len + page.body_len;
                    p->p = new char[p->size];
                    memcpy(p->p, page.header, page.header_len);
                    memcpy(p->p + page.header_len, page.body, page.body_len);
                    bufferList.push_back(p);

                    if(ogg_page_eos(&page)){
                        eos = 1;
                    }
                }
            }
        }
    }

    ogg_stream_clear(&streamState);
    vorbis_block_clear(&block);
    vorbis_dsp_clear(&dspState);
    vorbis_comment_clear(&comment);
    vorbis_info_clear(&info);

    *size = 0;
    for(list<vorbisBuffer*>::iterator i = bufferList.begin(); i != bufferList.end(); i++) {
        *size += (*i)->size;
    }
    *dst = new char[*size];
    char *cur = *dst;
    for(list<vorbisBuffer*>::iterator i = bufferList.begin(); i != bufferList.end(); i++) {
        // Endian 依存．
        memcpy(cur, (*i)->p, (*i)->size);
        cur += (*i)->size;
        delete[] (*i)->p;
        delete (*i);
    }
}

void vConnectUtility::_get_graduation( double* src, double* dst, int length,
                                     double *d1s, double *d2s, double *d1d, double *d2d)
{
    /* 導関数の数値を計算 */
    double *temp_spectrum = (double*)malloc(sizeof(double) * length);
    memset( temp_spectrum,0,sizeof(double)*length );

    for( int i = 1; i < length; i++ )
        temp_spectrum[i] = fabs( dst[i] - dst[i-1] );
    for( int i = 0; i < length - 1; i++ )
        d1d[i] = 0.5 * ( temp_spectrum[i] + temp_spectrum[i+1] );
    d1d[length-1] = 0.0;

    for( int i = 1; i < length; i++ )
        temp_spectrum[i] = fabs( d1d[i] - d1d[i-1] );
    for( int i = 0; i < length - 1; i++ )
        d2d[i] = 0.5 * ( temp_spectrum[i] + temp_spectrum[i+1] );
    d2d[length-1] = 0.0;

    for( int i = 1; i < length; i++ )
        temp_spectrum[i] = fabs( src[i] - src[i-1] );
    for( int i = 0; i < length - 1; i++ )
        d1s[i] = 0.5 * ( temp_spectrum[i] + temp_spectrum[i+1] );
    d1s[length-1] = 0.0;

    for( int i = 1; i < length; i++ )
        temp_spectrum[i] = fabs( d1s[i] - d1s[i-1] );
    for( int i = 0; i < length - 1; i++ )
        d2s[i] = 0.5 * ( temp_spectrum[i] + temp_spectrum[i+1] );
    d2s[length-1] = 0.0;

    free(temp_spectrum);
}

void vConnectUtility::_log_normalize_infinite( double* dst, int length )
{
    /* 正規化の結果は０～１の範囲になる． */
    int i;
    double min_v = DBL_MAX, max_v = DBL_MIN;
    for( i = 0; i < length; i++ ){
        if( dst[i] < min_v )
            min_v = dst[i];
        if( dst[i] > max_v )
            max_v = dst[i];
    }
    for( i = 0; i < length; i++ )
        dst[i] = log( dst[i] / min_v ) / log( max_v );
}

double vConnectUtility::_get_cost( double* src, double* dst, int i, int j, int n, int m, 
                       double *d1d, double *d2d, double *d1s, double *d2s)
{
    int k;
    double temp, rate, sum;

    if( n > m * GRAD || m > n * GRAD )
        return DBL_MAX;

    rate = temp = (double)n / (double)m;
    sum = 0.0;

    if( rate > 1.0 )
        rate = 1.0 / rate;
    if( m >= n ){
        for( k = 1; k <= m; k++ ){
            sum += fabs( interpolateArray( (double)i + (double)k * temp, src ) - dst[j+k] );
            sum += fabs( interpolateArray( (double)i + (double)k * temp, d1s ) / temp - d1d[j+k] );
            sum += 0.5 * fabs( interpolateArray( (double)i + (double)k * temp, d2s ) / pow( temp, 2.0 ) - d2d[j+k] );
        }
        temp = m;
        temp = sqrt( (double)( m*m + n*n ) ) / temp;
        sum *= temp;
    }else{
        for( k = 1; k <= n; k++ ){
            sum += fabs( src[i+k] - interpolateArray( (double)j + (double)k / temp, dst ) );
            sum += fabs( d1s[i+k] - interpolateArray( (double)j + (double)k / temp, d1d ) * temp );
            sum += 0.5 * fabs( d2s[i+k] - interpolateArray( (double)j + (double)k / temp, d2d ) * pow( temp, 2.0 ) );
        }
        temp = n;
        temp = sqrt( (double)( m*m + n*n ) ) / temp;
        sum *= temp;
    }
    rate = 1.0 + G_WEIGHT * pow( 1.0 - rate, 2.0 );

    return sum * rate;
}

void vConnectUtility::calculateMatching( double* T, double* H, double* src_s, double* dst_s, int length )
{
    int i, j, k, l, n, m, tx, ty;
    double **dpMap;
    double tempd, g1, g2;
    int    **pathX, **pathY;

    double* src = (double*)malloc( sizeof(double)*length );
    double* dst = (double*)malloc( sizeof(double)*length );
    double *d1d = (double*)malloc( sizeof(double)*length );
    double *d2d = (double*)malloc( sizeof(double)*length );
    double *d1s = (double*)malloc( sizeof(double)*length );
    double *d2s = (double*)malloc( sizeof(double)*length );

    memcpy( src, src_s, sizeof(double)*length );
    memcpy( dst, dst_s, sizeof(double)*length );

    /* メモリの確保と値の初期化 */
    dpMap = (double **)malloc( sizeof(double *) * length );
    pathX = (int **)malloc( sizeof(int *) * length );
    pathY = (int **)malloc( sizeof(int *) * length );
    for( i = 0; i < length; i++ ){
        dpMap[i] = (double *)malloc( sizeof(double) * 1100); //length );
        for( j = 0; j < 1100; j++ ) //length; j++ )
            dpMap[i][j] = 1000000000.0;
        pathX[i] = (int *)malloc( sizeof(int) * 1100); //length );
        pathY[i] = (int *)malloc( sizeof(int) * 1100); //length );
        memset( pathX[i], -1, sizeof(int) * 1100); //length );
        memset( pathY[i], -1, sizeof(int) * 1100); //length );
    }
    pathX[0][512] = pathY[0][512] = 0;
    dpMap[0][512] = fabs( dst[0] - src[0] );


    /* 導関数を算出 */
    _get_graduation( src, dst, length, d1s, d2s, d1d, d2d );

    /* 最短経路を求めてマッチング */
    for( i = 0; i < length; i++ ){
        for( j = 0; j < length; j++ ){
            if( abs(i - j) > 48 )  /* 領域を直指定するなんて... */
                continue;
            /* 現在点における傾き */
            tx = pathX[i][j-i+512]; ty = pathY[i][j-i+512];
            if( i == tx || j == ty )
                g1 = 1.0;
            else
                g1 = (double)( j - ty ) / (double)( i - tx );
            /* 分割はあまり増やしても意味が無いみたい */
            for( n = 1; n < 8; n++ ){
                if( i + n >= length )
                    break;
                for( m = 1; m < 8; m++ ){
                    if( j + m >= length )
                        break;
                    /* 目標点における傾き */
                    g2 = (double)m / (double)n;
                    if( g2 > g1 )
                        tempd = dpMap[i][j-i+512] + _get_cost( src, dst, i, j, n, m, d1d, d2d, d1s, d2s ) * g2 / g1;
                    else
                        tempd = dpMap[i][j-i+512] + _get_cost( src, dst, i, j, n, m, d1d, d2d, d1s, d2s ) * g1 / g2;
                    /* 最大値の更新 */
                    if( i + n < length && j + m < length && dpMap[i+n][j+m-(i+n)+512] > tempd ){
                        dpMap[i+n][j+m-(i+n)+512] = tempd;
                        pathX[i+n][j+m-(i+n)+512] = i;
                        pathY[i+n][j+m-(i+n)+512] = j;
                    }
                }
            }
        }
    }

    T[0] = 0.0;
    l = j = length - 1;

    while( l > 0 && j > 0 ){
        tx = pathX[l][j-l+512]; ty = pathY[l][j-l+512];
        for( k = l; k > tx; k-- )
            T[k] = (double)j - (double)( l - k ) * (double)( j - ty ) / (double)( l - tx );
        l = tx; j = ty;
    }

/*    if(H){
        applyStretching( T, dst, length );
        for( i = 0; i < length; i++ )
            H[i] = src[i] - dst[i];
    }
*/
    /* メモリのお掃除 */
    for( i = 0; i < length; i++ ){
        free( dpMap[i] );
        free( pathX[i] );
        free( pathY[i] );
    }
    free( dpMap );
    free( pathX );
    free( pathY );
    free( src );
    free( dst );
    free( d1d );
    free( d2d );
    free( d1s );
    free( d2s );

}
