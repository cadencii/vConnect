/**
 * Config.cpp
 *
 * Copyright (C) 2011 kbinani.
 */
#include <string>
#include <iostream>

#include "Config.h"

using namespace std;

namespace vcnctd
{
    int Config::getPort()
    {
        return mPort;
    }
    
    Config::~Config()
    {
        for( int i = 0; i < mRawDBConf.size(); i++ )
        {
            ConfDB *conf = mRawDBConf[i];
            if( conf )
            {
                delete conf;
            }
        }
        mRawDBConf.clear();
    }
    
    int Config::getRawDBCount()
    {
        return mRawDBConf.size();
    }

    ConfDB *Config::getRawDBConf( int index )
    {
        if( index < 0 ) return NULL;
        if( mRawDBConf.size() <= index ) return NULL;
        return mRawDBConf[index];
    }
    
    Config::Config()
    {
        // デフォルト値で埋める
        mConfPath = "/etc/vcnctd.conf";
        mPort = 52525;

        // 設定ファイルを読み取りモードで開く
        FILE *fp = fopen( getConfPath().c_str(), "r" );
        if( !fp )
        {
            return;
        }

        // 各行について調べる．
        char buf[4096];
        // current_parseには，現在パースしているエントリ[db]等の文字列が入る
        string current_parse = "";
        bool skip_line = false;
        string line;
        while( true )
        {
            if( skip_line )
            {
                skip_line = false;
            }
            else
            {
                if( fgets( buf, 4096, fp ) )
                {
                    line = buf;
                }
                else
                {
                    break;
                }
            }
            cout << "Config::load; line=" << line << endl;
            int indx_cr = line.find( "\n" );
            if( indx_cr != string::npos )
            {
                line = line.substr( 0, indx_cr );
            }
            
            if( line.find( "[db]" ) == 0 )
            {
                ConfDB *conf = new ConfDB();
                skip_line = true;
                bool ret = conf->parse( fp, line );
                mRawDBConf.push_back( conf );
                if( !ret ) break;
            }
        }
        fclose( fp );

        for( int i = 0; i < mRawDBConf.size(); i++ )
        {
            string path = mRawDBConf[i]->getPath();
            string charset = mRawDBConf[i]->getCharset();
            cout << "[#" << i << "] path=" << path << "; charset=" << charset << endl;
        }
        
        return;
    }

    string Config::getConfPath()
    {
        return mConfPath;
    }

}
