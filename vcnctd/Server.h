/**
 * Server.h
 *
 * Copyright © 2011 kbinani.
 */
#if !defined( __Server_h__ )
#define __Server_h__

#if defined( WIN32 )
    #include <windows.h>
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

        /// <summary>
        /// 指定したwaveファイルをソケットに送信します．
        /// </summary>
        /// <param name="socket">送信対象のソケット</param>
        /// <param name="wav">送信するwaveファイルのパス</param>
        /// <returns>全ての送信に成功した場合は0以外の値を，そうでない場合は0を返します．</returns>
        int sendWave( Socket socket, char *wav );
        
    public:

        /// <summary>
        /// サーバーの設定値を保持するインスタンスです．
        /// </summary>
        Config *config;

        
    private:
        
        /// <summary>
        /// ソケットにバッファーを書き込みます．
        /// バッファーを1回で書ききれない場合，複数回に分けて書き込みます．
        /// </summary>
        /// <param name="socket">書込み先のソケット</param>
        /// <param name="buffer">書きこむバッファ</param>
        /// <param name="length">書きこむバッファの長さ</param>
        /// <returns>書き込んだバッファーの長さ．書き込みに失敗した場合は-1を返します．
        /// 途中で失敗した場合，書き込みが完了したバッファーの長さを返します．</returns>
        int sendBuffer( Socket socket, char *buffer, int length );
        
        /// <summary>
        /// ソケットを閉じます
        /// </summary>
        /// <param name="socket">閉じるソケット</param>
        /// <returns>閉じる操作に成功した場合は0以外の値を，そうでない場合は0を返します．</returns>
        int closeSocket( Socket socket );
        
    private:
        
        /// <summary>
        /// 解析済み音源を保持するインスタンスのリスト．
        /// </summary>
        vector<CorpusManager *> mCorpusManagers;
        
    };
    
}

#endif
