#include "Star.h"
#include "World.h"
#include "Dio.h"
#include "MatlabFunctions.h"
#include "FFTSet.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef max
#define max(a,b) (((a)<(b))?(b):(a))
#endif

using namespace stand::math;
using namespace stand::math::world;

static void starGeneralBody(const double *x, int xLen, int fs, double f0, double t, int fftl, double * sliceSTAR, FFTSet &forward);

int stand::math::world::FFTLengthForStar(int fs)
{
    return (int)pow(2.0, 1.0+(int)(log(3.0*fs/FLOOR_F0+1) / log(2.0)));
}

void stand::math::world::star(const double *x, int xLen, int fs, double *timeAxis, double *f0,
         double **specgram)
{
    int i;
    double framePeriod = (timeAxis[1]-timeAxis[0])*1000.0;
    double f0LowLimit = FLOOR_F0; // F0がFLOOR Hz以下の場合は無声音として扱う
    double currentF0;

    int	fftl = (int)pow(2.0, 1.0+(int)(log(3.0*fs/f0LowLimit+1) / log(2.0)));
    int tLen = samplesForDio(fs, xLen, framePeriod);

    FFTSet forward(fftl, FFTSet::FFTW_R2C);

    for(i = 0;i < tLen;i++)
    {
        currentF0 = f0[i] <= FLOOR_F0 ? DEFAULT_F0 : f0[i];
        starGeneralBody(x, xLen, fs, currentF0, timeAxis[i], fftl, specgram[i], forward);
    }
}


void starGeneralBody(const double *x, int xLen, int fs, double f0, double t, int fftl, double * sliceSTAR, FFTSet &forward)
{
    int i,j;

    int *baseIndex, *index; // i付きのも包含する (Matlab版参照)
    int nFragment = (int)(0.5 + 3.0*(double)fs/f0/2.0);
    baseIndex = (int *)malloc(sizeof(int) * (nFragment*2+1));
    index  = (int *)malloc(sizeof(int) * (nFragment*2+1));

    for(i = -nFragment, j = 0;i <= nFragment;i++, j++)
        baseIndex[j] = i;
    for(i = 0;i <= nFragment*2;i++)
        index[i]  = min(xLen, max(1, matlab_round(t*(double)fs+1+baseIndex[i]) ) ) - 1;

    double *segment, *window;
    double position, average;
    segment  = (double *)malloc(sizeof(double) * (nFragment*2+1));
    window   = (double *)malloc(sizeof(double) * (nFragment*2+1));
    average  = 0.0;
    for(i = 0;i <= nFragment*2;i++)
    {
        segment[i]  = x[index[i]];
        position  = (double)(baseIndex[i]/(double)fs/(3.0/2.0) ) +
            (t*(double)fs - (double)(matlab_round(t*(double)fs))) / (double)fs;
        window[i]  = 0.5*cos(PI*position*f0) +0.5;
        average  += window[i]*window[i];
    }
    average  = sqrt(average);
    for(i = 0;i <= nFragment*2;i++) window[i]  /= average;

    // 波形のスペクトルを計算
    double				*waveform;
    double				*powerSpec;
    waveform  = (double *)forward.in();
    powerSpec = (double *)malloc(sizeof(double) * fftl);

    fftw_complex		*ySpec;		// スペクトル
    ySpec = (fftw_complex *)forward.out();

    // パワースペクトルの計算
    for(i = 0;i <= nFragment*2;i++)
        waveform[i] = segment[i] * window[i];
    for(;i < fftl;i++)
        waveform[i] = 0.0;
    forward.execute();
    for(i = 1;i <= fftl/2;i++)
        powerSpec[i] = ySpec[i][0]*ySpec[i][0] + ySpec[i][1]*ySpec[i][1];
    powerSpec[0] = powerSpec[1];

    free(segment);
    free(window);
    free(baseIndex); free(index);

    // adroit smoothing
    // 無駄を仕分け．
    int limit;
    limit = (int)(f0 / (fs/(double)fftl))+1;
    double *dSpectrum, dFrequencyAxis, dShift;
    dSpectrum		= (double *)malloc(sizeof(double) * (fftl+limit*2 + 1) );

    // 計算コストを少しでも減らす
    dFrequencyAxis = -((double)limit-0.5)*(double)fs/(double)fftl;
    dShift = (double)fs/(double)fftl;

    for(i = 0;i < limit;i++)
        dSpectrum[i] = powerSpec[limit-i];
    for(j = 0;i < fftl/2+limit;i++,j++)
        dSpectrum[i] = powerSpec[j];
    for(j=1;i < fftl/2+limit*2+1;i++,j++)
        dSpectrum[i] = powerSpec[fftl/2-j];

    double *dSegment, *centers;
    dSegment	= (double *)malloc(sizeof(double) * fftl*2);
    centers		= (double *)malloc(sizeof(double) * (fftl/2 + 1) );

    dSegment[0]	= log(dSpectrum[0])*(double)fs/(double)fftl;
    for(i = 1;i < fftl/2+limit*2+1;i++)
        dSegment[i] = log(dSpectrum[i])*(double)fs/(double)fftl + dSegment[i-1];

    for(i = 0;i <= fftl/2;i++)
        centers[i] = (double)i / (double)fftl * (double)fs - f0/2.0;

    double *lowLevels, *highLevels;
    lowLevels  = (double *)malloc(sizeof(double) * (fftl/2+1));
    highLevels = (double *)malloc(sizeof(double) * (fftl/2+1));
    interp1Q(dFrequencyAxis, dShift, dSegment, fftl/2+limit*2+1, centers, (fftl/2 + 1), lowLevels);
    for(i = 0;i <= fftl/2;i++)
        centers[i] += f0;
    interp1Q(dFrequencyAxis, dShift, dSegment, fftl/2+limit*2+1, centers, (fftl/2 + 1), highLevels);

    for(i = 0;i <= fftl/2;i++)
    {
        sliceSTAR[i] = exp( (highLevels[i]-lowLevels[i])/f0);
    }

    free(lowLevels); free(highLevels);
    free(dSegment); free(centers);
    free(dSpectrum);
    free(powerSpec);
}
