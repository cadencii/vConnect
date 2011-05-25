/**
 * Config.cpp
 *
 * Copyright (C) 2011 kbinani.
 */
#include "Config.h"

namespace vcnctd
{

    string Config::mConfPath = "/etc/vcnctd.conf";
    vector<string> Config::mRawDBPath;

    int Config::getRawDBCount()
    {
        return mRawDBPath.size();
    }

    string Config::getRawDBPath( int index )
    {
        if( index < 0 ) return "";
        if( mRawDBPath.size() <= index ) return "";
        return mRawDBPath[index];
    }
    
    int Config::load()
    {
        // 設定ファイルを読み取りモードで開く
        FILE *fp = fopen( getConfPath().c_str(), "r" );
        if( !fp )
        {
            return 0;
        }

        // 各行について調べる．
        char buf[4096];
        // current_parseには，現在パースしているエントリ[db]等の文字列が入る
        string current_parse = "";
        while( fgets( buf, 4096, fp ) )
        {
            string line = buf;
            int indx_cr = line.find( "\n" );
            if( indx_cr != string::npos )
            {
                line = line.substr( 0, indx_cr );
            }
            
            if( line.find( "[" ) == 0 )
            {
                current_parse = line;
            }
            else
            {
                if( current_parse.compare( "[db]" ) == 0 )
                {
                    int indx = line.find( "=" );
                    if( indx > 0 )
                    {
                        string key = line.substr( 0, indx );
                        string value = line.substr( indx + 1 );
                        if( key.compare( "path" ) == 0 )
                        {
                            mRawDBPath.push_back( value );
                        }
                    }
                }
            }
        }
        fclose( fp );

        return 1;
    }

    string Config::getConfPath()
    {
        return Config::mConfPath;
    }

}
