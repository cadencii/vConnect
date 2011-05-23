/**
 * Config.cpp
 *
 * Copyright (C) 2011 kbinani.
 */
#include "Config.h"

namespace vcnctd
{

    string Config::mConfPath = "/etc/vcnctd.conf";

    int Config::load()
    {
    }

    string Config::getConfPath()
    {
        return Config::mConfPath;
    }

}
