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
         * テンポ値
         */
        double tempo;

    public:
        TempoList()
        {
            this->tempo = DEFAULT_TEMPO;
        }

        void setParameter( string left, string right );

        long secondToTick( double second );

        double tickToSecond( long tick );

        /**
         * テンポ値を取得する
         * @return テンポ値
         */
        double getTempo()
        {
            return this->tempo;
        }
    };
}
#endif
