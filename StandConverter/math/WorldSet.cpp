/*!
 * Stand Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU GPL License
 *
 * Stand Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  @file WorldSet.cpp
 *  @brief WorldSet class supports calculation of WORLD,
 *         and keep data for WORLD; DIO, STAR and PLATINUM.
 *         This code is thread safe with Qt Library.
 *  @author HAL@shurabaP
 */
#include "WorldSet.h"

#include "World.h"

using namespace stand::math::world;

WorldSet::WorldSet(const SpecgramSet::SpecgramSetting *s) :
    SpecgramSet(s)
{
    _residual = NULL;
    _f0 = _t = NULL;
}

WorldSet::WorldSet(const double *x, int xLen, const SpecgramSet::SpecgramSetting *s) :
    SpecgramSet(s)
{
    _residual = NULL;
    _f0 = _t = NULL;
    compute(x, xLen, s);
}

WorldSet::WorldSet(int tLen, int fftl, const SpecgramSet::SpecgramSetting *s) :
    SpecgramSet(s)
{
    _specgram = _residual = NULL;
    _f0 = _t = NULL;
    _create(tLen, fftl);
}

WorldSet::~WorldSet()
{
    _destroy();
}

void WorldSet::compute(const double *x, int xLen, const SpecgramSet::SpecgramSetting *s)
{
    if(!x)
    {
        return;
    }

    if(s)
    {
        _setting = *s;
    }
    int tLen = samplesForDio(_setting.fs, xLen, _setting.framePeriod);
    int fftl = FFTLengthForStar(_setting.fs);
    _create(tLen, fftl);

    dio(x, xLen, _setting.fs, _setting.framePeriod, _t, _f0);
    star(x, xLen, _setting.fs, _t, _f0, _specgram);
    platinum(x, xLen, _setting.fs, _t, _f0, _specgram, _residual);
}

double *WorldSet::spectrumAt(double t)
{
    return spectrumAt(_indexAtSec(t));
}

double *WorldSet::spectrumAt(int i)
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

double *WorldSet::residualAt(double t)
{
    return residualAt(_indexAtSec(t));
}

double *WorldSet::residualAt(int i)
{
    if(i < 0)
    {
        i = 0;
    }
    else if(i >= _tLen)
    {
        i = _tLen - 1;
    }
    return _residual[i];
}

double WorldSet::f0At(double t)
{
    return f0At(_indexAtSec(t));
}

double WorldSet::f0At(int i)
{
    if(i < 0)
    {
        i = 0;
    }
    else if(i >= _tLen)
    {
        i = _tLen - 1;
    }
    return _f0[i];
}

double WorldSet::tAt(double t)
{
    return tAt(_indexAtSec(t));
}

double WorldSet::tAt(int i)
{
    if(i < 0)
    {
        i = 0;
    }
    else if(i >= _tLen)
    {
        i = _tLen - 1;
    }
    return _t[i];
}

void WorldSet::_destroy()
{
    _destroySpecgram();
    if(_residual)
    {
        delete[] _residual[0];
    }
    delete[] _residual;

    delete[] _f0;
    delete[] _t;
    _residual = NULL;
    _f0 = _t = NULL;
}

void WorldSet::_create(int tLen, int fftl)
{
    _destroy();
    _createSpecgram(tLen, fftl);
    _f0 = new double[tLen];
    _t = new double[tLen];
    _residual = new double*[tLen];
    _residual[0] = new double[tLen * fftl];
    for(int i = 1; i < tLen; i++)
    {
        _residual[i] = _residual[0] + i * fftl;
    }
}
