#ifndef __vConnectUtility_h__
#define __vConnectUtility_h__

#include <fftw3.h>

/// <summary>
/// vConnect-STAND で使われる数学関数群です．
/// </summary>
class vConnectUtility
{
public: // public static method

    /// <summary>メル尺度から周波数を計算します．</summary>
    /// <param name="melScale">メル尺度</param>
    /// <returns>対応する周波数を返します．</returns>
    static double getFrequency(double melScale);

    /// <summary>周波数からメル尺度を計算します．</summary>
    /// <param name="freq">周波数</param>
    /// <returns>対応するメル尺度を返します．</returns>
    static double getMelScale(double freq);

    /// <summary>メルケプストラムをスペクトルへ展開します．</summary>
    /// <param name="dst">展開先のスペクトル格納配列です．</param>
    static void extractMelCepstrum( double *dst,
                                    int fftl,
                                    float *src,
                                    int cepl,
                                    fftw_complex *in,
                                    double *out,
                                    fftw_plan plan,
                                    int fs );

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

    /// <summary> スペクトルを線形伸縮します．</summary>
    /// <param name="dst">結果を書き込むバッファ．</param>
    /// <param name="src">元となるスペクトル．</param>
    /// <param name="ratio">伸縮率</param>
    /// <param name="length">配列長．</param>
    static void linearStretch(double *dst, const double *src, double ratio, int length);

    /// <summary>配列の中間値を線形補間します．</summary>
    /// <param name="x">配列のインデックス．</param>
    /// <param name="p">配列</param>
    /// <returns>配列の中間値を返します．</returns>
    static double interpolateArray( double x, const double *p );

    /// <summary>WORLDの非周期性指標をFFTWで使用する形へ展開します．</summary>
    /// <param name="dst">FFTWの複素数配列．</param>
    /// <param name="src">WORLD非周期性指標．</param>
    /// <param name="fftLength">FFT長．</param>
    static void extractResidual(fftw_complex *dst, const double *src, int fftLength);

    /// <summary>波形を OggVorbis 形式へエンコードします．バッファはnewで確保されます．</summary>
    /// <param name="dst">OggVorbisバッファが返ります．</param>
    /// <param name="size">バッファサイズが返ります．</param>
    /// <param name="wave">エンコードする波形．</param>
    /// <param name="length">波形長．</param>
    /// <param name="fs">標本化周波数．</param>
    static void newOggVorbis(char **dst, int *size, const double *wave, int length, int fs);

    /// <summary>二つのデータを伸縮マッチングします．</summary>
    /// <param name = "T">ストレッチング関数．</param>
    /// <param name = "H">ストレッチング関数．</param>
    /// <param name = "src_s">元となるデータ列．</param>
    /// <param name = "dst_s">対象となるデータ列．</param>
    /// <param name = "length">データ長．</param>
    static void calculateMatching( double* dst_to_src, double* src_to_dst, double* src_s, double* dst_s, int length );

    static void applyStretching( double *T, double* target, int length );

private:
    static void _get_graduation( double* src, double* dst, int length,
                                 double *d1s, double *d2s, double *d1d, double *d2d);

    static void _log_normalize_infinite( double* dst, int length );

    static double _get_cost( double* src, double* dst, int i, int j, int n, int m,
                             double *d1d, double *d2d, double *d1s, double *d2s);
};

#endif
