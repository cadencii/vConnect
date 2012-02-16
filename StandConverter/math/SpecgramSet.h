/*!
 * Stand Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU GPL License
 *
 * Stand Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  @file SpecgramSet.h
 *  @brief SpecgramSet class supports specgram calculation,
 *         and keep data for specgram.
 *         This code is thread safe with Qt Library.
 *  @author HAL@shurabaP
 */
#ifndef SPECGRAMSET_H
#define SPECGRAMSET_H

namespace stand
{
namespace math
{

class SpecgramSet
{
public:
    struct SpecgramSetting
    {
        int fs;
        double framePeriod;
    };

    const static SpecgramSetting SPECGRAM_DEFAULT_SETTING;

    explicit SpecgramSet(const SpecgramSetting *s = & SPECGRAM_DEFAULT_SETTING);
    explicit SpecgramSet(int tLen, int fftl, const SpecgramSetting *s = & SPECGRAM_DEFAULT_SETTING);
    virtual ~SpecgramSet();

    virtual void compute(const double *x, int xLen, const SpecgramSetting *s = &SPECGRAM_DEFAULT_SETTING);
    double *spectrumAt(int i);

    double **specgram()
    {
        return _specgram;
    }
    int tLen()
    {
        return _tLen;
    }

    int fftl()
    {
        return _fftl;
    }

    int samplingFrequency()
    {
        return _setting.fs;
    }

    double framePeriod()
    {
        return _setting.framePeriod;
    }

protected:
    void _createSpecgram(int tLen, int fftl);
    void _destroySpecgram();

    SpecgramSet::SpecgramSetting _setting;
    double **_specgram;
    int _tLen;
    int _fftl;

private:
};

}
}

#endif // SPECGRAMSET_H
