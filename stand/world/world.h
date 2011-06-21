/*
 *
 * 音声分析合成法 WORLD by M. Morise
 *
 *    WORLD.h
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
 *
 */

// 現状で分かっているバグ
// decimateForF0 : 開始直後・終了間際4サンプルくらいに誤差が入ります．
#ifndef __world_h__
#define __world_h__

#include <algorithm>
#include <fftw3.h>
#include <stdlib.h>
#include <math.h>
#include "../stand.h"

#ifndef PI
#define PI 3.1415926535897932384
#endif

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifndef __GNUC__
// windowsならでは
#pragma warning( disable : 4996 )
#pragma comment(lib, "libfftw3-3.lib")
//#pragma comment(lib, "libfftw3f-3.lib")
//#pragma comment(lib, "libfftw3l-3.lib")
#endif

#define MAX_FFT_LENGTH 2048
#define FLOOR 71.0
#define FLOOR_F0 71.0
#define DEFAULT_F0 150.0
#define LOW_LIMIT 65.0
// 71は，fs: 44100においてFFT長を2048にできる下限．
// 70 Hzにすると4096点必要になる．

// F0推定法 DIO : Distributed Inline-filter Operation
void dio(double *x, int xLen, int fs, double framePeriod, 
         double *timeAxis, double *f0);
int getSamplesForDIO(int fs, int xLen, double framePeriod);

// スペクトル包絡推定法 STAR : Synchronous Technique and Adroit Restoration
int getFFTLengthForStar(int fs);
void star(double *x, int xLen, int fs, double *timeAxis, double *f0,
          double **specgram, bool mode);
void starGeneralBody(double *x, int xLen, int fs, double f0, double t, int fftl,
                               double * sliceSTAR, double *waveform, double *powerSpec, fftw_complex *ySpec, fftw_plan *forwardFFT);

// 非周期性指標推定法 PLATINUM : 名称未定
void platinum(int fs, double *f0, int tLen,
              double **aperiodicity);
void calculateAperiodicity(double *aperiodicity, int fftl, int fs, double *periodicSpec);

// 非周期性指標推定法 PLATINUM from WORLD0.0.4
void platinum_v4(double *x, int xLen, int fs, double *timeAxis, double *f0, double **specgram, 
		 double **residualSpecgram);

// WORLD Synthesis
void synthesis(double *f0, int tLen, double **specgram, double **aperiodicity, int fftl, double framePeriod, int fs, 
               double *synthesisOut, int xLen);
void getMinimumPhaseSpectrum(double *inputSpec, fftw_complex *spectrum, fftw_complex *cepstrum, int fftl, fftw_plan forward, fftw_plan inverse);

// WORLD Synthesis 0.0.4
void synthesis_v4(double *f0, int tLen, double **specgram, double **residualSpecgram, int fftl, double framePeriod, int fs, 
               double *synthesisOut, int xLen);

//------------------------------------------------------------------------------------
// Matlab 関数の移植

double std_mat(double *x, int xLen);

void inv(double **r, int n, double **invr);

void fftfilt(double *x, int xlen, double *h, int hlen, int fftl, double *y);

// ガウス雑音発生
float randn(void);

void histc(double *x, int xLen, double *y, int yLen, int *index);

void interp1(double *t, double *y, int iLen, double *t1, int oLen, double *y1);

long decimateForF0(double *x, int xLen, double *y, int r);

void filterForDecimate(double *x, int xLen, double *y, int r);

// matlabに順ずる丸め
// gccのmath.hにあるdouble round(double)と被るので名前が変わってますです
int c_round(double x);

void diff(double *x, int xLength, double *ans);

// 河原先生作成のMatlab関数interp1Hを移植．
// 基本的には同じだが，配列の要素数を明示的に指定する必要がある．
void interp1Q(double x, double shift, double *y, int xLength, double *xi, int xiLength, double *ans);


#endif
