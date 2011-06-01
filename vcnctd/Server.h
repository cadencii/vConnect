/**
 * Server.h
 *
 * Copyright © 2011 kbinani.
 */
#ifndef __Server_h__
#define __Server_h__

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define PORT 8765       /* サーバーが使うポート番号  */
#define MAX_SOCKETS 5      /* 最大ソケット数をここで決めた (最大32)*/
#define UNUSED (-1)

#include "Config.h"
#include "Connection.h"
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
        void startListening();        
        
        /// <summary>
        /// 未解析の音源について，解析を行います．
        /// </summary>
        void analyze();
        

    public:
        
        /// <summary>
        /// ソケット接続を管理するインスタンス．
        /// </summary>
        Connection *cons;
        
        /// <summary>
        /// 合成器のインスタンスです．
        /// </summary>
        Synth *synth;
        
        /// <summary>
        /// サーバーの設定値を保持するインスタンスです．
        /// </summary>
        Config *config;

        
    private:
        
        /// <summary>
        /// 解析済み音源を保持するインスタンスのリスト．
        /// </summary>
        vector<CorpusManager *> mCorpusManagers;

        /// <summary>
        /// ソケットのリスト
        /// </summary>
        //int mSockets[MAX_SOCKETS];
        
    };
    
}

#endif
