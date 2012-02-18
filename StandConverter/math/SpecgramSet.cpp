/*!
 * Stand Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU GPL License
 *
 * Stand Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  @file SpecgramSet.cpp
 *  @brief SpecgramSet class supports specgram calculation,
 *         and keep data for specgram.
 *         This code is thread safe with Qt Library.
 *  @author HAL@shurabaP
 */
#include "SpecgramSet.h"

#include "World.h"
#include "FFTSet.h"

#include <math.h>

using namespace stand::math;

const SpecgramSet::SpecgramSetting SpecgramSet::SPECGRAM_DEFAULT_SETTING =
{
    44100,
    2.0
};

SpecgramSet::SpecgramSet(const SpecgramSetting *s)
{
    if(!s)
    {
        s = &SPECGRAM_DEFAULT_SETTING;
    }
    _setting = *s;
    _specgram = NULL;
    _fftl = _tLen = 0;
}

SpecgramSet::SpecgramSet(int tLen, int fftl, const SpecgramSetting *s)
{
    if(!s)
    {
        s = &SPECGRAM_DEFAULT_SETTING;
    }
    _setting = *s;
    _specgram = NULL;
    _createSpecgram(tLen , fftl);
}

SpecgramSet::~SpecgramSet()
{
    _destroySpecgram();
}

void SpecgramSet::_destroySpecgram()
{
    if(_specgram)
    {
        delete[] _specgram[0];
    }
    delete[] _specgram;
    _specgram = NULL;
    _tLen = _fftl = 0;
}

void SpecgramSet::_createSpecgram(int tLen, int fftl)
{
    if(tLen <= 0 || fftl <= 0)
    {
        return;
    }
    _destroySpecgram();
    _specgram = new double*[tLen];
    _specgram[0] = new double[tLen * fftl];
    for(int i = 1; i < tLen; i++)
    {
        _specgram[i] = _specgram[0] + i * fftl;
    }
    _tLen = tLen;
    _fftl = fftl;
}

void SpecgramSet::compute(const double *x, int xLen, const SpecgramSetting *s)
{
    if(!x || xLen <= 0)
    {
        return;
    }
    if(s)
    {
        _setting = *s;
    }
    int tLen = (double)xLen / (double)_setting.fs * 1000.0 / _setting.framePeriod + 1;
    _destroySpecgram();
    _createSpecgram(tLen, 2048);

    FFTSet forward(2048, FFTSet::FFTW_R2C);
    double *in = (double *)forward.in();
    fftw_complex *out = (fftw_complex *)forward.out();

    for(int i = 0; i < tLen; i++)
    {
        int begin = i * _setting.framePeriod / 1000.0 * (double)_setting.fs - _fftl / 2;
        // 時刻 t の前後 1024 点ずつを取り出す
        for(int j = begin, k = 0; k < _fftl; j++, k++)
        {
            if(0 <= j && j < xLen)
            {
                in[k] = x[j];
            }
            else
            {
                in[k] = 0;
            }
        }
        // 窓掛ける
        for(int j = 0; j < _fftl; j++)
        {
            double theta = stand::math::PI * (double)j / (double)_fftl;
            in[j] *= sin(theta);
        }
        // FFT する
        forward.execute();
        // パワー計算する
        for(int j = 0; j <= _fftl / 2; j++)
        {
            _specgram[i][j] = out[j][0] * out[j][0] + out[j][1] * out[j][1];
        }
    }
}

double *SpecgramSet::spectrumAt(int i)
{
    if(i < 0)
    {
        i = 0;
    }
    else if(i >= _tLen)
    {
        i = _tLen - 1;
    }
    return _specgram[i];
}

