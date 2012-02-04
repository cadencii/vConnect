/*
 * TempoList.h
 * Copyright © 2010-2012 HAL
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
#ifndef __TempoList_h__
#define __TempoList_h__

#include <string>
#include <map>
#include "TempoBP.h"

using namespace std;

namespace vconnect
{
    class TempoList
    {
    public:
        /**
         * デフォルトのテンポ値
         */
        static const double DEFAULT_TEMPO;

    private:
        /**
         * tick 単位の時刻をキーとした、テンポ変更点の連想配列
         */
        std::map<long, TempoBP> points;

        /**
         * points フィールドの中身のテンポ変更点の時刻が更新されたかどうか
         */
        bool isUpdated;

    public:
        TempoList(){
            this->isUpdated = false;
        }

        /**
         * @brief テンポ値と時刻のセットを、テンポリストに追加する
         * @param tick tick 単位の時刻
         * @param tempo テンポ値
         */
        void push( long tick, double tempo );

        double tickToSecond( long tick );

    private:
        /**
         * リスト内のテンポ変更情報の秒単位の時刻部分を更新する
         */
        void updateTempoInfo();
    };
}
#endif
