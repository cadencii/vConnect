#ifndef __vConnectPhoneme_h__
#define __vConnectPhoneme_h__

#include <vorbis/vorbisfile.h>
#include <fftw3.h>
#include <string>
#include "utau/UtauParameter.h"

using namespace std;
using namespace vconnect;

// 音量正規化用のマジックナンバー
const double VOL_NORMALIZE = 0.06;

/// <summary>
/// 音素片の形式に対応した列挙子です．
/// </summary>
enum phonemeMode {
    VCNT_UNKNOWN = -1,       // 形式不明．
    VCNT_COMPRESSED = 0,     // Ogg + MelCepstrum に圧縮された形式．
    VCNT_RAW = 1,            // 波形データをそのまま使う形式．
};

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
    int computeWave(
        double *wave,
        int length,
        int fs,
        double framePeriod,
        int cepstrumLength = 32 );

    /// <summary>
    /// 与えられたファイルパスから圧縮形式の音素片を読み込みます．
    /// </summary>
    /// <param name="path">読み込みたいファイルのパス．</param>
    /// <returns>読み込みに成功したときは true, 失敗したときは false を返します．</returns>
    bool readPhoneme( const char *path );

    /// <summary>
    /// 与えられたファイルパスへ圧縮形式の音素片を書き込みます．
    /// </summary>
    /// <param name="path">書き込みたいファイルのパス．</param>
    /// <returns>書き込みに成功したときは true, 失敗したときは false を返します．</returns>
    bool writePhoneme( const char *path );

    /// <summary>
    /// 与えられた UTAU 原音設定から波形を保持する音素片を得ます．
    /// </summary>
    /// <param name="path">読み込むディレクトリパス．</parm>
    /// <param name="params">UTAU 原音設定．</param>
    /// <param name="framePeriod">フレーム単位時間．</param>
    /// <returns>読み込みに成功したときは true，失敗時は false を返します．</returns>
    bool readRawWave( string dir_path, const UtauParameter *params, double framePeriod );

    /// <summary>
    /// 今保持している音素片の該当箇所のメルケプストラムを取得します．
    /// </summary>
    /// <param name="index">読み込むフレーム時刻</param>
    /// <param name="length">ケプストラムの次元．</param>
    /// <returns>成功したときは，該当箇所のケプストラムへのポインタ，失敗したときは NULL を返します．</returns>
    float *getMelCepstrum( int index, int *length );

    /// <summary>
    /// 今保持している音素片の該当箇所の F0 を取得します．
    /// </summary>
    /// <param name="index">読み込むフレーム時刻</param>
    /// <returns>成功したときは，該当箇所の F0，失敗したときは -1 を返します．</returns>
    float getF0( int index );

    /// <summary>
    /// 今保持している音素片の長さを取得します．
    /// </summary>
    /// <returns>音素片フレーム長．</returns>
    int getTimeLength()
    {
        return timeLength;
    }

    /// <summary>
    /// 今保持している音素片の励起信号 Ogg ストリームを開きます．
    /// </summary>
    /// <param name="ovf">開きたい Ogg ストリーム．</param>
    /// <returns>成功したときは true，失敗したときは false を返します．</returns>
    bool vorbisOpen( OggVorbis_File *ovf );

    /// <summary>
    /// 今保持しているデータの形式を取得します．
    /// </summary>
    /// <returns>データ保持形式．</returns>
    phonemeMode getMode()
    {
        return mode;
    }

    /// <summary>
    /// 現在保持しているデータが波形の際，対応する箇所の STAR パラメータを計算します．
    /// </summary>
    /// <param name="starSpec">STAR スペクトルを書き込むバッファ．</param>
    /// <param name="residualSpec">PLATINUM 残差スペクトルを書き込むバッファ．</param>
    /// <param name="t">計算したいフレーム時刻．</param>
    /// <param name="fftLength">FFT 長．</param>
    /// <param name="waveform">FFT 長の作業領域．</param>
    /// <param name="spectrum">FFT 長の作業領域．</param>
    /// <param name="cepstrum">FFT 長の作業領域．</param>
    /// <param name="forward_r2c">waveform -> spectrum の FFTW プラン．</param>
    /// <param name="forward">spectrum -> cepstrum の FFTW プラン．</param>
    /// <param name="inverse">cepstrum -> spectrum の FFTW プラン．</param>
    void getOneFrameWorld(  double *starSpec,
                            fftw_complex *residualSpec,
                            double t, int fftLength,
                            double *waveform,
                            fftw_complex *spectrum,
                            fftw_complex *cepstrum,
                            fftw_plan forward_r2c,
                            fftw_plan forward,
                            fftw_plan inverse);

    /// <summary>
    /// 現在保持しているデータが波形の際，対応する箇所の STAR パラメータを計算します．
    /// </summary>
    /// <param name="frameTime"> フレーム時刻 </param>
    /// <returns> 該当時刻のフレームインデックス． </returns>
    double getFrameTime(int frameTime);
    double getBaseFrameTime(int frameTime);

    void setTimeAxis(double *t, int length);
    void setBaseTimeAxis(double *base, int length);

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

    // 転写用
    float *baseTimeAxis;
    int baseTimeLength;

    double *wave;
    int waveLength;
    int *pulseLocations;
    int waveOffset;

    phonemeMode mode;
};


#endif
