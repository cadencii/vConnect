#include "StandFile.h"

#include "../math/MathSet.h"
#include "WaveFile.h"

#include "../utility/Utility.h"

#include <math.h>

using namespace stand::io;
using namespace stand::math;
using namespace stand::math::world;
using namespace stand::utility;

StandFile::StandFile()
{
    _baseTimeAxis = _f0 = _t = NULL;
    _MFCC = NULL;
    _baseTimeLength = _tLen = _cLen = 0;
    /* ぐぬぬぬ */
    _fftl = 2048;
    _fs = 44100;
}

StandFile::~StandFile()
{
    _destroy();
}

void StandFile::_destroy()
{
    delete[] _f0;
    delete[] _t;
    if(_MFCC)
    {
        delete[] _MFCC[0];
    }
    delete[] _MFCC;
    delete[] _baseTimeAxis;

    _baseTimeAxis = _f0 = _t = NULL;
    _MFCC = NULL;
    _baseTimeLength = _tLen = _cLen = 0;
    /* ぐぬぬぬ */
    _fftl = 2048;
    _fs = 44100;

    delete[] vorbis.data;
    vorbis.data = NULL;
    vorbis.size = 0;
}

void StandFile::_create(int tLen, int cLen)
{
    if(tLen <= 0 || cLen <= 0 || _fftl <= 0)
    {
        qDebug("StandFile::_create(%d, %d, %d); // invalid args.", tLen, cLen, _fftl);
        return;
    }

    _f0 = new float[tLen];
    _t = new float[tLen];
    _MFCC = new float*[tLen];
    _MFCC[0] = new float[tLen * cLen];
    for(int i = 1; i < tLen; i++)
    {
        _MFCC[i] = _MFCC[0] + i * cLen;
    }
    this->_tLen = tLen;
    this->_cLen = cLen;
}

bool StandFile::read(const char *path)
{
    qDebug("StandFile::read(\"%s\");", path);
    FILE *fp = fopen(path, "rb");
    if(!fp)
    {
        qDebug("StandFile::read(\"%s\"); // File could not be opened.", path);
        return false;
    }
    _destroy();

    fread(&_tLen, sizeof(qint32), 1, fp);
    fread(&_cLen, sizeof(qint32), 1, fp);
    fread(&_framePeriod, sizeof(float), 1, fp);

    _create(_tLen, _cLen);

    for(int i = 0; i < _tLen; i++) {
        fread(_MFCC[i], sizeof(float), _cLen, fp);
    }
    fread(_f0, sizeof(float), _tLen, fp);
    fread(_t , sizeof(float), _tLen, fp);

    fread(&vorbis.size, sizeof(qint32), 1, fp);
    vorbis.data = new char[vorbis.size];
    fread(vorbis.data, vorbis.size, 1, fp);

    // 後方互換性と言う名の行き当たりばったり．仕様未だ定まらず．
    if(fread(&_baseTimeLength, sizeof(qint32), 1, fp) == 1 && _baseTimeLength > 0)
    {
        _baseTimeAxis = new float[_baseTimeLength];
        fread(_baseTimeAxis, sizeof(float), _baseTimeLength, fp);
    }
    else
    {
        _baseTimeLength = 0;
    }

    fclose(fp);

    return true;
}

bool StandFile::write(const char *path)
{
    qDebug("StandFile::write(\"%s\");", path);
    if(!_f0 || !_t || !_MFCC || !vorbis.data)
    {
        qDebug("StandFile::write(\"%s\"); // Empty Data.", path);
        return false;
    }
    FILE *fp = fopen(path, "wb");
    if(!fp)
    {
        qDebug("StandFile::write(\"%s\"); // File open error.", path);
        return false;
    }

    fwrite(&_tLen, sizeof(qint32), 1, fp);
    fwrite(&_cLen, sizeof(qint32), 1, fp);
    fwrite(&_framePeriod, sizeof(float), 1, fp);

    for(int i = 0; i < _tLen; i++)
    {
        fwrite(_MFCC[i], sizeof(float), _cLen, fp);
    }
    fwrite(_f0, sizeof(float), _tLen, fp);
    fwrite(_t , sizeof(float), _tLen, fp);

    fwrite(&vorbis.size, sizeof(qint32), 1, fp);
    fwrite(vorbis.data, vorbis.size, 1, fp);

    if(_baseTimeAxis)
    {
        fwrite(&_baseTimeLength, sizeof(qint32), 1, fp);
        fwrite(_baseTimeAxis, sizeof(float), _baseTimeLength, fp);
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

    this->_framePeriod = (float)framePeriod;
    this->_tLen = samplesForDio(fs, xLen, framePeriod);
    this->_cLen = cepstrumLength;
    this->_fftl = FFTLengthForStar(fs);
    this->_fs = fs;

    World::WorldSetting s = {
        fs,
        framePeriod
    };

    World w(_tLen , _fftl, &s);
    MFCCSet mfcc(_fftl);
    _create(_tLen, _cLen);

    /****************************************************/
    /* まず WORLD による分析を行う．                           */
    /* ただしスペクトルの代わりにメルケプを使用する．                   */
    /****************************************************/
    dio(x, xLen, fs, framePeriod, w.t(), w.f0());
    star(x, xLen, fs, w.t(), w.f0(), w.specgram());

    for(int i = 0; i < _tLen; i++)
    {
        this->_f0[i] = (float)w.f0At(i);
        this->_t[i] = (float)w.tAt(i);
    }

    /* melCepstrum の計算 → specgram へ再度展開 */
    // 展開する場合は melCepstrum の歪み分を残差に持たせる形になる．
    for(int i = 0; i < _tLen; i++) {
        // calculate MFCC
        double *wspec = w.spectrumAt(i);
        fftw_complex *mfcc_complex = mfcc.compute(wspec, fs);

        // copy MFCC(double) to float array
        for(int j = 0; j < _cLen; j++)
        {
            _MFCC[i][j] = mfcc_complex[j][0] / _fftl;
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
    double *residualWave = new double[_fftl * _tLen];

    FFTSet fft(_fftl, FFTSet::FFTW_C2R);
    // 残差波形を計算
    for(int i = 0; i < _tLen; i++)
    {
        fftw_complex *in = (fftw_complex *)fft.in();
        double *out = (double *)fft.out();

        World::extractResidual(in, w.residualAt(i), _fftl);

        fft.execute();

        for(int j = 0, k = i * _fftl; j < _fftl; j++, k++)
        {
            residualWave[k] = out[j] / _fftl;
        }
    }

    encodeToVorbis(residualWave, _fftl * _tLen, fs, minBitrate, maxBitrate, averageBitrate);

    delete[] residualWave;

    int yLen = _tLen * s.framePeriod / 1000.0 * s.fs;
    double *y = new double[yLen];
    for(int i = 0; i < yLen; i++)
    {
        y[i] = 0.0;
    }
/*    stand::math::world::synthesis(w.f0(), w.specgram(), w.residual(), tLen, fftl, s.framePeriod, s.fs, y, yLen);
    stand::io::WaveFile wa(y, yLen);

    wa.normalize();
    wa.write("d:\\temp.wav");*/

    return true;
}

void StandFile::matching(StandFile *src, StandFile *dst)
{
    if(!src || !dst)
    {
        qDebug("StandFile::matching(%h, %h);", src, dst);
        return;
    }
    int src_len, dst_len;
    double *src_env, *dst_env;
    double *src_to_dst, *dst_to_src, *dst_to_src_stretched;

    src_len = src->tLen();
    dst_len = dst->tLen();

    src_env = new double[src_len];
    dst_env = new double[dst_len];
    src_to_dst = new double[src_len];
    dst_to_src = new double[dst_len];
    dst_to_src_stretched = new double[src_len];

    // 振幅エンベロープを求めます．
    _calculateVolumeEnvelope(src_env, src, src_len);
    _calculateVolumeEnvelope(dst_env, dst, dst_len);

    for( int i = 0; i < src_len - 1; i++ )
    {
        double tmp = (double)i / (double)src_len * (double)dst_len;
        if( tmp >= dst_len - 1 )
        {
            dst_to_src_stretched[i] = dst_env[dst_len-1];
        }
        else
        {
            dst_to_src_stretched[i] = stand::math::interpolateArray(tmp, dst_env);
        }
    }
    dst_to_src_stretched[src_len-1] = dst_env[dst_len-1];

    stand::math::smoothMatching(dst_to_src_stretched, src_to_dst, src_env, dst_to_src_stretched, src_len);

    for( int i = 0; i < dst_len - 1; i++ )
    {
        double tmp = (double)i / (double)dst_len * (double)src_len;
        if( tmp >= src_len - 1 )
        {
            dst_to_src[i] = (dst_len - 1) * dst->framePeriod() / 1000.0;
        }
        else
        {
            dst_to_src[i] = stand::math::interpolateArray(tmp, dst_to_src_stretched) * dst->framePeriod() / 1000.0 / (double)src_len * (double)dst_len;
        }
    }
    dst_to_src[dst_len-1] = (dst_len - 1) * dst->framePeriod() / 1000.0;

    for( int i = 0; i < src_len; i++ )
    {
        src_to_dst[i] *= src->framePeriod() / 1000.0;
    }

    dst->setTimeAxis(dst_to_src, dst_len);
    dst->setBaseTimeAxis(src_to_dst, src_len);

    delete[] dst_to_src_stretched;
    delete[] src_to_dst;
    delete[] dst_to_src;
    delete[] src_env;
    delete[] dst_env;
}

void StandFile::_calculateVolumeEnvelope(double *dst, StandFile *src, int length)
{
    int fftl = src->fftl();

    MFCCSet mfcc(fftl);
    FFTSet forward(fftl, FFTSet::FFTW_C2C_FORWARD);
    FFTSet inverse(fftl, FFTSet::FFTW_C2C_BACKWARD);
    FFTSet fromWave(fftl, FFTSet::FFTW_R2C);
    FFTSet toWave(fftl, FFTSet::FFTW_C2R);

    OggVorbis_File ovf;
    src->open( &ovf );
    float **pcm_channels;

    for( int i = 0; i < length; i++ ){
        int c;
        float *mel_cep = src->MFCC()[i];
        int mel_len = src->cLen();
        double sum = 0.0;

        // ケプストラムからパワースペクトルを計算．
        double *pow_spec = mfcc.extract(mel_cep, mel_len, src->fs());
        fftw_complex *spectrum = stand::math::world::minimumPhaseSpectrum(pow_spec, fftl, &forward, &inverse);

        // Ogg ストリームから残差波形をデコード．
        double *res_wave = (double *)fromWave.in();
        for(c = 0; c < fftl;)
        {
            int bitStream;
            long samples = ov_read_float( &ovf, &pcm_channels, fftl - c, &bitStream );
            if(samples <= 0)
            {
                break;
            }
            for(int j = 0, k = c; j < samples && k < fftl; j++, k++)
            {
                res_wave[k] = pcm_channels[0][j];
            }
            c += samples;
        }

        // 残差スペクトルの計算．
        fromWave.execute();
        fftw_complex *res_spec = (fftw_complex *)fromWave.out();
        fftw_complex *wav_spec = (fftw_complex *)toWave.in();

        for(int k = 0; k <= fftl / 2; k++)
        {
            wav_spec[k][0] = spectrum[k][0] * res_spec[k][0] - spectrum[k][1] * res_spec[k][1];
            wav_spec[k][1] = spectrum[k][1] * res_spec[k][0] + spectrum[k][0] * res_spec[k][1];
        }

        toWave.execute();
        double *out = (double *)toWave.out();

        for( int j = 0; j < fftl; j++ )
        {
            sum += out[j] * out[j];
        }
        dst[i] = sum;
    }

    src->close(&ovf);
}


void StandFile::setTimeAxis(double *t, int length)
{
    if(length != _tLen)
    {
        qDebug("StandFile::setTimeAxis(%d); // length is not equalt to innner data(%d)", length, _tLen);
        return;
    }
    for(int i = 0; i < length; i++)
    {
        this->_t[i] = t[i];
    }
}

void StandFile::setBaseTimeAxis(double *base, int length)
{
    if(length <= 0)
    {
        return;
    }
    delete[] _baseTimeAxis;
    _baseTimeAxis = new float[length];
    _baseTimeLength = length;
    for(int i = 0; i < length; i++)
    {
        this->_baseTimeAxis[i] = base[i];
    }
}
