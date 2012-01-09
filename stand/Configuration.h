/*
 * Configuration.h
 * Copyright © 2012 kbinani
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
#ifndef __Configuration_h__
#define __Configuration_h__

#include <list>
#include <string>

namespace vconnect
{
    using namespace std;

    /**
     * vConnect の動作に必要な設定情報を保持するクラス
     */
    class Configuration
    {
    public:
        /**
         * サポートするコードセット名のリストを取得する
         * @return コードセット名のリスト
         */
        static list<string> supportedCodesets()
        {
            list<string> result;
            result.push_back( "Shift_JIS" );
            result.push_back( "euc-jp" );
            result.push_back( "iso-2022-jp"  );
            result.push_back( "UTF-8" );
            result.push_back( "UTF-16LE" );
            result.push_back( "UTF-16BE" );
            result.push_back( "UTF-32BE" );
            result.push_back( "UTF-32LE" );
            return result;
        }

    private:
        Configuration()
        {
        }
    };
}

#endif
