/*
 * Task.h
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
#ifndef __Task_h__
#define __Task_h__

#include "RuntimeOption.h"

namespace vconnect
{

    /**
     * vConnect が実行するタスクが実装するべきインターフェース
     */
    class Task
    {
    protected:
        RuntimeOption option;

    public:
        Task( RuntimeOption option )
        {
            this->option = option;
        }

        /**
         * タスクを実行する
         */
        virtual void run() = 0;

        virtual ~Task()
        {
        }

    private:
        Task()
        {
        }
    };
}
#endif
