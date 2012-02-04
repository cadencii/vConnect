/*
 * TempoBP.h
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
#ifndef __vconnect_TempoBP_h__
#define __vconnect_TempoBP_h__

namespace vconnect{
    class TempoBP{
    public:
        /**
         * BPM 単位のテンポ値
         */
        double tempo;

        /**
         * 秒単位の時刻
         */
        double time;
    };
}

#endif
