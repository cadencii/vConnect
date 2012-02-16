/*!
 * Stand Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU GPL License
 *
 * Stand Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  @file MFCCSet.h
 *  @brief MFCCSet class supports calculation of MFCC,
 *         and keeps data for MFCC.
 *         This class is thread safe with Qt Library.
 *  @author HAL@shurabaP
 */
#ifndef MFCCSET_H
#define MFCCSET_H

#include "FFTSet.h"

namespace stand
{
namespace math
{

class MFCCSet
{
public:
    explicit MFCCSet(unsigned int l);
    ~MFCCSet();

    fftw_complex *compute(double *spectrum, int fs);
    double *extract(double *melCepstrum, unsigned int l, int fs);

    // つかうかしら．
    double *extract(float *melCepstrum, unsigned int l, int fs);

    unsigned int length()
    {
        return _ffft->length();
    }

    /// <summary>メル尺度から周波数を計算します．</summary>
    /// <param name="melScale">メル尺度</param>
    /// <returns>対応する周波数を返します．</returns>
    static double getFrequency(double melScale);

    /// <summary>周波数からメル尺度を計算します．</summary>
    /// <param name="freq">周波数</param>
    /// <returns>対応するメル尺度を返します．</returns>
    static double getMelScale(double freq);

    /// <summary>スペクトルをメルスケールへ変換します．</summary>
    /// <param name="melSpectrum">結果を書き込むバッファ．</param>
    /// <param name="spectrum">元となるスペクトル．</param>
    /// <param name="spectumLength">配列長．</param>
    /// <param name="maxFrequency">最大周波数．</param>
    static void stretchToMelScale(double *melSpectrum, const double *spectrum, int spectrumLength, int maxFrequency);

    /// <summary>メルスケールのスペクトルを線形スペクトルへ変換します．</summary>
    /// <param name="spectrum">結果を書き込むバッファ．</param>
    /// <param name="melSpectrum">元となるメルスケール上のスペクトル．</param>
    /// <param name="spectumLength">配列長．</param>
    /// <param name="maxFrequency">最大周波数．</param>
    static void stretchFromMelScale(double *spectrum, const double *melSpectrum, int spectrumLength, int maxFrequency);

private:

    FFTSet *_ffft, *_ifft;
};

}
}

#endif // MFCCSET_H
