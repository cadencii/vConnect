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
#include <memory>
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
        /**
         * oto.iniのファイル名とそのテキスト・エンコーディングを指定して，oto.iniを読み込みます．
         * @param fileName oto.iniファイルのパス．
         * @param codepage oto.iniファイルのテキスト・エンコーディング．
         */
        UtauDB( string fileName, string codepage );

        virtual ~UtauDB();

        /// <summary>
        /// 指定した音素の原音のパラメータを取得します
        /// </summary>
        /// <param name="parameters">パラメータの格納先</param>
        /// <param name="search">パラメータを取得する音素</param>
        /// <returns>取得に失敗した場合に0，成功した場合は0以外の値を返します．</returns>
        int getParams( UtauParameter &parameters, string search );

        int getParams( UtauParameter &parameters, int index );

        /**
         * oto.iniファイルのパスを取得します．
         * @return oto.ini のパス
         */
        string getOtoIniPath();

        int size();

    private:
        UtauDB()
        {}

        struct Impl;
        std::shared_ptr<Impl> impl_;
    };
}
#endif
