/*
 * Connection.h
 *
 * Copyright (C) 2011 kbinani.
 */
#ifndef __Connection_h__
#define __Connection_h__

namespace vcnctd
{

    typedef int Socket;

    /// <summary>
    /// ソケット接続を管理します．
    /// </summary>
    class Connection
    {
    public:
        
        /// <summary>
        /// 第number番目に接続したクライアントとのソケット接続を取得します．
        /// </summary>
        /// <param name="number">クライアントとの接続番号</param>
        /// <returns>ソケット接続</returns>
        static Socket get( int number );
        
    public:
        
        /// <summary>
        /// 許可する同時最大接続数．
        /// </summary>
        const int MAX_CONS = 128;
        
    private:
        
        
        
    };
    
}

#endif
