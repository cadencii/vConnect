/**
 * Server.cpp
 *
 * Copyright © 2011 kbinani.
 */
#include "Server.h"

namespace vcnctd
{

    Server::Server()
    {
        this->synth = new Synth();
        this->config = new Config();
    }

    Server::~Server()
    {
        int num = mCorpusManagers.size();
        for( int i = 0; i < num; i++ )
        {
            CorpusManager *man = mCorpusManagers[i];
            if( man ) delete man;
            mCorpusManagers[i] = NULL;
        }
        mCorpusManagers.clear();
        UtauDB::dbClear();
    }
    
    void Server::analyze()
    {
        runtimeOptions opts;
        
        int num_row_db = this->config->getRawDBCount();
        for( int i = 0; i < num_row_db; i++ )
        {
            // DBを読み込む
            ConfDB *conf = this->config->getRawDBConf( i );
            string path = conf->getPath();
            string charset = conf->getCharset();
            UtauDB *db = new UtauDB();
            db->read( path.c_str(), charset.c_str() );
            
            // 登録するお
            UtauDB::dbRegist( db );
            
            // 解析をします
            CorpusManager *man = new CorpusManager();
            mCorpusManagers.push_back( man );
            man->setUtauDB( db, opts );
            man->analyze( opts );
        }
    }
    
}
