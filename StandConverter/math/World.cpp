#include "World.h"

#include <stdio.h>

using namespace stand::math::world;

const World::WorldSetting World::WORLD_DEFAULT_SETTING =
{
    44100,
    2.0
};

World::World(const WorldSetting *s)
{
    _specgram = _residual = NULL;
    _f0 = _t = NULL;
    if(!s)
    {
        s = &WORLD_DEFAULT_SETTING;
    }
    setting = *s;
}

World::World(double *x, int xLen, const WorldSetting *s)
{
    _specgram = _residual = NULL;
    _f0 = _t = NULL;
    if(!s)
    {
        s = &WORLD_DEFAULT_SETTING;
    }
    execute(x, xLen, s);
}

World::World(int tLen, int fftl, const WorldSetting *s)
{
    _specgram = _residual = NULL;
    _f0 = _t = NULL;
    if(!s)
    {
        s = &WORLD_DEFAULT_SETTING;
    }
    setting = *s;
    _create(tLen, fftl);
}

World::~World()
{
    _destroy();
}

void World::execute(double *x, int xLen, const WorldSetting *s)
{
    if(!x)
    {
        return;
    }
    int tLen = samplesForDio(s->fs, xLen, s->framePeriod);
    int fftl = FFTLengthForStar(s->fs);
    _create(tLen, fftl);

    if(!s)
    {
        s = &WORLD_DEFAULT_SETTING;
    }
    setting = *s;

    dio(x, xLen, s->fs, s->framePeriod, _t, _f0);
    star(x, xLen, s->fs, _t, _f0, _specgram);
    platinum(x, xLen, s->fs, _t, _f0, _specgram, _residual);
}

double *World::spectrumAt(double t)
{
    return spectrumAt(_indexAtSec(t));
}

double *World::spectrumAt(int i)
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

double *World::residualAt(double t)
{
    return residualAt(_indexAtSec(t));
}

double *World::residualAt(int i)
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

double World::f0At(double t)
{
    return f0At(_indexAtSec(t));
}

double World::f0At(int i)
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

double World::tAt(double t)
{
    return tAt(_indexAtSec(t));
}

double World::tAt(int i)
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

void World::_destroy()
{
    if(_specgram)
    {
        delete[] _specgram[0];
    }
    delete[] _specgram;

    if(_residual)
    {
        delete[] _residual[0];
    }
    delete[] _residual;

    delete[] _f0;
    delete[] _t;
    _specgram = _residual = NULL;
    _f0 = _t = NULL;
}

void World::_create(int tLen, int fftl)
{
    _destroy();
    _f0 = new double[tLen];
    _t = new double[tLen];
    _specgram = new double*[tLen];
    _specgram[0] = new double[tLen * fftl];
    _residual = new double*[tLen];
    _residual[0] = new double[tLen * fftl];
    for(int i = 1; i < tLen; i++)
    {
        _specgram[i] = _specgram[0] + i * fftl;
        _residual[i] = _residual[0] + i * fftl;
    }
    _tLen = tLen;
    _fftl = fftl;
}

void World::extractResidual(fftw_complex *dst, const double *src, int fftLength)
{
    dst[0][0] = src[0];
    dst[0][1] = 0.0;
    for(int i = 1; i < fftLength / 2; i++) {
        dst[i][0] = src[2*i-1];
        dst[i][1] = src[2*i];
    }
    dst[fftLength/2][0] = src[fftLength-1];
    dst[fftLength/2][1] = 0.0;
}
