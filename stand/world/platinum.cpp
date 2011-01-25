/*
 *
 * 非周期性成分推定法 PLATINUM 0.0.1 by M. Morise
 *
 *    platinum.cpp
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
 *  platinum.cpp includes a set of functions
 * that supports PLATINUM 0.0.1.
 *  Notice that aperiodicity of 0.0.1 is much different
 * from that of 0.0.4. Please use platinum function
 * for synthesis function.
 *
 */
#include "world.h"

#include <stdio.h> // for debug
#include <stdlib.h>
#include <math.h>

// PLATINUMの実装は限定的．
// 正規リリースにはもう少しマシなアルゴリズムを実装します．
// Aperiodicity estimation based on PLATINUM
// [0]から順番にu, b, fc, w0

void platinum(int fs, double *f0, int tLen,
         double **aperiodicity)
{
    int i, j;

    double u, b, fc, w0; // これらが保存されるパラメタ
//    double fl, fh, alpha;

    // デバッグ用につき固定値を設定する
    u = 0.8; 
    b = 0.2;


    for(i = 0;i < tLen;i++)
    {
        if(f0[i] == 0)
        {
            for(j = 0;j < 4;j++)
                aperiodicity[i][j] = 0.0;
        }
        else
        {
            w0 = 0.01 / (f0[i]/20.0);
            fc = f0[i]*1273.0/32.0 + 87.5;
            aperiodicity[i][0] = u;
            aperiodicity[i][1] = b;
            aperiodicity[i][2] = fc;
            aperiodicity[i][3] = w0;
        }
    }
    return;
}

// periodicSpecはfftl/2+1の長さが必要
void calculateAperiodicity(double *aperiodicity, int fftl, int fs, double *periodicSpec)
{
    int i;
    double w;
    for(i = 0;i <= fftl/2; i++)
    {
        w = (double)(i * fs)/ (double)fftl;
        periodicSpec[i] = 1.0 / (1.0 + exp(-aperiodicity[3] * (w-aperiodicity[2])));
        periodicSpec[i] = aperiodicity[1]+aperiodicity[0]*(1.0 - periodicSpec[i]);
//        periodicSpec[i] = aperiodicity[1]+(aperiodicity[0]-aperiodicity[1]) / 
//            (1.0 + exp(-aperiodicity[3]*(w-aperiodicity[2])));
    }
}
