/*
 *
 *    standTexture.h
 *                              (c) HAL 2010-           
 *
 *  This file is a part of STAND Library.
 * standTexture is a structure that contains
 * frequency transform functions and spectral filter.
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#ifndef __standTexture_h__
#define __standTexture_h__

#include "stand.h"
#include "runtimeOptions.h"

class standTexture{
public:
    standTexture();
    ~standTexture();

    bool readTexture(string_t input);
    bool writeTexture(string_t output);

    //! @brief 与えられたスペクトルをメルケプストラムへ変換し自身のバッファへ格納する．
    void calculateMelCepstrum(int cepstrumLength, const double *f0, const double **sourceSpecgram, int spectrumNumber, int spectrumLength, int maxFrequency);

    void destroy(void);
    standComplex *getMelCepstrum(double msTime, int *length);

private:
    standComplex    **melCepstrum; //! @brief 内部的には Mel Cepstrum をテクスチャとして使用する．
    int             cepstrumLength;
    int             cepstrumNumber;
    float           *f0;
    float           framePeriod;
};

extern double interpolateArray( double x, const double *p );

extern void applyStretching( double *T, double *target, int length );

//! @brief スペクトルの横軸を周波数からメル尺度へ変形する．
void stretchToMelScale(double *melSpectrum, const double *spectrum, int spectrumLength, int maxFrequency);
//! @brief スペクトルの横軸をメル尺度から周波数へ変形する．
void stretchFromMelScale(double *spectrum, const double *melSpectrum, int spectrumLength, int maxFrequency);

//! @brief 周波数をメルスケールへ変換する．
double  getMelScale(double freq);

//! @brief メルスケールを周波数へ変換する．
double  getFrequency(double mel);

#endif
