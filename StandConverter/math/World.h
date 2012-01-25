/*!
 * @file World.h
 * @author HAL
 */
#ifndef WORLD_H
#define WORLD_H

#include <fftw3.h>

#include "Dio.h"
#include "Star.h"
#include "Platinum.h"
#include "MinimumPhaseSpectrum.h"
#include "WorldSynthesis.h"

namespace stand
{
namespace math
{
namespace world
{
const double PI = 3.1415926535897932384;
const unsigned int MAX_FFT_LENGTH = 2048;
const double FLOOR_F0 = 71.0;
const double DEFAULT_F0 = 150.0;
const double LOW_LIMIT = 65.0;

/// <summay>
/// WORLD による分析に必要なデータを確保します．
/// また， WORLD　の API を使用した分析機能も提供します．
/// </summay>
class World
{
public:
    struct WorldSetting
    {
        int fs;
        double framePeriod;
    };
    const static WorldSetting WORLD_DEFAULT_SETTING;

    explicit World(const WorldSetting *s = & WORLD_DEFAULT_SETTING);
    explicit World(int tLen, int fftl, const WorldSetting *s = & WORLD_DEFAULT_SETTING);
    explicit World(double *x, int xLen, const WorldSetting *s = & WORLD_DEFAULT_SETTING);

    ~World();

    void execute(double *x, int xLen, const WorldSetting *s = &WORLD_DEFAULT_SETTING);

    double *spectrumAt(int i);
    double *spectrumAt(double t);
    double *residualAt(int i);
    double *residualAt(double t);
    double f0At(int i);
    double f0At(double t);
    double tAt(int i);
    double tAt(double t);

    double **specgram()
    {
        return _specgram;
    }

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

    int size()
    {
        return _tLen;
    }

    int fftl()
    {
        return _fftl;
    }

    int samplingFrequency()
    {
        return setting.fs;
    }

    double framePeriod()
    {
        return setting.framePeriod;
    }

    /// <summary>WORLDの非周期性指標をFFTWで使用する形へ展開します．</summary>
    /// <param name="dst">FFTWの複素数配列．</param>
    /// <param name="src">WORLD非周期性指標．</param>
    /// <param name="fftLength">FFT長．</param>
    static void extractResidual(fftw_complex *dst, const double *src, int fftLength);

private:
    void _create(int tLen, int fftl);
    void _destroy();
    int _indexAtSec(double t)
    {
        /* 本来は配列 _t の値から算出するのが正しい． */
        int ret = t * 1000.0 / setting.framePeriod;
        return ret;
    }

    double **_specgram;
    double **_residual;
    double *_t;
    double *_f0;
    WorldSetting setting;
    int _tLen;
    int _fftl;
};

}
}
}

#endif // WORLD_H
