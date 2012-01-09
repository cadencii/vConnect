/*
 * InputStream.h
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
#ifndef __InputStream_h__
#define __InputStream_h__

#include <string>

namespace vconnect
{
    using namespace std;

    /**
     * テキストを読み込むストリーム
     */
    class InputStream
    {
    public:
        /**
         * ストリームを閉じる
         */
        virtual void close() = 0;

        /**
         * ストリームから1行分のデータを読み込む
         * @return 1行分のデータ
         */
        virtual string readLine() = 0;

        /**
         * ストリームに対してさらに読み込めるかどうか
         * @return 読み込める状態であれば true を返す
         */
        virtual bool ready() = 0;

        /**
         * デストラクタ
         */
        virtual ~InputStream()
        {
        }
    };

}

#endif
