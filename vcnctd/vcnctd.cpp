/**
 * vcnctd.cpp
 *
 * Copyright © 2011 kbinani.
 */
#include "vcnctd.h"
#include "Config.h"
#include "Server.h"

#include <stdlib.h>

#include <iostream>

using namespace vcnctd;
using namespace vcnctd::stand;

static Server *server = NULL;

int main( int argc, char *argv[] )
{
    mtrace();
    
    // デーモンのインスタンスを生成（とりあえず
    server = new Server();
    
    // 未解析の音源について解析を行う
    server->analyze();

    return 0;
}
