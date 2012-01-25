#ifndef STANDFILE_H
#define STANDFILE_H

#include "VorbisFile.h"

namespace stand
{
namespace io
{

class StandFile : public VorbisFile
{
public:
    StandFile();
    virtual ~StandFile();

    /// <summary>
    /// 与えられたファイルパスから stand 形式のファイルを読み込みます．
    /// </summary>
    /// <param name="path">読み込みたいファイルのパス．</param>
    /// <returns>読み込みに成功したときは true, 失敗したときは false を返します．</returns>
    bool read(const char *path);

    /// <summary>
    /// 与えられたファイルパスへ stand 形式のファイルを書き込みます．
    /// </summary>
    /// <param name="path">書き込みたいファイルのパス．</param>
    /// <returns>書き込みに成功したときは true, 失敗したときは false を返します．</returns>
    bool write(const char *path);

    /// </summary>
    /// <param name="x">波形を格納した配列．</param>
    /// <param name="xLen">波形用配列長．</param>
    /// <param name="fs">標本化周波数．</param>
    /// <param name="framePeriod">分析シフト長．</param>
    /// <param name="cepstrumLength">ケプストラムの次元．</param>
    /// <param name="minBitrate">Vorbis 圧縮の最小ビットレートの設定．</param>
    /// <param name="maxBitrate">Vorbis 圧縮の最大ビットレートの設定．</param>
    /// <param name="averageBitrate">Vorbis 圧縮の平均ビットレートの設定．</param>
    /// <returns>計算に成功したときは true, 失敗したときは false を返します．</returns>
    bool compute(double *x,
                 int xLen,
                 int fs,
                 double framePeriod,
                 int cepstrumLength,
                 int minBitrate,
                 int maxBitrate,
                 int averageBitrate);

    static void matching(StandFile *src, StandFile *dst);

    void setTimeAxis(double *t, int length);

    void setBaseTimeAxis(double *base, int length);

    qint32 tLen() const
    {
        return _tLen;
    }

    qint32 cLen() const
    {
        return _cLen;
    }

    qint32 fftl() const
    {
        return _fftl;
    }

    qint32 fs() const
    {
        return _fs;
    }

    float **MFCC()
    {
        return _MFCC;
    }
    float *f0()
    {
        return _f0;
    }
    qint32 baseTimeLength() const
    {
        return _baseTimeLength;
    }
    float *baseTimeAxis()
    {
        return _baseTimeAxis;
    }

    float framePeriod() const
    {
        return _framePeriod;
    }


private:
    void _create(int tLen, int cLen);
    void _destroy();
    static void _calculateVolumeEnvelope(double *dst, StandFile *src, int length);

    // 32bit 実数型で定義したいが…
    float **_MFCC;
    float *_f0;
    float *_t;
    float _framePeriod;


    qint32 _tLen; //! @brief 時間長
    qint32 _cLen; //! @brief ケプストラム長

    qint32 _fftl; //! @brief FFT 長
    qint32 _fs;

    // 時間伸縮用写像関数．
    qint32 _baseTimeLength;
    float *_baseTimeAxis;
};

}
}

#endif // STANDFILE_H
