/*
 * UtauDB.h
 *
 * Copyright (C) 2009-2011 HAL,
 * Copyright (C) 2011 kbinani.
 */
#ifndef __UtauDB_h__
#define __UtauDB_h__

#include "utauParameters.h"
#include "utauFreq.h"
#include "../stand.h"

/// <summary>
/// UTAU音源を管理する機能を提供します．
/// </summary>
class UtauDB
{
public: // public method

    ~UtauDB();

    /// <summary>
    /// oto.iniのファイル名とそのテキスト・エンコーディングを指定して，oto.iniを読み込みます．
    /// </summary>
    /// <param name="path_oto_ini">oto.iniファイルのパス．</param>
    /// <param name="codepage">oto.iniファイルのテキスト・エンコーディング．</param>
    /// <returns>
    int read( string_t fileName, const char *codepage );

    /// <summary>
    /// 指定した音素の原音のパラメータを取得します
    /// </summary>
    /// <param name="parameters">パラメータの格納先</param>
    /// <param name="search">パラメータを取得する音素</param>
    /// <returns>取得に失敗した場合に0，成功した場合は0以外の値を返します．</returns>
    int getParams( utauParameters &parameters, string_t search );

    /// <summary>
    /// oto.iniファイルのパスを取得します．
    /// </summary>
    /// <param name="dst">ファイルパスの格納先．</param>
    /// <returns>1を返します．</returns>
    int getDBPath( string_t &dst );

    /// <summary>
    /// 音素の数が0かどうかを取得します．
    /// </summary>
    /// <returns>音素の数が0の場合trueを，そうでない場合はfalseを返します．</returns>
    bool empty();

    /// <summary>
    /// 音素とパラメータの紐付けを格納したマップの反復子の先頭を返します．
    /// </summary>
    /// <returns>反復子の先頭．</returns>
    map_t<string_t, utauParameters *>::iterator begin();

    /// <summary>
    /// 音素とパラメータの紐付けを格納したマップの反復子の末尾を返します．
    /// </summary>
    /// <returns>反復子の末尾．</returns>
    map_t<string_t, utauParameters *>::iterator end();
    
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
    /// </summary>
    /// <param name="db">音源のインスタンス</param>
    static void dbRegist( UtauDB *db );

    /// <summary>
    /// 指定したインデックスのUTAU音源を取得します．
    /// </summary>
    /// <param name="index">音源を指定するインデックス</param>
    /// <returns>UTAU音源．</returns>
    static UtauDB *dbGet( int index );


protected: // protected field

    /// <summary>
    /// oto.iniファイルのパス．
    /// </summary>
    string_t mDBPath;

    /// <summary>
    /// 音素と音素のパラメータの紐付けを格納するマップ．
    /// </summary>
    map_t<string_t, utauParameters *> mSettingMap;

    /// <summary>
    /// 音素のパラメータを格納したインスタンスのリスト．
    /// </summary>
    list<utauParameters *> mSettingList;


private: // private static field

    /// <summary>
    /// 実行時に読み込まれたUTAU音源のリストです．
    /// </summary>
    static vector<UtauDB *> mDBs;
};

typedef UtauDB utauVoiceDataBase;

#endif
