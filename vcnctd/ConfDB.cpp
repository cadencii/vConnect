/*
 * ConfDB.cpp
 *
 * Copyright (C) 2011 kbinani.
 */
#include "ConfDB.h"

namespace vcnctd
{

    ConfDB::ConfDB()
    {
        this->mPath = "";
        this->mCharset = "UTF-8";
    }

    bool ConfDB::parse( FILE *fp, string &last_line )
    {
        cout << "ConfDB::parse" << endl;
        char buf[4096];
        string line;
        while( true )
        {
            if( fgets( buf, 4096, fp ) )
            {
                line = buf;
            }
            else
            {
                last_line = line;
                return false;
            }
            int indx_cr = line.find( "\n" );
            if( indx_cr > 0 )
            {
                line = line.substr( 0, indx_cr );
            }
            int indx = line.find( "=" );
            if( indx == string::npos )
            {
                if( line.find( "[" ) == 0 )
                {
                    last_line = line;
                    return true;
                }
                continue;
            }
            
            string key = line.substr( 0, indx );
            string value = line.substr( indx + 1 );
            
            if( key.compare( "path" ) == 0 )
            {
                this->mPath = value;
            }
            else if( key.compare( "charset" ) == 0 )
            {
                this->mCharset = value;
            }
        }
        return true;
    }
    
    string ConfDB::getPath()
    {
        return this->mPath;
    }
    
    string ConfDB::getCharset()
    {
        return this->mCharset;
    }
    
}
