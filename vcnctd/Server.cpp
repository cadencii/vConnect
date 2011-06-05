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
        int s[SOCK_MAX + 1];
        for( int i = 0; i < SOCK_MAX + 1; i++ )
        {
            s[i] = -1;
        }

        // ソケット接続のステータスコード
        const int ST_NONE = 0;      // 何もしてない状態
        const int ST_READ = 1;  // メタテキストを受信中の状態
        const int ST_SYNTH = 2;     // 合成処理中の状態
        
        // ソケット接続のステータス
        int status[SOCK_MAX + 1];
        for( int i = 0; i < SOCK_MAX + 1; i++ )
        {
            status[i] = ST_NONE;
        }

        // メタテキストを書き込むためのファイルポインタ
        FILE *text[SOCK_MAX + 1];
        for( int i = 0; i < SOCK_MAX + 1; i++ )
        {
            text[i] = NULL;
        }
        
#ifdef WIN32
        // Windows用
        WSADATA data;
        WSAStartup( MAKEWORD( 2, 0 ), &data );
#endif

        // ソケット作る
        if( (s[0] = socket( AF_INET, SOCK_STREAM, 0 )) == -1 )
        {
            // エラーなので死ぬ
            printf( "error; create first socket; s[0]=%d\n", s[0] );
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
        const int STR_LEN = 1024;
        char str[STR_LEN];
        
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
                switch( status[i] )
                {
                    case ST_NONE:{
                        int msg_len = read( s[i], str, STR_LEN );
                        if( msg_len > 0 )
                        {
                            string s_str = str;
                            int indx = s_str.find( "\n" );
                            if( indx > 0 ) s_str = s_str.substr( 0, indx );
                            if( s_str.find( "{seq_start}" ) == 0 )
                            {
                                // 合成処理の指示だったばあい
                                char msg[] = "{accepted}";
                                write( s[i], (char *)msg, strlen( msg ) );
                                if( text[i] ) fclose( text[i] );
                                string work = this->config->getWorkDir();
                                char wave_path[1024];
                                sprintf( wave_path, "%s/%d.txt", work.c_str(), i );
                                text[i] = fopen( wave_path, "w" );
                                status[i] = ST_READ;
                            }
                            else
                            {
                                // その他不明な指示
                                printf( "info; #%d said \"%s\"\n", i, s_str.c_str() );
                                char msg[] = "{unknown}";
                                write( s[i], (char *)msg, strlen( msg ) );
                            }
                        }
                        else if( msg_len < 0 )
                        {
                            // 受信に失敗
                            printf( "warning; read failed from #%d\n", i );
                        }
                        else
                        {
                            // 接続が切られた
                            printf( "info; %d disconnected\n", i );
                            close( s[i] );
                            s[i] = 0;
                            status[i] = ST_NONE;
                        }
                        break;
                    }
                    case ST_READ:{
                        int msg_len = read( s[i], str, STR_LEN );
                        if( msg_len > 0 )
                        {
                            string s_str = str;
                            int indx = s_str.find( "\n" );
                            if( indx > 0 ) s_str = s_str.substr( 0, indx );
                            if( s_str.find( "{synth}" ) == 0 )
                            {
                                char msg[] = "{accepted}";
                                write( s[i], (char *)msg, strlen( msg ) );
                                status[i] = ST_SYNTH;
                                FILE *fp = text[i];
                                if( fp ) fclose( fp );
                                string work = this->config->getWorkDir();
                                char txt[1024];
                                char wav[1024];
                                sprintf( txt, "%s/%d.txt", work.c_str(), i );
                                sprintf( wav, "%s/%d.wav", work.c_str(), i );
                                runtimeOptions opts;
                                synthesize( txt, wav );
                                status[i] = ST_NONE;
                            }
                            else
                            {
                                FILE *fp = text[i];
                                if( fp )
                                {
                                    cout << "info; #" << i << " " << s_str << endl;
                                    fprintf( fp, "%s\n", s_str.c_str() );
                                }
                                char msg[] = "{accepted}";
                                write( s[i], (char *)msg, strlen( msg ) );
                            }
                        }
                        else if( msg_len < 0 )
                        {
                            // 受信に失敗
                            printf( "warning; read failed from #%d\n", i );
                        }
                        else
                        {
                            // 接続が切られた
                            printf( "info; %d disconnected\n", i );
                            close( s[i] );
                            s[i] = 0;
                            status[i] = ST_NONE;
                        }
                        break;
                    }
                }
                
                
                
                /*int msg_length = read( s[i], str, sizeof( str ) );
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
                }
                else
                {
                    // 接続が切られた
                    printf( "info; %d disconnected\n", i );
                    close( s[i] );
                    s[i] = 0;
                    status[i] = ST_NONE;
                }*/
            }
            
            // 新しい接続がないかな
            if( FD_ISSET( s[0], &readfds ) != 0 )
            {
                printf( "info; new connection\n" );
                
                struct sockaddr_in client_addr;
#ifdef WIN32
                int len;
#else
                socklen_t len;
#endif
                len = sizeof( client_addr );
                
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

    int Server::synthesize( char *txt, char *wav )
    {
        printf( "info; Server::synthesize; txt=%s; wav=%s\n", txt, wav );
        Synth synth;
        
        runtimeOptions opts;
        
        string str_txt = txt;
        string_t tstr_txt;
        mb_conv( str_txt, tstr_txt );

        string str_wav = wav;
        string_t tstr_wav;
        mb_conv( str_wav, tstr_wav );
        
        bool ret = synth.synthesize( tstr_txt, tstr_wav, opts );
        
        cout << "info; Server::synthesize; done; ret=" << (ret ? "true" : "false") << endl;
        return ret ? 1 : 0;
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
            string_t tstr_path;
            mb_conv( path, tstr_path );
            db->read( tstr_path, charset.c_str() );
            
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
