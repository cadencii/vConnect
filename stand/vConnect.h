/*
 *
 *    vConnect.h
 *                        (c) HAL 2010-
 *
 *  This files is a part of v.Connect.
 * vConnect class is a main class that connects UTAU and WORLD.
 * It is consisted of spectral, pitch and dynamics calculation.
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#ifndef __vConnect_h__
#define __vConnect_h__

#include <limits.h>
#include <string.h>
#include "standSpecgram.h"
#include "corpusManager.h"
#include "vowelTable.h"
#include "vsqMetaText/vsqFileEx.h"
#include "runtimeOptions.h"

#define NOTE_NUM 128
#define VIB_NUM 128

class vConnect {
public:
    vConnect();
    ~vConnect();
    bool synthesize( string_t input, string_t output, runtimeOptions options );
    bool createWspFile( string_t v_path, string_t output, string_t alias, runtimeOptions options );
    static void        calculateAperiodicity(double *dst, const double *src1, const double *src2, int aperiodicityLength,
                                             double morphRate, double noiseRatio, double breRate, bool fast);
private:
    vsqFileEx     vsq;
    corpusManager manager;
    vector<corpusManager*> managers;
    standSpecgram specgram;

    static double noteFrequency[NOTE_NUM];
    static double vibrato[VIB_NUM];
    double        fluctTheta;

    /* 内部処理用関数 */
    void        calculateVsqInfo( void );
    void        calculateF0( standSpecgram& dst, vector<double>& dynamics );
    void        calculateDynamics( vector<double> &dynamics, double *wave, long wave_len, bool volumeNormalization );
    vector<vector<standBP> > controlCurves;

    // vConnect内でしか使わない関数．
    static      double getPitchFluctuation( double second );
    static      void   emptyPath( double secOffset, string_t output );

    long        endFrame;
};

#endif
