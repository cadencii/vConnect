#ifndef __worldParameters_h__
#define __worldParameters_h__

/// <summary>
/// WORLD の分析パラメータの一部を保持します．
/// </summary>
class worldParameters
{
public:

    worldParameters();
    ~worldParameters();

    /// <summary>
    /// 与えられた波形から WORLD の分析パラメータを抽出します．
    /// </summary>
    /// <param name="wave">波形を格納した配列．</param>
    /// <param name="waveLength">配列長．</param>
    /// <param name="fs">標本化周波数．</param>
    /// <param name="framePeriod">フレーム単位時間．</param>
    /// <returns>成功したときは true，失敗したときは false を返します．</returns>
    bool computeWave(double *wave, int waveLength, int fs, double framePeriod);

    /// <summary>
    /// 保持している分析済みパラメータをファイルに書き出します．
    /// </summary>
    /// <param name="path">書き込むファイルパス．</param>
    /// <returns>成功したときは true，失敗したときは false を返します．</returns>
    bool writeParameters(const char *path);

    /// <summary>
    /// 分析済みパラメータを読み込みます．
    /// </summary>
    /// <param name="path">読み込むファイルパス．</param>
    /// <returns>成功したときは true，失敗したときは false を返します．</returns>
    bool readParameters(const char *path);

    /// <summary>
    /// 分析済みパラメータの該当箇所を読み出します．
    /// </summary>
    /// <param name="f0">F0 列を書き込む配列．</param>
    /// <param name="t">時間軸を書き込む配列．</param>
    /// <param name="pulseLocation">フレーム時刻毎のパルス位置を書き込む配列．</param>
    /// <param name="fs">標本化周波数．</param>
    /// <param name="beginFrame">開始フレーム時刻．</param>
    /// <param name="endFrame">終了フレーム時刻．</param>
    /// <param name="framePeriod">フレーム単位時間．</param>
    /// <returns>成功したときは true，失敗したときは false を返します．</returns>
    bool getParameters(float *f0, float *t, int *pulseLocations, int fs, double beginTime, double endTime, double framePeriod);

private:

    void destroy();

    float *f0;
    float *t;
    int *pulseLocations;
    int tLen;
    float framePeriod;
};

#endif
