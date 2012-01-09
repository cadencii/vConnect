/**
 * vsqFileEx.h
 * Copyright (C) 2009-2011 HAL,
 * Copyright (C) 2011-2012 kbinani.
 */
#ifndef __vsqFileEx_h__
#define __vsqFileEx_h__

#include "vsqBase.h"
#include "vsqEventList.h"
#include "vsqBPList.h"
#include "vsqTempo.h"
#include "vsqPhonemeDB.h"
#include "../RuntimeOption.h"
#include "../Socket.h"
#include "../Path.h"
#include "../InputStream.h"
#include "../TextInputStream.h"

using namespace vconnect;

/// <summary>
/// VSQメタテキストが表現するシーケンスを取り扱うクラスです．
/// </summary>
class vsqFileEx
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
    bool read( string file_name, RuntimeOption option );

    /// <summary>
    /// ソケットからVSQのメタテキストを読み込みます．
    /// </summary>
    /// <param name="socket">読み込むソケット．</param>
    /// <param name="options">読み込み時の設定値．</param>
    /// <returns>読み込みに成功した場合true，それ以外はfalseを返します．</returns>
    bool read( Socket socket, RuntimeOption option );

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
    int getSingerIndex( string singer_name );

    /// <summary>
    /// テンポ値を取得します．
    /// </summary>
    double getTempo();

    /// <summary>
    /// シーケンスの長さ（tick単位）を取得します．
    /// </summary>
    /// <returns>シーケンスの長さ（tick単位）</returns>
    long getEndTick();

public:

    /// <summary>
    /// ダイナミクスなどのコントロールカーブを格納したvector．
    /// </summary>
    vector<vsqBPList> controlCurves;

    map_t<string, vsqBPList *> mMapCurves;

    /// <summary>
    /// シーケンス内の音符と歌手変更イベントを格納したリスト．
    /// </summary>
    vsqEventList events;

    /// <summary>
    /// テンポの変更情報を格納したリスト．
    /// </summary>
    vsqTempo vsqTempoBp;

    /// <summary>
    /// [ID#]の文字列と，その中身との紐付けを保持するマップ．
    /// </summary>
    map_t<string, vsqEventEx *> mMapIDs;

    /// <summary>
    /// [h#]の文字列と，その中身との紐付けを保持するマップ．
    /// </summary>
    map_t<string, vsqHandle *> mMapHandles;

    /// <summary>
    /// メタテキストのセクション名（[]で囲われた部分）と，
    /// その内部の値を保持したオブジェクトとの紐付けを保持する．
    /// </summary>
    //map_t<string, vsqBase *> objectMap;

    /// <summary>
    /// 歌手の名称（だったけ？IDSのぶぶんだったかIconIDの部分だったか忘れた）と，
    /// 歌手のインデックスとの紐付けを保持する．
    /// </summary>
    map_t<string, int> singerMap;


protected:

private:

    /// <summary>
    /// VSQのメタテキストを読み込みます．
    /// </summary>
    /// <param name="stream">読み込むストリーム．</param>
    /// <param name="vsqFilePath">読み込むファイルのパス</param>
    /// <returns>読み込みに成功した場合true，それ以外はfalseを返します．</returns>
    bool readCore( InputStream *stream, string vsqFilePath );

    /// <summary>
    /// 指定したイベントの内容を，メタテキストの行データを元に設定します．
    /// </summary>
    /// <param name="target>設定対象のイベント</param>
    /// <param name="left">メタテキストの"="の左側部分</param>
    /// <param name="right">メタテキストの"="の右側部分</param>
    void setParamEvent( vsqEventEx *target, string left, string right );

    void setParamOtoIni( vsqPhonemeDB *target, string singerName, string otoIniPath );

private:

#if defined( _DEBUG )
public:
#endif
    /// <summary>
    /// eventsの中身をダンプします
    /// </summary>
    void dumpEvents()
    {
        cout << "vsqFileEx::dumpEvents" << endl;
        int size = events.eventList.size();
        for( int i = 0; i < size; i++ )
        {
            vsqEventEx *item = events.eventList[i];
            cout << "[" << i << "]" << " tick=" << item->tick << endl;
        }
    }

    /// <summary>
    /// mMapIDsの中身をダンプします．
    /// </summary>
    void dumpMapIDs()
    {
        cout << "vsqFileEx::dumpMapIDs" << endl;
        map_t<string, vsqEventEx *>::iterator i;
        for( i = mMapIDs.begin(); i != mMapIDs.end(); i++ ){
            vsqEventEx *item = i->second;
            cout << i->first << ":" << item->toString() << endl;
        }
    }

    /// <summary>
    /// mMapHandlesの中身をダンプします．
    /// </summary>
    void dumpMapHandles()
    {
        cout << "vsqFileEx::dumpMapHandles" << endl;
        map_t<string, vsqHandle *>::iterator i;
        for( i = mMapHandles.begin(); i != mMapHandles.end(); i++ ){
            cout << i->first << ":" << i->second->toString() << endl;
        }
    }

private:
    vsqPhonemeDB voiceDataBase;

};

#endif
