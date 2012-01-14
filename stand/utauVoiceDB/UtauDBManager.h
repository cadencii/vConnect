/*
 * UtauDBManager.h
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
#ifndef __UtauDBManager_h__
#define __UtauDBManager_h__

#include <vector>
#include "UtauDB.h"

namespace vconnect
{
    /**
     * UTAU 音源のリストを管理するクラス
     */
    class UtauDBManager
    {
    private:
        /**
         * 実行時に読み込まれたUTAU音源のリストです．
         */
        static vector<UtauDB *> mDBs;

    public:
        /**
         * 読込済みのUTAU音源の個数を取得します．
         * @returns 読込済みのUTAU音源の個数．
         */
        static int size();

        /**
         * 読み込んだUTAU音源のリストを破棄します．
         */
        static void clear();

        /**
         * UTAU音源をリストに追加します．
         * @param db 音源のインスタンス
         */
        static void regist( UtauDB *db );

        /**
         * 指定したインデックスのUTAU音源を取得します．
         * @param index 音源を指定するインデックス
         * @returns UTAU音源．
         */
        static UtauDB *get( int index );

        /**
         * 指定した oto.ini の音源のインデックスを取得する
         * @param otoIniPath oto.ini のパス
         * @return 音源のインデックス
         */
        static int find( string otoIniPath );

    private:
        UtauDBManager()
        {
        }
    };
}
#endif
