/*
 *
 *    matching.h
 *                              (c) HAL 2010-           
 *
 *   void calculateMatcing( double* T, double* H,       
 *               double* src, double* dst, int length );
 *
 *    Calculate matching function between src & dst;    
 *  where T is the destination for writing result,      
 *  H is difference between src & 'stretched' dst.      
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#include "matching.h"

/* 導関数群 */
double *d1s, *d2s, *d3s;
double *d1d, *d2d, *d3d;

double interpolateArray( double x, const double *p )
{
    int t = (int)x;
    double r = x - (double)t;
    return ( p[t] * ( 1.0 - r ) + p[t+1] * r );
}

void applyStretching( double *T, double *target, int length )
{
    int i;
    double *temp_spectrum = new double[length];
    for( i = 0; i < length; i++ )
        temp_spectrum[i] = interpolateArray( T[i], target );
    memcpy( target, temp_spectrum, sizeof(double) * length );
    delete[] temp_spectrum;
}

void getGraduation( double* src, double* dst, int length )
{
    /* 導関数の数値を計算 */
    double *temp_spectrum = (double*)malloc(sizeof(double)*length);
    memset( temp_spectrum,0,sizeof(double)*length );
    memset( d1s,0,sizeof(double)*length );
    memset( d2s,0,sizeof(double)*length );
    memset( d3s,0,sizeof(double)*length );
    memset( d1d,0,sizeof(double)*length );
    memset( d2d,0,sizeof(double)*length );
    memset( d3d,0,sizeof(double)*length );
    for( int i = 1; i < length; i++ )
        temp_spectrum[i] = fabs( dst[i] - dst[i-1] );
    for( int i = 0; i < length - 1; i++ )
        d1d[i] = 0.5 * ( temp_spectrum[i] + temp_spectrum[i+1] );

    for( int i = 1; i < length; i++ )
        temp_spectrum[i] = fabs( d1d[i] - d1d[i-1] );
    for( int i = 0; i < length - 1; i++ )
        d2d[i] = 0.5 * ( temp_spectrum[i] + temp_spectrum[i+1] );

    for( int i = 1; i < length; i++ )
        temp_spectrum[i] = fabs( d2d[i] - d2d[i-1] );
    for( int i = 0; i < length - 1; i++ )
        d3d[i] = 0.5 * ( temp_spectrum[i] + temp_spectrum[i+1] );

    for( int i = 1; i < length; i++ )
        temp_spectrum[i] = fabs( src[i] - src[i-1] );
    for( int i = 0; i < length - 1; i++ )
        d1s[i] = 0.5 * ( temp_spectrum[i] + temp_spectrum[i+1] );

    for( int i = 1; i < length; i++ )
        temp_spectrum[i] = fabs( d1s[i] - d1s[i-1] );
    for( int i = 0; i < length - 1; i++ )
        d2s[i] = 0.5 * ( temp_spectrum[i] + temp_spectrum[i+1] );

    for( int i = 1; i < length; i++ )
        temp_spectrum[i] = fabs( d2s[i] - d2s[i-1] );
    for( int i = 0; i < length - 1; i++ )
        d3s[i] = 0.5 * ( temp_spectrum[i] + temp_spectrum[i+1] );
    free(temp_spectrum);
}

void logNormalizeInfinite( double* target, int length )
{
    /* 正規化の結果は０～１の範囲になる． */
    int i;
    double min_v = DBL_MAX, max_v = DBL_MIN;
    for( i = 0; i < length; i++ ){
        if( target[i] < min_v )
            min_v = target[i];
        if( target[i] > max_v )
            max_v = target[i];
    }
    for( i = 0; i < length; i++ )
        target[i] = log( target[i] / min_v ) / log( max_v );
}

double getCost( double* src, double* dst, int i, int j, int n, int m )
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
    rate = 1.0 + MAGIC_K * pow( 1.0 - rate, 2.0 );

    return sum * rate;
}

void calculateMatching( double* T, double* H, double* src_s, double* dst_s, int length )
{
    int i, j, k, l, n, m, tx, ty;
    double **dpMap;
    double tempd, g1, g2;
    int    **pathX, **pathY;

    double* src = (double*)malloc( sizeof(double)*length );
    double* dst = (double*)malloc( sizeof(double)*length );

    d1d = (double*)malloc( sizeof(double)*length );
    d2d = (double*)malloc( sizeof(double)*length );
    d3d = (double*)malloc( sizeof(double)*length );
    d1s = (double*)malloc( sizeof(double)*length );
    d2s = (double*)malloc( sizeof(double)*length );
    d3s = (double*)malloc( sizeof(double)*length );

    memcpy( src, src_s, sizeof(double)*length );
    memcpy( dst, dst_s, sizeof(double)*length );

    if( length > MAX_MAT_LENGTH ){
        fprintf( stderr, "error; length( =%d ) is over MAX_MAT_LENGTH: %d\n", length, MAX_MAT_LENGTH );
        return;
    }

    /* 無限大ノルムで正規化して対数をとっておこう */
    logNormalizeInfinite( src, length );
    logNormalizeInfinite( dst, length );

    /* メモリの確保と値の初期化 */
    dpMap = (double **)malloc( sizeof(double *) * length );
    pathX = (int **)malloc( sizeof(int *) * length );
    pathY = (int **)malloc( sizeof(int *) * length );
    for( i = 0; i < length; i++ ){
        dpMap[i] = (double *)malloc( sizeof(double) * length );
        for( j = 0; j < length; j++ )
            dpMap[i][j] = 10000000.0;
        pathX[i] = (int *)malloc( sizeof(int) * length );
        pathY[i] = (int *)malloc( sizeof(int) * length );
        memset( pathX[i], -1, sizeof(int) * length );
        memset( pathY[i], -1, sizeof(int) * length );
    }
    pathX[0][0] = pathY[0][0] = 0;
    dpMap[0][0] = fabs( dst[0] - src[0] );


    /* 導関数を算出 */
    getGraduation( src, dst, length );

    /* 最短経路を求めてマッチング */
    for( i = 0; i < length; i++ ){
        for( j = 0; j < length; j++ ){
            if( abs(i - j) > 256 )  /* 領域を直指定するなんて... */
                continue;
            /* 現在点における傾き */
            tx = pathX[i][j]; ty = pathY[i][j];
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
                        tempd = dpMap[i][j] + getCost( src, dst, i, j, n, m ) * g2 / g1;
                    else
                        tempd = dpMap[i][j] + getCost( src, dst, i, j, n, m ) * g1 / g2;
                    /* 最大値の更新 */
                    if( i + n < length && j + m < length && dpMap[i+n][j+m] > tempd ){
                        dpMap[i+n][j+m] = tempd;
                        pathX[i+n][j+m] = i;
                        pathY[i+n][j+m] = j;
                    }
                }
            }
        }
    }

    T[0] = 0.0;
    l = j = length - 1;

    while( l > 0 && j > 0 ){
        tx = pathX[l][j]; ty = pathY[l][j];
        for( k = l; k > tx; k-- )
            T[k] = (double)j - (double)( l - k ) * (double)( j - ty ) / (double)( l - tx );
        l = tx; j = ty;
    }

    applyStretching( T, dst, length );
    for( i = 0; i < length; i++ )
        H[i] = src[i] - dst[i];

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
    free( d3d );
    free( d1s );
    free( d2s );
    free( d3s );

}

/*
 *
 * double dp_matching(double *path, double *src, int length_s, double *dst, int length_d);
 *
 * This function calculates dynamic programming matching.
 * It returns distance between 1 dimentional data arrays.
 * And path will be tha matching between them.
 *
 */

double dp_matching(int *path, double *src, int length_s, double *dst, int length_d, int width)
{
    double ret = 0, tmp;
    double **matrix;
    int **pathX, **pathY, i, j ,k;

    // invalid values.
    if(!path || !dst || !src || length_s < 1 || length_d < 1)
        return ret;

    // allocate memory for DP matching.
    matrix = (double**)malloc(sizeof(double*) * length_d);
    pathX  = (   int**)malloc(sizeof(   int*) * length_d);
    pathY  = (   int**)malloc(sizeof(   int*) * length_d);
    for(i = 0; i < length_d; i++){
        matrix[i] = (double*)malloc(sizeof(double) * length_s);
        pathX[i]  = (   int*)malloc(sizeof(   int) * length_s);
        pathY[i]  = (   int*)malloc(sizeof(   int) * length_s);
        memset(matrix[i], 0, sizeof(double) * length_s);
        memset(pathX[i] , 0, sizeof(   int) * length_s);
        memset(pathY[i] , 0, sizeof(   int) * length_s);
    }
    memset(path, 0, sizeof(int) * length_s);

    /*================= DP matching ==================*/
    
    for(i = 0; i < length_d; i++){
        for(j = 0; j < length_s; j++){
            // input invalid value when out of range.
            if(abs(i - j) > width){
                matrix[i][j] = DBL_MAX;
                pathX[i][j] = pathY[i][j] = -1;
                continue;
            }
            tmp = fabs(dst[i] - src[j]);
            if(i){
                matrix[i][j] = matrix[i-1][j];
                pathX[i][j] = i - 1;
                pathY[i][j] = j;
            }
            if(j){
                if(!matrix[i][j] || matrix[i][j] > matrix[i][j-1]){
                    matrix[i][j] = matrix[i][j-1];
                    pathX[i][j] = i;
                    pathY[i][j] = j - 1;
                }
            }
            if(i && j){
                if(matrix[i][j] > matrix[i-1][j-1]){
                    matrix[i][j] = matrix[i-1][j-1];
                    pathX[i][j] = i - 1;
                    pathY[i][j] = j - 1;
                }
            }
            matrix[i][j] += tmp;
        }
    }

    ret = matrix[length_d-1][length_s-1];

    /* Back step */

    i = length_d - 1;
    j = length_s - 1;
    while(i != -1 && j != -1 && (i || j) ){
        printf("i = %d, j = %d\n", i, j);
        path[j]=i;
        // inclement
        k = i;
        i = pathX[i][j];
        j = pathY[k][j];
    }

    /* Release allocated memory */
    for(i = 0; i < length_d; i++){
        free(matrix[i]);
        free(pathX[i]);
        free(pathY[i]);
    }
    free(matrix);
    free(pathX);
    free(pathY);

    return ret;
}

