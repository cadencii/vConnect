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

#include <list>
#include "UtauParameter.h"
#include "../Map.h"

namespace vconnect
{
    /// <summary>
    /// UTAU音源を管理する機能を提供します．
    /// </summary>
    class UtauDB
    {
    public:
        virtual ~UtauDB();

        /**
         * oto.iniのファイル名とそのテキスト・エンコーディングを指定して，oto.iniを読み込みます．
         * @param fileName oto.iniファイルのパス．
         * @param codepage oto.iniファイルのテキスト・エンコーディング．
         */
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

        Map<string, UtauParameter *>::iterator begin();

        Map<string, UtauParameter *>::iterator end();

    protected:
        /// <summary>
        /// oto.iniファイルのパス．
        /// </summary>
        string mDBPath;

        Map<string, UtauParameter *> mSettingMap;

        list<UtauParameter *> mSettingList;
    };
}
#endif
