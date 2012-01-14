/**
 * Sequence.h
 * Copyright (C) 2009-2011 HAL,
 * Copyright (C) 2011-2012 kbinani.
 */
#ifndef __vsqFileEx_h__
#define __vsqFileEx_h__

#include "vsqBase.h"
#include "Event.h"
#include "EventList.h"
#include "BPList.h"
#include "vsqTempo.h"
#include "CurveTypeEnum.h"
#include "../RuntimeOption.h"
#include "../Socket.h"
#include "../Path.h"
#include "../InputStream.h"
#include "../TextInputStream.h"

namespace vconnect
{
    /// <summary>
    /// VSQメタテキストが表現するシーケンスを取り扱うクラスです．
    /// </summary>
    class Sequence
    {
    public:
        /// <summary>
        /// コンストラクタ．コントロールカーブのvectorの初期化等を行います．
        /// </summary>
        Sequence();

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
        vector<BPList> controlCurves;

        Map<string, BPList *> mMapCurves;

        /// <summary>
        /// シーケンス内の音符と歌手変更イベントを格納したリスト．
        /// </summary>
        EventList events;

        /// <summary>
        /// テンポの変更情報を格納したリスト．
        /// </summary>
        vsqTempo vsqTempoBp;

        /// <summary>
        /// [ID#]の文字列と，その中身との紐付けを保持するマップ．
        /// </summary>
        Map<string, vconnect::Event *> mMapIDs;

        /// <summary>
        /// [h#]の文字列と，その中身との紐付けを保持するマップ．
        /// </summary>
        Map<string, Handle *> mMapHandles;

        /**
         * 歌手の名称（だったけ？IDSのぶぶんだったかIconIDの部分だったか忘れた）と，
         * 歌手のインデックスとの紐付けを保持する．
         */
        Map<string, int> singerMap;

    private:
        /**
         * VSQのメタテキストを読み込みます．
         * @param stream 読み込むストリーム．
         * @param vsqFilePath 読み込むファイルのパス
         * @param encodingOtoIni oto.ini ファイルのテキストエンコーディング
         * @return 読み込みに成功した場合true，それ以外はfalseを返します．
         */
        bool readCore( InputStream *stream, string vsqFilePath, string encodingOtoIni );

        /// <summary>
        /// 指定したイベントの内容を，メタテキストの行データを元に設定します．
        /// </summary>
        /// <param name="target>設定対象のイベント</param>
        /// <param name="left">メタテキストの"="の左側部分</param>
        /// <param name="right">メタテキストの"="の右側部分</param>
        void setParamEvent( vconnect::Event *target, string left, string right );

        /**
         * 歌手名と oto.ini のパスの紐付けを登録する
         * @param singerName 歌手名
         * @param otoIniPath oto.ini のパス
         * @param encoding oto.ini のテキストエンコーディング
         */
        void setParamOtoIni( string singerName, string otoIniPath, string encoding );

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
                Event *item = events.eventList[i];
                cout << "[" << i << "]" << " tick=" << item->tick << endl;
            }
        }

        /// <summary>
        /// mMapIDsの中身をダンプします．
        /// </summary>
        void dumpMapIDs()
        {
            cout << "vsqFileEx::dumpMapIDs" << endl;
            Map<string, Event *>::iterator i;
            for( i = mMapIDs.begin(); i != mMapIDs.end(); i++ ){
                Event *item = i->second;
                cout << i->first << ":" << item->toString() << endl;
            }
        }

        /// <summary>
        /// mMapHandlesの中身をダンプします．
        /// </summary>
        void dumpMapHandles()
        {
            cout << "vsqFileEx::dumpMapHandles" << endl;
            Map<string, Handle *>::iterator i;
            for( i = mMapHandles.begin(); i != mMapHandles.end(); i++ ){
                cout << i->first << ":" << i->second->toString() << endl;
            }
        }
    };
}
#endif
