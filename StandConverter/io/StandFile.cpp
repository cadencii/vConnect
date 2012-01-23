#include "StandFile.h"

#include "../math/World.h"
#include "../math/MFCCSet.h"
#include "../math/FFTSet.h"

#include "WaveFile.h"

#include "../utility/Utility.h"

#include <math.h>

using namespace stand::io;
using namespace stand::math;
using namespace stand::math::world;
using namespace stand::utility;

StandFile::StandFile()
{
    baseTimeAxis = f0 = t = NULL;
    MFCC = NULL;
    baseTimeLength = tLen = cLen = 0;
    /* ぐぬぬぬ */
    fftl = 2048;
    fs = 44100;
}

StandFile::~StandFile()
{
    _destroy();
}

void StandFile::_destroy()
{
    delete[] f0;
    delete[] t;
    if(MFCC)
    {
        delete[] MFCC[0];
    }
    delete[] MFCC;
    delete[] baseTimeAxis;

    baseTimeAxis = f0 = t = NULL;
    MFCC = NULL;
    baseTimeLength = tLen = cLen = 0;
    /* ぐぬぬぬ */
    fftl = 2048;
    fs = 44100;

    delete vorbis.data;
    vorbis.data = NULL;
    vorbis.size = 0;
}

void StandFile::_create(int tLen, int cLen)
{
    if(tLen <= 0 || cLen <= 0 || fftl <= 0)
    {
        qDebug("StandFile::_create(%d, %d, %d); // invalid args.", tLen, cLen, fftl);
        return;
    }

    f0 = new float[tLen];
    t = new float[tLen];
    MFCC = new float*[tLen];
    MFCC[0] = new float[tLen * cLen];
    for(int i = 1; i < tLen; i++)
    {
        MFCC[i] = MFCC[0] + i * cLen;
    }
    this->tLen = tLen;
    this->cLen = cLen;
}

bool StandFile::read(const char *path)
{
    FILE *fp = fopen(path, "rb");
    if(!fp)
    {
        qDebug("StandFile::read(\"%s\"); // File could not be opened.", path);
        return false;
    }
    _destroy();

    fread(&tLen, sizeof(qint32), 1, fp);
    fread(&cLen, sizeof(qint32), 1, fp);
    fread(&framePeriod, sizeof(float), 1, fp);

    _create(tLen, cLen);

    for(int i = 0; i < tLen; i++) {
        fread(MFCC[i], sizeof(float), cLen, fp);
    }
    fread(f0, sizeof(float), tLen, fp);
    fread(t , sizeof(float), tLen, fp);

    fread(&vorbis.size, sizeof(qint32), 1, fp);
    vorbis.data = new char[vorbis.size];
    fread(vorbis.data, vorbis.size, 1, fp);

    // 後方互換性と言う名の行き当たりばったり．仕様未だ定まらず．
    if(fread(&baseTimeLength, sizeof(qint32), 1, fp) == 1 && baseTimeLength > 0)
    {
        baseTimeAxis = new float[baseTimeLength];
    }
    else
    {
        baseTimeLength = 0;
    }

    fclose(fp);

    return true;
}

bool StandFile::write(const char *path)
{
    if(!f0 || !t || !MFCC || !vorbis.data)
    {
        qDebug("StandFile::write(\"%s\"); // Empty Data.", path);
        return false;
    }
    FILE *fp = fopen(path, "wb");
    if(!fp)
    {
        qDebug("StandFile::write(\"%s\"); // File could not be opened.", path);
        return false;
    }

    fwrite(&tLen, sizeof(qint32), 1, fp);
    fwrite(&cLen, sizeof(qint32), 1, fp);
    fwrite(&framePeriod, sizeof(float), 1, fp);

    for(int i = 0; i < tLen; i++)
    {
        fwrite(MFCC[i], sizeof(float), cLen, fp);
    }
    fwrite(f0, sizeof(float), tLen, fp);
    fwrite(t , sizeof(float), tLen, fp);

    fwrite(&vorbis.size, sizeof(qint32), 1, fp);
    fwrite(vorbis.data, vorbis.size, 1, fp);

    if(baseTimeAxis)
    {
        fwrite(&baseTimeLength, sizeof(qint32), 1, fp);
        fwrite(baseTimeAxis, sizeof(float), baseTimeLength, fp);
    }

    fclose(fp);

    return true;
}

bool StandFile::compute(double *x, int xLen, int fs, double framePeriod, int cepstrumLength, int minBitrate, int maxBitrate, int averageBitrate)
{
    if(!x || xLen < 0)
    {
        qDebug("StandFile::copmute(x = %h, xLen = %d); // Invalid args.", x, xLen);
        return false;
    }

    _destroy();

    this->framePeriod = (float)framePeriod;
    this->tLen = samplesForDio(fs, xLen, framePeriod);
    this->cLen = cepstrumLength;
    this->fftl = FFTLengthForStar(fs);
    this->fs = fs;

    World::WorldSetting s = {
        fs,
        framePeriod
    };

    World w(tLen , fftl, &s);
    MFCCSet mfcc(fftl);
    _create(tLen, cLen);

    /****************************************************/
    /* まず WORLD による分析を行う．                           */
    /* ただしスペクトルの代わりにメルケプを使用する．                   */
    /****************************************************/
    dio(x, xLen, fs, framePeriod, w.t(), w.f0());
    star(x, xLen, fs, w.t(), w.f0(), w.specgram());

    for(int i = 0; i < tLen; i++)
    {
        this->f0[i] = (float)w.f0At(i);
        this->t[i] = (float)w.tAt(i);
    }

    /* melCepstrum の計算 → specgram へ再度展開 */
    // 展開する場合は melCepstrum の歪み分を残差に持たせる形になる．
    for(int i = 0; i < tLen; i++) {
        // calculate MFCC
        double *wspec = w.spectrumAt(i);
        fftw_complex *mfcc_complex = mfcc.compute(wspec, fs);

        // copy MFCC(double) to float array
        for(int j = 0; j < cLen; j++)
        {
            MFCC[i][j] = mfcc_complex[j][0] / fftl;
        }

        /*double *spectrum = mfcc.extract(MFCC[i], cLen, fs);
        for(int j = 0; j <= fftl / 2; j++)
        {
            wspec[j] = exp(spectrum[j]);
        }*/
    }

    platinum(x, xLen, fs, w.t(), w.f0(), w.specgram(), w.residual());

    /****************************************************/
    /* 次に残差スペクトルを波形の形に持ち直す．                     */
    /* 保存形式は Ogg Vorbis                              */
    /****************************************************/
    double *residualWave = new double[fftl * tLen];

    FFTSet fft(fftl, FFTSet::FFTW_C2R);
    // 残差波形を計算
    for(int i = 0; i < tLen; i++)
    {
        fftw_complex *in = (fftw_complex *)fft.in();
        double *out = (double *)fft.out();

        World::extractResidual(in, w.residualAt(i), fftl);

        fft.execute();

        for(int j = 0, k = i * fftl; j < fftl; j++, k++)
        {
            residualWave[k] = out[j] / fftl;
        }
    }

    encodeToVorbis(residualWave, fftl * tLen, fs, minBitrate, maxBitrate, averageBitrate);

    delete[] residualWave;

    int yLen = tLen * s.framePeriod / 1000.0 * s.fs;
    double *y = new double[yLen];
    for(int i = 0; i < yLen; i++){ y[i] = 0.0; }
    stand::math::world::synthesis(w.f0(), w.specgram(), w.residual(), tLen, fftl, s.framePeriod, s.fs, y, yLen);
    stand::io::WaveFile wa(y, yLen);

    wa.normalize();
    wa.write("d:\\temp.wav");

    return true;
}
