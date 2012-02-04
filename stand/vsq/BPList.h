/*
 * BPList.h
 * Copyright © 2010-2012 HAL, 2012 kbinani
 *
 * This file is part of vConnect-STAND.
 *
 * vConnect-STAND is free software; you can redistribute it and/or
 * modify it under the terms of the GPL License.
 *
 * vConnect-STAND is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * ---------------------------------------------------------------------
 * vsqBPList class contains and reads vsqBP data.
 */
#ifndef __BPList_h__
#define __BPList_h__

#include <string>
#include <vector>
#include "BP.h"
#include "TempoList.h"
#include "../FrameBP.h"

namespace vconnect
{
    using namespace std;

    class BPList
    {
    public:
        /**
         * このインスタンスが表現するコントロールカーブの時間変化を、FrameBP の配列に変換する
         * @param dst 変換後配列の格納先
         * @param tempo 秒時の算出に必要なテンポ変更リスト
         */
        void getList( vector<FrameBP> &dst, TempoList &tempo );

        void setParameter( long tick, int value );

        void setParameter( string left, string right );

        /**
         * 指定したインデックスにおけるカーブの値を取得する
         * @param index インデックス
         * @return カーブの値
         */
        BP get( int index );

    private:
        vector<BP> data;
    };
}
#endif
