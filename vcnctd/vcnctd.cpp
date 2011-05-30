/**
 * vcnctd.cpp
 *
 * Copyright © 2011 kbinani.
 */
#include "vcnctd.h"
#include "Config.h"
#include "Server.h"

#include <iostream>

using namespace vcnctd;
using namespace vcnctd::stand;

static Server *server = NULL;

int main( int argc, char *argv[] )
{
    // デーモンのインスタンスを生成（とりあえず
    server = new Server();
    
    // 未解析の音源について解析を行う
    server->analyze();

    // サーバーのメインループを起動
    server->startListening();
    
    return 0;
}
