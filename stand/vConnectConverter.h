#ifndef __vConnectConverter_h__
#define __vConnectConverter_h__

class vConnectConverter {
public:
    vConnectConverter(){ }
    ~vConnectConverter(){ }

    /// <summary>
    /// UTAU 音源形式を STAND 音源形式へ変換します． Unicode に対応していない簡易版です．
    /// </summary>
    /// <param name="otoIni">分析する音源の oto.ini へのパス．</param>
    /// <param name="dstDir">出力先ディレクトリ名．</param>
    /// <returns>分析に成功した場合true，それ以外はfalseを返します．</returns>
    bool convert(const char *otoIni, const char *dstDir);

private:
};

#endif
