/*
 * Connection.h
 *
 * Copyright (C) 2011 kbinani.
 */
#ifndef __Connection_h__
#define __Connection_h__

#include <sys/socket.h>
#include <netinet/in.h> 
#include <map>

#include "Config.h"

using namespace std;

namespace vcnctd
{

    typedef int Socket;

    /// <summary>
    /// ソケット接続を管理します．
    /// </summary>
    class Connection
    {
    public: // public method
        
        /// <summary>
        /// 第number番目に接続したクライアントとのソケット接続を取得します．
        /// </summary>
        /// <param name="number">クライアントとの接続番号</param>
        /// <returns>ソケット接続</returns>
        Socket get( int number );
        
        /// <summary>
        /// ソケットを用意します．
        /// </summary>
        void initSockets( Config *config );
        
    public: // public static field
        
        /// <summary>
        /// 許可する同時最大接続数．
        /// </summary>
        static const int MAX_CONS = 128;
        
    private:

        /// <summary>
        /// ソケット接続のリスト．
        /// </summary>
        Socket mSockets[MAX_CONS];
        
        /// <summary>
        /// 接続番号とソケットリストのインデックスを紐付けるマップ．
        /// </summary>
        map<int, int> mMap;
        
    };
    
}

#endif
