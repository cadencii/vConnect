#include "SmootMatching.h"

#include "MathSet.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

using namespace stand::math;

//! @brief 一回微分と二階微分を計算します．
static void get_graduation(double* src, double* dst, int length,
                            double *d1s, double *d2s, double *d1d, double *d2d);

//! @brief 対数軸上で最大値ゼロで正規化します．
static void log_normalize_infinite( double* dst, int length );

//! @brief 写像関数を DP で求める際の枝の重みを計算します．
static double get_cost( double* src, double* dst, int i, int j, int n, int m,
                         double *d1d, double *d2d, double *d1s, double *d2s);

/* 許容する傾きの最大値 */
const double GRAD = 2.0;
/* 変形関数の傾きに対する重み */
const double G_WEIGHT = 1.5;

void stand::math::applyStretching( double *T, double *target, int length )
{
    int i;
    double *tmp = (double *)malloc(sizeof(double) * length);
    for( i = 0; i < length; i++ )
        tmp[i] = interpolateArray( T[i], target );
    memcpy( target, tmp, sizeof(double) * length );
    free(tmp);
}

void get_graduation( double* src, double* dst, int length,
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

void log_normalize_infinite( double* dst, int length )
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

double get_cost( double* src, double* dst, int i, int j, int n, int m,
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

void stand::math::smoothMatching(double* T, double* H, double* src_s, double* dst_s, int length)
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
    get_graduation( src, dst, length, d1s, d2s, d1d, d2d );

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
                        tempd = dpMap[i][j-i+512] + get_cost( src, dst, i, j, n, m, d1d, d2d, d1s, d2s ) * g2 / g1;
                    else
                        tempd = dpMap[i][j-i+512] + get_cost( src, dst, i, j, n, m, d1d, d2d, d1s, d2s ) * g1 / g2;
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

    H[0] = 0.0;
    l = j = length - 1;

    while( l > 0 && j > 0 ){
        tx = pathX[l][j-l+512]; ty = pathY[l][j-l+512];
        for( k = j; k > ty; k-- )
            H[k] = (double)j - (double)( j - k ) * (double)( l - tx ) / (double)( j - ty );
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

