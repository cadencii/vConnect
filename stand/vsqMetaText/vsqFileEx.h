/**
 * vsqFileEx.h
 * Copyright (C) 2009-2011 HAL,
 * Copyright (C) 2011 kbinani
 */
#ifndef __vsqFileEx_h__
#define __vsqFileEx_h__

#include "vsqEventList.h"
#include "vsqBPList.h"
#include "vsqTempo.h"
#include "vsqPhonemeDB.h"
#include "../runtimeOptions.h"
#include "../Socket.h"

/// <summary>
/// VSQメタテキストが表現するシーケンスを取り扱うクラスです．
/// </summary>
class vsqFileEx : public vsqBase
{
public:
    /// <summary>
    /// コンストラクタ．コントロールカーブのvectorの初期化等を行います．
    /// </summary>
    vsqFileEx();

    /// <summary>
    /// ファイルからVSQのメタテキストを読み込みます．
    /// </summary>
    /// <param name="file_name">読み込むメタテキストファイルのパス．</param>
    /// <param name="options">読み込み時の設定値．</param>
    /// <returns>読み込みに成功した場合true，それ以外はfalseを返します．</returns>
    bool read( string_t file_name, runtimeOptions options );

    /// <summary>
    /// ソケットからVSQのメタテキストを読み込みます．
    /// </summary>
    /// <param name="socket">読み込むソケット．</param>
    /// <param name="options">読み込み時の設定値．</param>
    /// <returns>読み込みに成功した場合true，それ以外はfalseを返します．</returns>
    bool read( Socket socket, runtimeOptions options );
    
    /// <summary>
    /// シーケンスの演奏長さを取得します．単位は秒です．
    /// </summary>
    /// <returns>シーケンスの演奏長さ．</returns>
    double getEndSec();

    /// <summary>
    /// 歌手の名前から，歌手を特定するためのインデックスを取得します．
    /// </summary>
    /// <param name="singer_name">歌手の名前</param>
    /// <returns>歌手を特定するインデックス．歌手が見つけられなかった場合は規定値の0を返します．</returns>
    int getSingerIndex( string_t singer_name );

    /// <summary>
    /// ダイナミクスなどのコントロールカーブを格納したvector．
    /// </summary>
    vector<vsqBPList> controlCurves;

    /// <summary>
    /// シーケンス内の音符と歌手変更イベントを格納したリスト．
    /// </summary>
    vsqEventList events;

    /// <summary>
    /// テンポの変更情報を格納したリスト．
    /// </summary>
    vsqTempo vsqTempoBp;

protected:

private:

    /// <summary>
    /// VSQのメタテキストを読み込みます．
    /// </summary>
    /// <param name="fp">読み込むストリーム．</param>
    /// <returns>読み込みに成功した場合true，それ以外はfalseを返します．</returns>
    bool readCore( MB_FILE *fp );
    
    vsqPhonemeDB voiceDataBase;
};

#endif
