/**
 * Server.cpp
 *
 * Copyright (C) 2011 kbinani.
 */
#include "Server.h"

namespace vcnctd
{

    Server::Server()
    {
        this->synth = new Synth();
        this->config = new Config();
        this->cons = new Connection();
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
    
    void Server::startListening()
    {
        // ソケット接続を初期化．
        this->cons->initSockets();
        
        /* 接続されるのを待つ listen() */
        /* 第2引数の値を大きくする */
        if( (listen( this->cons->get( 0 ), MAX_SOCKETS )) == -1 )
        {
            perror( "listen" );
            exit( 1 );
        } 
        max = 1;   /* s[0] is ready */
        
        /* メインループ */
        while( 1 )
        {
            FD_ZERO( &readfds );    /* fdsの初期化 */
            printf( "max: %d\n", max );
            /* 
             * すでにクライアントと接続されているソケットのビットを
             * セットしてselect()の検査対象にする。
             */
            for( i = 0; i < max; i++ )
            {
                if( mSockets[i] != UNUSED )
                {
                    FD_SET( mSockets[i], &readfds );
                }
            }
            /* 
             * メッセージが到着しているソケットがないか調べる
             */
            if( (n = select( FD_SETSIZE, &readfds, NULL, NULL, NULL )) == -1 )
            {
                perror( "select" );
                exit( 1 );
            }
            printf( "select returns: %d\n", n );
            /*   
             *  1～maxのソケットに届くのは、すでに接続されているクライアントからの
             *  メッセージである。
             *  もしメッセージがあれば、接続されているすべてのクライアントに送る。
             */
            for( i = 1; i < max; i++ )
            {
                if( mSockets[i] != UNUSED )
                {
                    if( FD_ISSET( mSockets[i], &readfds ) )
                    {
                        /* 
                         * mSockets[i]のビットが立っていれば、mSockets[i]にメッセージが到着している
                         */
                        printf( "s[%d] ready for reading\n", i );
                        if( (msglen = read( mSockets[i], str, sizeof( str ) )) == -1 )
                        {
                            /* 受信失敗 */
                            perror("read");
                        }
                        else if ( msglen != 0 )
                        {
                            /* メッセージの受信に成功 */
                            printf( "client[%d]: %s", i, str );
                            //Modification(str,msglen);   /* 作業 */
                            /* 接続されているクライアントすべてにメッセージを送る */
                            for( j = 1; j < max; j++ )
                            {
                                if( mSockets[j] != UNUSED )
                                {
                                    write( mSockets[j], str, strlen( str ) );
                                }
                            }
                        }
                        else
                        {
                            printf( "client[%d]: connection closed.\n", i );
                            close( mSockets[i] );
                            mSockets[i] = UNUSED;
                        }
                    }
                }
            }	
            /* 
             *  新たな接続要求があった場合は、s[0]のビットが立つ。
             *  以下では新たな接続の受け付けを行う。
             */
            if( FD_ISSET( mSockets[0], &readfds ) != 0 )
            {
                printf( "Accept New one.\n" );
                /* 接続を確立する accept() */
                len = sizeof( caddr );
                mSockets[max] =
                    accept( mSockets[0], (struct sockaddr *)&caddr, &len );
                printf( "%d = accept()\n", mSockets[max] );
                if( mSockets[max] == -1 )
                {
                    perror( NULL );
                    exit( 1 );
                }
                if( max < MAX_SOCKETS )
                {
                    printf( "client accepted(%d).\n", max );
                    max++;
                }
                else
                {
                    printf( "refuse connection.\n" );
                    strcpy( str, "Server is too busy.\n" );
                    write( mSockets[max], str, strlen( str ) );
                    close( mSockets[max] );
                }
            }
        }
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
