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
private:
    void _create(int tLen, int cLen);
    void _destroy();

    // 32bit 実数型で定義したいが…
    float **MFCC;
    float *f0;
    float *t;
    float framePeriod;


    qint32 tLen; //! @brief 時間長
    qint32 cLen; //! @brief ケプストラム長

    qint32 fftl; //! @brief FFT 長
    qint32 fs;

    // 時間伸縮用写像関数．
    qint32 baseTimeLength;
    float *baseTimeAxis;
};

}
}

#endif // STANDFILE_H
