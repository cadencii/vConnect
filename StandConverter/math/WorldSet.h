/*!
 * Stand Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU GPL License
 *
 * Stand Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  @file WorldSet.h
 *  @brief WorldSet class supports calculation of WORLD,
 *         and keep data for WORLD; DIO, STAR and PLATINUM.
 *         This code is thread safe with Qt Library.
 *  @author HAL@shurabaP
 */
#ifndef WORLDSET_H
#define WORLDSET_H

#include "SpecgramSet.h"

namespace stand
{
namespace math
{
namespace world
{

/// <summay>
/// WORLD による分析に必要なデータを確保します．
/// また， WORLD　の API を使用した分析機能も提供します．
/// </summay>
class WorldSet : public SpecgramSet
{
public:

    explicit WorldSet(const SpecgramSet::SpecgramSetting *s = & SPECGRAM_DEFAULT_SETTING);
    explicit WorldSet(int tLen, int fftl, const SpecgramSet::SpecgramSetting *s = & SPECGRAM_DEFAULT_SETTING);
    explicit WorldSet(const double *x, int xLen, const SpecgramSet::SpecgramSetting *s = & SPECGRAM_DEFAULT_SETTING);

    ~WorldSet();

    void compute(const double *x, int xLen, const SpecgramSet::SpecgramSetting *s = NULL);


    double *spectrumAt(int i);
    double *spectrumAt(double t);
    double *residualAt(int i);
    double *residualAt(double t);
    double f0At(int i);
    double f0At(double t);
    double tAt(int i);
    double tAt(double t);

    double **residual()
    {
        return _residual;
    }

    double *f0()
    {
        return _f0;
    }

    double *t()
    {
        return _t;
    }

private:
    void _create(int tLen, int fftl);
    void _destroy();
    int _indexAtSec(double t)
    {
        /* 本来は配列 _t の値から算出するのが正しい． */
        int ret = t * 1000.0 / _setting.framePeriod;
        return ret;
    }

    double **_residual;
    double *_t;
    double *_f0;
};

}
}
}

#endif // WORLDSET_H
