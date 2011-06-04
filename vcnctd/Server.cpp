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
    }

    Server::~Server()
    {
        int num = mCorpusManagers.size();
        for( int i = 0; i < num; i++ )
        {
            CorpusManager *man = mCorpusManagers[i];
            mCorpusManagers[i] = NULL;
            if( man ) delete man;
        }
        mCorpusManagers.clear();
        UtauDB::dbClear();
    }
    
    int Server::startListening()
    {
        const int SOCK_MAX = 128;

        // ソケット接続のリスト
        Socket s[SOCK_MAX + 1];
        for( int i = 0; i < SOCK_MAX + 1; i++ )
        {
            s[i] = -1;
        }

        // ソケット作る
        if( (s[0] = socket( AF_INET, SOCK_STREAM, 0 )) == -1 )
        {
            // エラーなので死ぬ
            printf( "error; create first socket\n" );
            return 0;
        }
        
        // サーバーのアドレス情報
        struct sockaddr_in server_addr;
        memset( (char *)&server_addr, 0, sizeof( server_addr ) );
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons( this->config->getPort() );

        // ソケットとアドレスを紐付け
        if( (bind( s[0], (struct sockaddr *)&server_addr, sizeof( server_addr ) )) == -1 )
        {
            // 紐付けに失敗したので死ぬ
            printf( "error; bind first socket\n" );
            return 0;
        }
        
        // サーバーのソケットを接続待ち
        if( (listen( s[0], SOCK_MAX )) == -1 )
        {
            // 待受開始に失敗したので死ぬ
            printf( "error; listen first socket\n" );
            return 0;
        }

        // 受信メッセージ用の文字列
        char str[1024];
        
        // メインループ．無限ループ
        while( 1 )
        {
            fd_set readfds;
            FD_ZERO( &readfds );
            
            // 接続済みのクライアントのビットを立てる
            for( int i = 0; i < SOCK_MAX + 1; i++ )
            {
                if( s[i] <= 0 ) continue;
                FD_SET( s[i], &readfds );
            }
            
            // メッセージの到着をチェック
            int n;
            if( (n = select( FD_SETSIZE, &readfds, NULL, NULL, NULL )) == -1 )
            {
                printf( "error; select failed\n" );
                return 0;
            }

            // 接続済みソケットからのメッセージをチェック
            for( int i = 1; i < SOCK_MAX + 1; i++ )
            {
                // 接続済み？
                if( s[i] <= 0 ) continue;
                // メッセージ到着している？
                if( !FD_ISSET( s[i], &readfds ) ) continue;

                strcpy( str, "" );
                int msg_length = read( s[i], str, sizeof( str ) );
                if( msg_length < 0 )
                {
                    // 受信に失敗
                    printf( "warning; read failed from #%d\n", i );
                }
                else if( msg_length > 0 )
                {
                    // 受信に成功
                    string s_str = str;
                    int indx = s_str.find( "\n" );
                    if( indx > 0 ) s_str = s_str.substr( 0, indx );
                    printf( "info; #%d said \"%s\"\n", i, s_str.c_str() );
                    char msg[] = "[OK]";
                    write( s[i], (char *)msg, strlen( msg ) );
                }
                else
                {
                    // 接続が切られた
                    printf( "info; %d disconnected\n", i );
                    close( s[i] );
                    s[i] = 0;
                }
            }
            
            // 新しい接続がないかな
            if( FD_ISSET( s[0], &readfds ) != 0 )
            {
                printf( "info; new connection\n" );
                
                struct sockaddr_in client_addr;
                socklen_t len = sizeof( client_addr );
                
                // どれがあいてるかな
                int index = -1;
                for( int i = 1; i < SOCK_MAX + 1; i++ )
                {
                    if( s[i] <= 0 )
                    {
                        index = i;
                        break;
                    }
                }
                if( index < 0 )
                {
                    // 最大接続数をオーバーしてる
                    printf( "warning: server too busy\n" );
                }
                else
                {
                    s[index] = accept( s[0], (struct sockaddr *)&client_addr, &len );
                    if( s[index] == -1 )
                    {
                        printf( "error; accept failed\n" );
                        return 0;
                    }
                    else
                    {
                        printf( "info; #%d connection accepted\n", index );
                    }
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
