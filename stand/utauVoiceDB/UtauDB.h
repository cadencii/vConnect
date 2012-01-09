/*
 * UtauDB.h
 * Copyright © 2009-2012 HAL, 2012 kbinani
 *
 * This file is part of vConnect-STAND.
 *
 * vConnect-STAND is free software; you can redistribute it and/or
 * modify it under the terms of the GPL License.
 *
 * vConnect-STAND is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifndef __UtauDB_h__
#define __UtauDB_h__

#include "UtauParameter.h"
#include "utauFreq.h"
#include "../stand.h"
#include "../TextInputStream.h"
#include "../Path.h"

using namespace vconnect;

/// <summary>
/// UTAU音源を管理する機能を提供します．
/// </summary>
class UtauDB
{
public: // public method

    virtual ~UtauDB();

    /// <summary>
    /// oto.iniのファイル名とそのテキスト・エンコーディングを指定して，oto.iniを読み込みます．
    /// </summary>
    /// <param name="path_oto_ini">oto.iniファイルのパス．</param>
    /// <param name="codepage">oto.iniファイルのテキスト・エンコーディング．</param>
    int read( string fileName, const char *codepage );

    /// <summary>
    /// 指定した音素の原音のパラメータを取得します
    /// </summary>
    /// <param name="parameters">パラメータの格納先</param>
    /// <param name="search">パラメータを取得する音素</param>
    /// <returns>取得に失敗した場合に0，成功した場合は0以外の値を返します．</returns>
    int getParams( UtauParameter &parameters, string search );

    int getParams( UtauParameter &parameters, int index );

    /// <summary>
    /// oto.iniファイルのパスを取得します．
    /// </summary>
    /// <param name="dst">ファイルパスの格納先．</param>
    /// <returns>1を返します．</returns>
    int getDBPath( string &dst );

    /// <summary>
    /// 音素の数が0かどうかを取得します．
    /// </summary>
    /// <returns>音素の数が0の場合trueを，そうでない場合はfalseを返します．</returns>
    bool empty();


    int size();

    map_t<string, UtauParameter *>::iterator begin();

    map_t<string, UtauParameter *>::iterator end();

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
    string mDBPath;

    map_t<string, UtauParameter *> mSettingMap;

    list<UtauParameter *> mSettingList;


private: // private static field

    /// <summary>
    /// 実行時に読み込まれたUTAU音源のリストです．
    /// </summary>
    static vector<UtauDB *> mDBs;
};

typedef UtauDB utauVoiceDataBase;

#endif
