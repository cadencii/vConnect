/*
 * Connection.cpp
 *
 * Copyright (C) 2011 kbinani.
 */

#include "Connection.h"

namespace vcnctd
{
    Socket Connection::get( int number )
    {
        if( number <= 0 )
        {
            return mSockets[0];
        }
        else
        {
            map<int, Socket>::iterator itr = mMap.find( number );
            if( itr != mMap.end() )
            {
                int index = mMap[number];
                return mSockets[index];
            }
            else
            {
                return NULL;
            }
        }
    }
    
    void Connection::initSockets( Config *config )
    {
        //int s[SOCK_MAX + 1]; /* array of socket descriptors */
        int max = 0;         /* max num of used socket */
        int n = 0;
        socklen_t len;
        fd_set readfds;
        int clilen;               /* client length */
        struct sockaddr_in saddr; /* サーバ側(自分)のアドレス情報 */
        struct sockaddr_in caddr; /* クライアント側のアドレス情報を入れるところ */
        char str[1024];           /* string buffer */
        int i, j;                 /* loop counter */
        int msglen;
        
        /* 
         *  ソケットを作る。
         *  このソケットはINETドメインでストリーム型(コネクション型) 。
         */
        if( (mSockets[0] = socket( AF_INET, SOCK_STREAM, 0 )) == -1 )
        {
            perror( "socket" );
            exit( 1 );
        }
        
        /* 
         * saddrの中身を0にしておかないと、bind()でエラーが起こることがある
         */
        memset( (char *)&saddr, 0, sizeof( saddr ) );
        
        /* ソケットに名前をつける bind() */
        saddr.sin_family = AF_INET;
        saddr.sin_addr.s_addr = INADDR_ANY;
        saddr.sin_port = htons( config->getPort() ); /* 定数PORTは#defineしたもの */
        
        if( (bind( mSockets[0], (struct sockaddr *)&saddr, sizeof( saddr ) )) == -1 )
        {
            perror( "bind" );
            exit( 1 );
        }
        
    }
    
}
