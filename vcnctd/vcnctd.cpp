/**
 * vcnctd.cpp
 *
 * Copyright © 2011 kbinani.
 */

#include "vcnctd.h"
#include "Config.h"
#include <iostream>

using namespace vcnctd;

int main( int argc, char *argv[] )
{
    // 設定をロード
    Config::load();

    // 合成器のインスタンスを生成（とりあえず
    vConnect synth;
    runtimeOptions opts;
    opts.encodingOtoIni = "Shift-JIS";
    
    // 未解析の音源について解析を行う
    int num_row_db = Config::getRawDBCount();
    for( int i = 0; i < num_row_db; i++ )
    {
        // DBを読み込む
        string path = Config::getRawDBPath( i );
        UtauDB *db = new UtauDB();
        db->read( path, "UTF-8" );

        // 登録するお
        UtauDB::dbRegist( db );

        // 合成をします
        corpusManager *man = new corpusManager();
        man->setUtauDB( db, opts );
        man->analyze( opts );
    }
    return 0;
}
