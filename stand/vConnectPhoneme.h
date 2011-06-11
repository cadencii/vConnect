#ifndef __vConnectPhoneme_h__
#define __vConnectPhoneme_h__

#include <vorbis/vorbisfile.h>

/// <summary>
/// vConnect-STANDの１音素片を格納するクラスです．
/// </summary>
class vConnectPhoneme
{
public: // public method
    vConnectPhoneme();
    ~vConnectPhoneme();

    /// <summary>
    /// 与えられた波形を音素片へと変換します．
    /// </summary>
    /// <param name="wave">波形を格納した配列．</param>
    /// <param name="length">波形用配列長．</param>
    /// <param name="fs">標本化周波数．</param>
    /// <param name="framePeriod">分析シフト長．</param>
    /// <param name="cepstrumLength">ケプストラムの次元．</param>
    /// <returns>計算に成功したときは 0, 失敗したときは -1 を返します．</returns>
    int computeWave(double *wave, int length, int fs, double framePeriod, int cepstrumLength = 32);

    /// <summary>
    /// 与えられたファイルパスから音素片を読み込みます．
    /// </summary>
    /// <param name="path">読み込みたいファイルのパス．</param>
    /// <returns>読み込みに成功したときは true, 失敗したときは false を返します．</returns>
    bool readPhoneme(const char* path);

    /// <summary>
    /// 与えられたファイルパスへ音素片を書き込みます．
    /// </summary>
    /// <param name="path">書き込みたいファイルのパス．</param>
    /// <returns>書き込みに成功したときは true, 失敗したときは false を返します．</returns>
    bool writePhoneme(const char* path);

    /// <summary>
    /// 今保持している音素片の該当箇所のメルケプストラムを取得します．
    /// </summary>
    /// <param name="index">読み込むフレーム時刻</param>
    /// <param name="length">ケプストラムの次元．</param>
    /// <returns>成功したときは，該当箇所のケプストラムへのポインタ，失敗したときは NULL を返します．</returns>
    float *getMelCepstrum(int index, int *length);

    /// <summary>
    /// 今保持している音素片の該当箇所の F0 を取得します．
    /// </summary>
    /// <param name="index">読み込むフレーム時刻</param>
    /// <returns>成功したときは，該当箇所の F0，失敗したときは -1 を返します．</returns>
    float getF0(int index);

    /// <summary>
    /// 今保持している音素片の長さを取得します．
    /// </summary>
    /// <returns>音素片フレーム長．</returns>
    int getTimeLength(){ return timeLength; }

    /// <summary>
    /// 今保持している音素片の励起信号 Ogg ストリームを開きます．
    /// </summary>
    /// <param name="ovf">開きたい Ogg ストリーム．</param>
    /// <returns>成功したときは true，失敗したときは false を返します．</returns>
    bool vorbisOpen(OggVorbis_File *ovf);

public: // public static method
    /// <summary>
    /// 開いた Ogg ストリームを閉じます．
    /// </summary>
    /// <param name="ovf">閉じたい Ogg ストリーム．</param>
    static void vorbisClose(OggVorbis_File *ovf){ ov_clear(ovf); }

private: // private static method

    // OggVorbis_File で使用する call back 関数群
    static size_t vorbisRead(void *dst, size_t size, size_t maxCount, void *vp);
    static int vorbisSeek(void *vp, ogg_int64_t offset, int flag);
    static int vorbisClose(void *vp);
    static long vorbisTell(void *vp);

private: // private method
    void destroy();

private: // private field
    struct V_FILE {
        char *p;
        int pos;
        int size;
    };

    int timeLength;
    int cepstrumLength;
    int vorbisSize;
    float framePeriod;

    float **melCepstrum;
    float *f0;
    float *t;
    char  *vorbisData;
};


#endif
