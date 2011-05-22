/*
 *
 *    utauVoiceDataBase.h
 *                        (c) HAL 2009-
 *
 * This files is a part of v.Connect.
 * utauVoiceDataBase is a container class of contains UTAU oto.ini
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#ifndef __utauVoiceDataBase_h__
#define __utauVoiceDataBase_h__

#include "utauParameters.h"
#include "utauFreq.h"
#include "../stand.h"

class utauVoiceDataBase
{
public: // public method

    ~utauVoiceDataBase();

    /// <summary>
    /// oto.iniのファイル名とそのテキスト・エンコーディングを指定して，oto.iniを読み込みます．
    /// </summary>
    /// <param name="path_oto_ini">oto.iniファイルのパス．</param>
    /// <param name="codepage">oto.iniファイルのテキスト・エンコーディング．</param>
    int read( string_t fileName, const char *codepage );

    int getUtauParameters( utauParameters& parameters, string_t search );

    int getVoicePath( string_t& dst ){
        dst = voicePath; 
        return 1;
    }

    bool empty( void ){
        return settingMap.empty();
    }


public: // public static method
    
    /// <summary>
    /// 読込済みのUTAU音源の個数を取得します．
    /// </summary>
    /// <returns>読込済みのUTAU音源の個数．</returns>
    static int dbSize();

    /// <summary>
    /// 読み込んだUTAU音源のリストを破棄します．
    /// </summary>
    static void dbClear();

    /// <summary>
    /// UTAU音源をリストに追加します．
    /// </sumamry>
    /// <param name="db">音源のインスタンス</param>
    static void dbRegist( utauVoiceDataBase *db );

    /// <summary>
    /// 指定したインデックスのUTAU音源を取得します．
    /// </summary>
    /// <param name="index">音源を指定するインデックス</param>
    /// <returns>UTAU音源．</returns>
    static utauVoiceDataBase *dbGet( int index );


protected: // protected field

    string_t voicePath;
    map_t<string_t, utauParameters *> settingMap;
    list<utauParameters *> settingList;


private: // private static field

    /// <summary>
    /// 実行時に読み込まれたUTAU音源のリストです．
    /// </summary>
    static vector<utauVoiceDataBase *> mDBs;
};

#endif
