/*
 * Transcriber.h
 * Copyright © 2011-2012 HAL, 2012 kbinani
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
#ifndef __Transcriber_h__
#define __Transcriber_h__

#include "Task.h"

class vConnectPhoneme;

namespace vconnect
{
    class Transcriber : Task
    {
    public:
        Transcriber( RuntimeOption option )
            : Task( option )
        {
        }

        /**
         * 与えられた二つの UTAU 原音設定から時間伸縮関数を生成します．
         */
        void run();

    private:
        static void _transcribe_compressed( vConnectPhoneme *src, vConnectPhoneme *dst );

        static void _transcribe_raw( vConnectPhoneme *src, vConnectPhoneme *dst );

        static void _calculate_compressed_env( double *dst, vConnectPhoneme *src, int length );
    };
}
#endif

