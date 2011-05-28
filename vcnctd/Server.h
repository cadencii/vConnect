/**
 * Server.h
 *
 * Copyright © 2011 kbinani.
 */
#ifndef __Server_h__
#define __Server_h__

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
        
        ~Server();
        
        /// <summary>
        /// 未解析の音源について，解析を行います．
        /// </summary>
        void analyze();
        
    public:
        
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
        
    };
    
}

#endif
