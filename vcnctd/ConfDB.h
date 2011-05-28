/*
 * ConfDB.h
 *
 * Copyright (C) 2011 kbinani.
 */
#ifndef __ConfDB_h__
#define __ConfDB_h__

#include <string>
#include <iostream>
#include <stdio.h>

using namespace std;

namespace vcnctd
{

    /// <summary>
    /// vcnctd.confの[db]エントリの中身を保持・解析するクラスです．
    /// </summary>
    class ConfDB
    {
    public:

        /// <summary>
        /// コンストラクタ．規定値を入れる処理をします．
        /// </summary>
        ConfDB();

        /// <summary>
        /// confファイルをパースします．
        /// </summary>
        /// <param name="fp">confファイル</param>
        /// <returns>次に解析すべき行</returns>
        bool parse( FILE *fp, string &last_line );
        
        /// <summary>
        /// oto.iniファイルのパスを取得します．
        /// </summary>
        /// <returns>oto.iniファイルのパス</returns>
        string getPath();
        
        /// <summary>
        /// oto.iniファイルのテキスト・エンコーディングを取得します．
        /// </summary>
        /// <returns>oto.iniファイルのテキストエンコーディング</returns>
        string getCharset();
        
        
    private:

        /// <summary>
        /// oto.iniファイルのパス
        /// </summary>
        string mPath;

        /// <summary>
        /// oto.iniファイルのキャラクタ・エンコーディング．
        /// デフォルト値は"UTF-8"．
        /// </summary>
        string mCharset;
    };
    
}
#endif
