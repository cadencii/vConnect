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

        /**
         * STAND で使用するフレーム単位時間が表す時間を取得する
         * @return 1 フレーム時間あたりのミリ秒単位の時間
         * @todo review; この doc コメントであってるか？
         */
        static double getMilliSecondsPerFrame()
        {
            return 2.0;
        }

        /**
         * 音声のデフォルトのサンプリング周波数を取得する
         * @return サンプリング周波数(Hz)
         */
        static int getDefaultSampleRate()
        {
            return 44100;
        }

    private:
        Configuration()
        {
        }
    };
}

#endif
