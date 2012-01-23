#include "Dio.h"
#include "FFTSet.h"
#include "World.h"
#include "MatlabFunctions.h"

#include <math.h>
#include <string.h>

// min, max macro
#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef max
#define max(a,b) (((a)<(b))?(b):(a))
#endif

using namespace stand::math;
using namespace stand::math::world;

// These functions are only used for DIO.
// You should not modify them.
static void rawEventByDio(double boundaryF0, double fs, fftw_complex *xSpec, int xLength, int fftl, double shiftTime, double f0Floor, double f0Ceil, double *timeAxis, int tLen,
                   double *f0Deviations, double *interpolatedF0);
static void zeroCrossingEngine(double *x, int xLen, double fs,
                        double *eLocations, double *iLocations, double *intervals, int *iLen);
static long decimateForF0(double *x, int xLen, double *y, int r);
static void filterForDecimate(double *x, int xLen, double *y, int r);
static void nuttallWindow(int yLen, double *y);
static void postprocessing(double framePeriod, double f0Floor, int candidates, int xLen, int fs,
                    double **f0Map, double *bestF0, double *f0);

void stand::math::world::dio(const double *x, int xLen, int fs, double framePeriod, double *timeAxis, double *f0)
{
    int i,j;

    // 初期条件 (改良したい人はここから頑張って)
    double f0Floor = DIO_FLOOR_F0;
    double f0Ceil = DIO_CEIL_F0;
    double channelsInOctave = DIO_CHANNNELS;
    double targetFs = DIO_TARGET_FS;

    // 基礎パラメタの計算
    int decimationRatio = (int)(fs/targetFs);
    double fss = (double)fs/(double)decimationRatio;
    int nBands = (int)(log((double)f0Ceil/(double)f0Floor)/log(2.0) * channelsInOctave);

    // ここも基礎パラメタ
    double * boundaryF0List = (double *)malloc(sizeof(double) * (nBands+1));
    for(i = 0;i <= nBands;i++)
        boundaryF0List[i] = f0Floor*pow(2.0, i/channelsInOctave);

    // fft Lengthの計算
    int yLen = (1 + (int)(xLen/decimationRatio));
    int fftl = (int)pow(2.0, 1.0 + (int)(log((double)yLen +
        (double)(4*(int)(1.0 + (double)fs/boundaryF0List[0]/2.0)) ) / log(2.0)));

    FFTSet forward(fftl, FFTSet::FFTW_R2C);
    double *y = (double *)forward.in();
    fftw_complex *ySpec = (fftw_complex *)forward.out(); // スペクトル

    // ダウンサンプリング
    decimateForF0(x, xLen, y, decimationRatio);

    // 直流成分の除去 y = y - mean(y)
    double meanY = 0.0;
    for(i = 0;i < yLen;i++)			meanY += y[i];
    meanY /= (double)yLen;
    for(i = 0;i < yLen;i++)			y[i] -= meanY;
    for(i = yLen; i < fftl;i++)		y[i] = 0.0;

    // 中間データの保存用
    int		tLen; // F0軌跡のサンプル数
    tLen = samplesForDio(fs, xLen, framePeriod); // debug
    int lengthInMs = 1 + (int)((double)xLen/(double)fs*1000.0);
    double **stabilityMap, ** f0Map; // f0mapに候補が全て入るので，結果に納得できない場合は，f0Mapを直接操作する．
    stabilityMap = (double **)malloc(sizeof(double *) * (nBands+1));
    f0Map		 = (double **)malloc(sizeof(double *) * (nBands+1));
    for(i = 0;i <= nBands;i++)
    {
        stabilityMap[i] = (double *)malloc(sizeof(double) * tLen);
        f0Map[i]		= (double *)malloc(sizeof(double) * tLen);
    }

    // 波形のスペクトルを事前に計算（ここは高速化の余地有り）
    forward.execute();

    // temporal values
    double *	interpolatedF0;
    double *	f0Deviations;
    interpolatedF0 = (double *) malloc(sizeof(double) * lengthInMs);
    f0Deviations   = (double *) malloc(sizeof(double) * lengthInMs);

    for(i = 0;i < tLen;i++)
        timeAxis[i] = (double)i * framePeriod/1000.0;

    // イベントの計算 (4つのゼロ交差．詳しくは論文にて)
    for(i = 0;i <= nBands;i++)
    {
        rawEventByDio(boundaryF0List[i], fss, ySpec, yLen, fftl, framePeriod/1000.0, f0Floor, f0Ceil, timeAxis, tLen,
            f0Deviations, interpolatedF0);
        for(j = 0;j < tLen;j++)
        {
            stabilityMap[i][j] = f0Deviations[j] / (interpolatedF0[j]+0.00000001);
            f0Map[i][j]        = interpolatedF0[j];
        }
    }

    // ベスト候補の選定 (基本波らしさを使い一意に決める)
    double *bestF0;
    bestF0 = (double *)malloc(sizeof(double) * tLen);

    double tmp;
    for(i = 0;i < tLen;i++)
    {
        tmp = stabilityMap[0][i];
        bestF0[i] = f0Map[0][i];
        for(j = 1;j <= nBands;j++)
        {
            if(tmp > stabilityMap[j][i])
            {
                tmp = stabilityMap[j][i];
                bestF0[i] = f0Map[j][i];
            }
        }
    }

    // 後処理 (第一候補と候補マップから最適なパスを探す)
    postprocessing(framePeriod/1000.0, f0Floor, nBands+1, xLen, fs, f0Map, bestF0, f0);

    // お片づけ(メモリの開放)
    free(bestF0);
    free(interpolatedF0);
    free(f0Deviations);
    for(i = 0;i <= nBands;i++)
    {
        free(stabilityMap[i]);
        free(f0Map[i]);
    }
    free(stabilityMap);
    free(f0Map);
    free(boundaryF0List);
}

int stand::math::world::samplesForDio(int fs, int xLen, double framePeriod)
{
    return (int)((double)xLen / (double)fs / (framePeriod/1000.0) ) + 1;
}


// イベント数があったか判定
// longの範囲を超えてしまったので苦肉の策
int checkEvent(int x)
{
    if(x > 0) return 1;
    return 0;
}

// 後処理（4ステップ）
void postprocessing(double framePeriod, double f0Floor, int candidates, int xLen, int fs, double **f0Map, double *bestF0,
                    double *f0)
{
    int i, j, k;
    int voiceRangeMinimum = (int)(0.5 + 1.0/framePeriod/f0Floor);
    int f0Len = (int)((double)xLen / (double)fs / framePeriod) + 1;
//	double allowedRange = 0.1; // これは5 msecの基準なのでframePeriodに併せて調整する．
    double allowedRange = 0.1 * framePeriod/0.005; // これは5 msecの基準なのでframePeriodに併せて調整する．

    // メモリ節約はできるけど，どうせ少量なのでデバッグのしやすさを優先
    double *f0Base;
    f0Base = (double *)malloc(sizeof(double) * f0Len);
    double *f0Step1;
    f0Step1 = (double *)malloc(sizeof(double) * f0Len);
    double *f0Step2;
    f0Step2 = (double *)malloc(sizeof(double) * f0Len);
    double *f0Step3;
    f0Step3 = (double *)malloc(sizeof(double) * f0Len);
    double *f0Step4;
    f0Step4 = (double *)malloc(sizeof(double) * f0Len);

    // まずは初期化
    for(i = 0;i < voiceRangeMinimum;i++) f0Base[i] = 0;
    for(;i < f0Len-voiceRangeMinimum;i++) f0Base[i] = bestF0[i];
    for(;i < f0Len;i++) f0Base[i] = 0;
    for(i = 0;i < f0Len;i++) f0Step1[i] = 0.0;

    // 第一のステップ (F0の跳躍防止)
    for(i = voiceRangeMinimum;i < f0Len;i++)
        if(fabs((f0Base[i]-f0Base[i-1])/(0.00001+f0Base[i]) ) < allowedRange)
            f0Step1[i] = f0Base[i];

    // 第二のステップ (無声区間の切り離し)
    for(i = 0;i < f0Len;i++) f0Step2[i] = f0Step1[i];
    for(i = voiceRangeMinimum;i < f0Len;i++)
    {
        for(j = 0;j < voiceRangeMinimum;j++)
        {
            if(f0Step1[i-j] == 0)
            {
                f0Step2[i] = 0.0;
                break;
            }
        }
    }

    // 島数の検出
    int *positiveIndex, *negativeIndex;
    positiveIndex = (int *)malloc(sizeof(int) * f0Len);
    negativeIndex = (int *)malloc(sizeof(int) * f0Len);
    int positiveCount, negativeCount;
    positiveCount = negativeCount = 0;
    for(i = 1;i < f0Len;i++)
    {
        if(f0Step2[i] == 0 && f0Step2[i-1] != 0)
            negativeIndex[negativeCount++] = i-1;
        else if (f0Step2[i-1] == 0 && f0Step2[i] != 0)
            positiveIndex[positiveCount++] = i;
    }

    // ステップ3（前向き補正）
    double refValue1, refValue2, bestError, errorValue;
    for(i = 0;i < f0Len;i++) f0Step3[i] = f0Step2[i];
    for(i = 0;i < negativeCount;i++)
    {
        for(j = negativeIndex[i];j < f0Len-1;j++)
        {
            refValue1 = f0Step3[j]*2 - f0Step3[j-1];
            refValue2 = f0Step3[j];
//			bestError = fabs(refValue - f0Map[0][j+1]);
            bestError = min(fabs(refValue1 - f0Map[0][j+1]), fabs(refValue2 - f0Map[0][j+1]));
            for(k = 1;k < candidates;k++)
            {
//				errorValue = fabs(refValue - f0Map[k][j+1]);
                errorValue = min(fabs(refValue1 - f0Map[k][j+1]), fabs(refValue2 - f0Map[k][j+1]));
                if(errorValue < bestError)
                {
                    bestError = errorValue;
                    f0Step3[j+1] = f0Map[k][j+1];
                }
            }
//			if(bestError / (refValue+0.0001) > allowedRange)
            if(min(bestError / (refValue1+0.0001), bestError / (refValue2+0.0001)) > allowedRange)
            {
                f0Step3[j+1] = 0.0;
                break;
            }
            if(i != negativeCount && j == positiveIndex[i+1]-1)
            {
                negativeIndex[j] = j;
                break;
            }
        }
    }

    // ステップ4（後向き補正）
    for(i = 0;i < f0Len;i++) f0Step4[i] = f0Step3[i];
    for(i = positiveCount-1;i >= 0;i--)
    {
        for(j = positiveIndex[i]+1;j > 1;j--)
        {
            refValue1 = f0Step4[j]*2 - f0Step4[j-1];
            refValue2 = f0Step4[j];
//			refValue = f0Step4[j]*2 - f0Step4[j+1];
            bestError = min(fabs(refValue1 - f0Map[0][j+1]), fabs(refValue2 - f0Map[0][j+1]));
//			bestError = fabs(refValue - f0Map[0][j-1]);
            for(k = 1;k < candidates;k++)
            {
                errorValue = min(fabs(refValue1 - f0Map[k][j-1]), fabs(refValue2 - f0Map[k][j-1]));
//				errorValue = fabs(refValue - f0Map[k][j-1]);
                if(min(bestError / (refValue1+0.0001), bestError / (refValue2+0.0001)) > allowedRange)
//				if(errorValue < bestError)
                {
                    bestError = errorValue;
                    f0Step4[j-1] = f0Map[k][j-1];
                }
            }
            if(min(bestError / (refValue1+0.0001), bestError / (refValue2+0.0001)) > allowedRange)
//			if(bestError / (refValue+0.0001) > allowedRange)
            {
                f0Step4[j-1] = 0.0;
                break;
            }
            if(i != 0 && j == negativeIndex[i-1]+1) break;
        }
    }

    // コピー
    for(i = 0;i < f0Len;i++)
    {
        f0[i] = f0Step4[i];
    }
/* ステップ5は，性能が上がらないので一時的に削除
    // ステップ5（孤立島の切り離し 2回目）
    int voiceRangeMinimum2 = 2+(int)(voiceRangeMinimum/2);
    for(i = 0;i < f0Len;i++) f0[i] = f0Step4[i];
    for(i = voiceRangeMinimum2; i < f0Len-voiceRangeMinimum2;i++)
    {
        for(j = 0;j < voiceRangeMinimum2;j++)
        {
            if(f0Step4[i-j] == 0)
                break;
        }
        for(k = 0;k < voiceRangeMinimum2;k++)
        {
            if(f0Step4[i+k] == 0)
                break;
        }
        f0[i] = j != voiceRangeMinimum2 && k != voiceRangeMinimum2 ?
            0 : f0Step4[i];
    }
*/
    // メモリの開放
    free(f0Base); free(positiveIndex); free(negativeIndex);
    free(f0Step1); free(f0Step2); free(f0Step3); free(f0Step4);
}

// イベントを計算する内部関数 (内部変数なので引数・戻り値に手加減なし)
void rawEventByDio(double boundaryF0, double fs, fftw_complex *xSpec, int xLength, int fftl, double framePeriod, double f0Floor, double f0Ceil, double *timeAxis, int tLen,
                   double *f0Deviations, double *interpolatedF0)
{
    int i;
    int halfAverageLength = (int)(fs / boundaryF0 / 2 + 0.5);
    int indexBias = halfAverageLength*2;

    FFTSet forward(fftl, FFTSet::FFTW_R2C);
    double *equivalentFIR = (double *)forward.in();
    for(i = halfAverageLength*2;i < fftl;i++) equivalentFIR[i] = 0.0;
    nuttallWindow(halfAverageLength*4, equivalentFIR);

    fftw_complex *eSpec = (fftw_complex *)forward.out(); // スペクトル
    forward.execute();
    FFTSet inverse(fftl, FFTSet::FFTW_C2R);
    fftw_complex *eSpec2 = (fftw_complex *)inverse.in();

    // 複素数の掛け算
    for(i = 0;i <= fftl-1;i++)
    {
        eSpec2[i][0] = xSpec[i][0]*eSpec[i][0] - xSpec[i][1]*eSpec[i][1];
        eSpec2[i][1] = xSpec[i][0]*eSpec[i][1] + xSpec[i][1]*eSpec[i][0];
    }

    // 低域通過フィルタリング
    inverse.execute();
    equivalentFIR = (double *)inverse.out();
    // バイアス（低域通過フィルタによる遅延）の除去
    for(i = 0;i < xLength;i++) equivalentFIR[i] = equivalentFIR[i+indexBias];

    // ４つのゼロ交差(構造体のほうがいいね) e:event, i:interval
    double *nELocations, *pELocations, *dnELocations, *dpELocations;
    double *nILocations, *pILocations, *dnILocations, *dpILocations;
    double *nIntervals, *pIntervals, *dnIntervals, *dpIntervals;
    int nLen, pLen, dnLen, dpLen;
    nELocations = (double *)malloc(sizeof(double) * xLength); // xLengthはかなりの保険
    pELocations = (double *)malloc(sizeof(double) * xLength);
    dnELocations = (double *)malloc(sizeof(double) * xLength);
    dpELocations = (double *)malloc(sizeof(double) * xLength);
    nILocations = (double *)malloc(sizeof(double) * xLength);
    pILocations = (double *)malloc(sizeof(double) * xLength);
    dnILocations = (double *)malloc(sizeof(double) * xLength);
    dpILocations = (double *)malloc(sizeof(double) * xLength);
    nIntervals = (double *)malloc(sizeof(double) * xLength);
    pIntervals = (double *)malloc(sizeof(double) * xLength);
    dnIntervals = (double *)malloc(sizeof(double) * xLength);
    dpIntervals = (double *)malloc(sizeof(double) * xLength);

    zeroCrossingEngine(equivalentFIR, xLength, fs,
        nELocations, nILocations, nIntervals, &nLen);

    for(i = 0;i < xLength;i++) equivalentFIR[i] = -equivalentFIR[i];
    zeroCrossingEngine(equivalentFIR, xLength, fs,
        pELocations, pILocations, pIntervals, &pLen);

    for(i = 0;i < xLength-1;i++) equivalentFIR[i] = equivalentFIR[i]-equivalentFIR[i+1];
    zeroCrossingEngine(equivalentFIR, xLength-1, fs,
        dnELocations, dnILocations, dnIntervals, &dnLen);

    for(i = 0;i < xLength-1;i++) equivalentFIR[i] = -equivalentFIR[i];
    zeroCrossingEngine(equivalentFIR, xLength-1, fs,
        dpELocations, dpILocations, dpIntervals, &dpLen);


    int usableChannel;
    usableChannel = checkEvent(nLen-2) * checkEvent(pLen-2) *
        checkEvent(dnLen-2) * checkEvent(dpLen-2);

    double *interpolatedF0Set[4];
    if(usableChannel <= 0)
    { // ノー候補でフィニッシュです
        for(i = 0;i < tLen;i++)
        {
            f0Deviations[i] = 100000.0;
            interpolatedF0[i] = 0.0;
        }
    }
    else
    {
        for(i = 0;i < 4;i++)
            interpolatedF0Set[i] = (double *)malloc(sizeof(double) * tLen);
        // 4つのゼロ交差
        interp1(nILocations , nIntervals , nLen , timeAxis, tLen, interpolatedF0Set[0]);
        interp1(pILocations , pIntervals , pLen , timeAxis, tLen, interpolatedF0Set[1]);
        interp1(dnILocations, dnIntervals, dnLen, timeAxis, tLen, interpolatedF0Set[2]);
        interp1(dpILocations, dpIntervals, dpLen, timeAxis, tLen, interpolatedF0Set[3]);

        for(i = 0;i < tLen;i++)
        {
            interpolatedF0[i] = (interpolatedF0Set[0][i] + interpolatedF0Set[1][i] +
                interpolatedF0Set[2][i] + interpolatedF0Set[3][i]) / 4.0;

            f0Deviations[i]   = sqrt( ((interpolatedF0Set[0][i]-interpolatedF0[i])*(interpolatedF0Set[0][i]-interpolatedF0[i])
                + (interpolatedF0Set[1][i]-interpolatedF0[i])*(interpolatedF0Set[1][i]-interpolatedF0[i])
                + (interpolatedF0Set[2][i]-interpolatedF0[i])*(interpolatedF0Set[2][i]-interpolatedF0[i])
                + (interpolatedF0Set[3][i]-interpolatedF0[i])*(interpolatedF0Set[3][i]-interpolatedF0[i])) / 3.0);

            if(interpolatedF0[i] > boundaryF0 || interpolatedF0[i] < boundaryF0/2.0
                || interpolatedF0[i] > f0Ceil || interpolatedF0[i] < DIO_FLOOR_F0) // 70 Hz以下はF0としない．
            {
                interpolatedF0[i] = 0.0;
                f0Deviations[i]   = 100000.0;
            }
        }

        for(i = 0;i < 4;i++) free(interpolatedF0Set[i]);
    }


    // メモリの開放
    free(nELocations);
    free(pELocations);
    free(dnELocations);
    free(dpELocations);
    free(nILocations);
    free(pILocations);
    free(dnILocations);
    free(dpILocations);
    free(nIntervals);
    free(pIntervals);
    free(dnIntervals);
    free(dpIntervals);
}

// ゼロ交差を計算
void zeroCrossingEngine(double *x, int xLen, double fs,
                        double *eLocations, double *iLocations, double *intervals, int *iLen)
{
    int i;
    int *negativeGoingPoints;
    negativeGoingPoints = (int *)malloc(sizeof(int) * xLen);

    int tmp1, tmp2;
    for(i = 0;i < xLen-1;i++) // 毎回余りを計算するのは無駄
    {
        tmp1 = x[i]*x[i+1] < 0 ? 1 : 0;
        tmp2 = x[i+1] < x[i]   ? 1 : 0;
        negativeGoingPoints[i] = (i+1) * tmp1 * tmp2;
    }
    negativeGoingPoints[xLen-1] = 0;

    // 有効イベントの検出
    int *edges;
    edges = (int *)malloc(sizeof(int) * xLen);
    int count = 0;
    for(i = 0;i < xLen;i++)
    {
        if(negativeGoingPoints[i] > 0) edges[count++] = negativeGoingPoints[i];
    }
    // 最終戻り値の計算準備
    double *fineEdges;
    fineEdges = (double *)malloc(sizeof(double) * count);
    for(i = 0;i < count;i++)
    {
        fineEdges[i] = (double)edges[i] - x[edges[i]-1]/(x[edges[i]]-x[edges[i]-1]);
    }

    *iLen = count-1;
    for(i = 0;i < *iLen;i++)
    {
        intervals[i] = fs / (fineEdges[i+1] - fineEdges[i]);
        iLocations[i] = (fineEdges[i]+fineEdges[i+1])/2.0/fs;
        eLocations[i] = fineEdges[i]/fs;
    }
    /////
    // Memory guard for heap corruption.
    //
    if( count <= 0 ) count=1;
    eLocations[count-1] = fineEdges[count-1]/fs;

    free(fineEdges);
    free(edges);
    free(negativeGoingPoints);
}

// ナットール窓．マジックナンバーのように見えるけどこれが正解．
void nuttallWindow(int yLen, double *y)
{
    int i;
    double tmp;
    for(i = 0;i < yLen;i++)
    {
        tmp  = ((double)(i+1) - (double)(yLen+1)/2.0) / (double)(yLen+1);
        y[i] = 0.355768+0.487396*cos(2*PI*tmp)+0.144232*cos(4*PI*tmp)+0.012604*cos(6*PI*tmp);
    }
}

