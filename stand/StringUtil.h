/*
 * StringUtil.h
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
#ifndef __StringUtil_h__
#define __StringUtil_h__

#include <list>
#include <string>

using namespace std;

namespace vconnect
{

    /**
     * 文字列関連のユーティリティ
     */
    class StringUtil
    {
    public:
        /**
         * 文字列を区切り文字で分割する
         * @param delimiter 区切り文字
         * @param text 文字列
         * @param limit 区切る回数の最大値
         * @return 区切られた文字列のリスト
         */
        static list<string> explode( string delimiter, string text, int limit = string::npos )
        {
            list<string> result;
            string::size_type searchFrom = 0;
            string::size_type delimiterIndex = text.find( delimiter, searchFrom );
            while( delimiterIndex != string::npos ){
                string token = text.substr( searchFrom, delimiterIndex - searchFrom );
                result.push_back( token );
                searchFrom = delimiterIndex + delimiter.length();
                if( result.size() + 1 == limit ){
                    break;
                }
                delimiterIndex = text.find( delimiter, searchFrom );
            }
            result.push_back( text.substr( searchFrom ) );
            return result;
        }
    };

}

#endif
