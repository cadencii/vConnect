/**
 * vcnctd.cpp
 *
 * Copyright © 2011 kbinani.
 */

#include "vcnctd.h"
#include "Config.h"
#include <iostream>

using namespace vcnctd;
using namespace vcnctd::stand;

int main( int argc, char *argv[] )
{
    // 設定をロード
    Config::load();

    // 合成器のインスタンスを生成（とりあえず
    Synth synth;
    Options opts;
    opts.encodingOtoIni = "Shift-JIS";
    
    // 未解析の音源について解析を行う
    int num_row_db = Config::getRawDBCount();
    for( int i = 0; i < num_row_db; i++ )
    {
        // DBを読み込む
        ConfDB *conf = Config::getRawDBConf( i );
        string path = conf->getPath();
        string charset = conf->getCharset();
        UtauDB *db = new UtauDB();
        db->read( path.c_str(), charset.c_str() );

        // 登録するお
        UtauDB::dbRegist( db );

        // 解析をします
        CorpusManager *man = new CorpusManager();
        man->setUtauDB( db, opts );
        man->analyze( opts );
    }
    return 0;
}
