/*
 *
 *    standSpecgram.h  
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
#ifndef __standSpecgram_h__
#define __standSpecgram_h__

#include "stand.h"
#include "utauVoiceDB/UtauDB.h"
#include "standFrame.h"

// ノーマライズ用平均二乗振幅の基準値
// どの程度の値を用いればいいのかが悩みどころ．
#define VOL_NORMALIZE 0.06

#ifdef STND_MULTI_THREAD
struct computeArg;
#endif

class standSpecgram {
public:
    standSpecgram();
    ~standSpecgram();
    int computeWaveFile( string_t input, bool fast );
    int computeWaveFile( string_t input, utauParameters& parameters, bool fast );    // 周波数表からの補完ありの場合
    void setFrameLength( long length, bool fast );
    void setFrame( long frame, standFrame& src );                        // 速度的には推奨されない
    void getFramePointer( long frame, standFrame& target );
    int getFFTLength( void ){ return fftl; }
    int getAperiodicityLength( void ){ return aperiodicityLength; }
    int getTimeLength( void ){ return tLen; }
    int writeWaveFile( string_t output, long beginFrame, vector<double>* dynamics );
	double* synthesizeWave( long* length );

private:
    void destroy( void );

#ifdef STND_MULTI_THREAD
    void setArguments(computeArg &arg1, computeArg &arg2);
#endif

    double** specgram;
    double** aperiodicity;
    double* f0;
    double* t;
    double  targetF0;        // 0.0.3 用
	double* synthWave;
	long    waveLength;
    int     tLen;
    int     aperiodicityLength;
    int     fftl;
    bool    isFast;
};

#endif
