/**
 * TextReader.h
 *
 * Copyright (C) 2011 kbinani.
 */
#ifndef __TextReader_h__
#define __TextReader_h__

#include "../mb_text.h"
#include "Socket.h";

/// <summary>
/// ファイルまたはウェブソケットから1行ずつデータを読み込む機能を提供します．
/// </summary>
class TextReader
{
public:
    
    /// <summary>
    /// コンストラクタ．ファイルから読み込みます．
    /// </summary>
    /// <param name="fp">読み込むファイルのポインタ</param>
    TextReader( MB_FILE *fp );
    
    /// <summary>
    /// コンストラクタ．ウェブソケットから読み込みます．
    /// </summary>
    /// <param name="socket">読み込むソケット接続</param>
    TextReader( Socket socket );

    /// <summary>
    /// デストラクタ．ファイルやソケットのclose処理は行いません．
    /// </summary>
    ~TextReader();

    /// <summary>
    /// 次の行を取得します．
    /// </summary>
    /// <param name="buffer">行データの格納先</param>
    /// <returns>読み込みに失敗した場合0，それ以外は0でない値を返します．</returns>
    int getNextLine( wstring &buffer );
    
    /// <summary>
    /// 次の行を取得します．
    /// </summary>
    /// <param name="buffer">行データの格納先</param>
    /// <returns>読み込みに失敗した場合0，それ以外は0でない値を返します．</returns>
    int getNextLine( string &buffer );
    
private:
    
    TextReader();

    
private:
    
    /// <summary>
    /// ファイルのポインタ．
    /// </summary>
    MB_FILE *mFile;
    
    /// <summary>
    /// ソケット接続
    /// </summary>
    Socket mSocket;
    
};

#endif
