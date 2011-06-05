/**
 * Server.h
 *
 * Copyright © 2011 kbinani.
 */
#ifndef __Server_h__
#define __Server_h__

#ifdef WIN32
    #include <winsock2.h>
#else
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <sys/time.h>
    #include <unistd.h>
#endif
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#define PORT 8765       /* サーバーが使うポート番号  */
#define MAX_SOCKETS 5      /* 最大ソケット数をここで決めた (最大32)*/
#define UNUSED (-1)

#include "../stand/Socket.h"
#include "Config.h"
#include "vcnctd.h"

using namespace vcnctd;
using namespace vcnctd::stand;

namespace vcnctd
{
    
    /// <summary>
    /// vcnctdサーバーの実体
    /// </summary>
    class Server
    {
    public:
        /// <summary>
        /// コンストラクタ．
        /// </summary>
        Server();

        /// <summary>
        /// デストラクタ．
        /// </summary>
        ~Server();
        
        /// <summary>
        /// ポートのLISTENを開始します．
        /// </summary>
        int startListening();        
        
        /// <summary>
        /// 未解析の音源について，解析を行います．
        /// </summary>
        void analyze();
        
        /// <summary>
        /// VSQメタテキストを読み込み，合成処理を行います．
        /// </summary>
        /// <param name="txt">メタテキストファイルのパス</summary>
        /// <param name="wav">合成結果の出力先</summary>
        /// <returns>合成に成功した場合は0以外の値を，そうでない場合は0を返します．</returns>
        int synthesize( char *txt, char *wav );

    public:

        /// <summary>
        /// サーバーの設定値を保持するインスタンスです．
        /// </summary>
        Config *config;

        
    private:
        
        /// <summary>
        /// 解析済み音源を保持するインスタンスのリスト．
        /// </summary>
        vector<CorpusManager *> mCorpusManagers;

        int mMax;
        /// <summary>
        /// ソケットのリスト
        /// </summary>
        //int mSockets[MAX_SOCKETS];
        
    };
    
}

#endif
