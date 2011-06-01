/*
 * Config.h
 *
 * Copyright (C) 2011 kbinani.
 */
#ifndef __Config_h__
#define __Config_h__

#include <string>
#include <stdio.h>
#include <iostream>
#include <vector>

#include "ConfDB.h"

using namespace std;

namespace vcnctd
{

    /// <summary>
    /// vcnctdの実行時の設定値を格納します
    /// </summary>
    class Config
    {
    public:
        /// <summary>
        /// コンストラクタ．設定値を読み込みます
        /// </summary>
        Config();

        /// <summary>
        /// デストラクタ．
        /// </summary>
        ~Config();
        
        /// <summary>
        /// 設定ファイルのパスを取得します．
        /// </summary>
        /// <returns>設定ファイルのパス．</returns>
        string getConfPath();

        /// <summary>
        /// 未解析の音源の個数を取得します
        /// </summary>
        /// <returns>未解析音源の個数</returns>
        int getRawDBCount();

        /// <summary>
        /// 第index番目の未解析音源の設定を取得します．
        /// </summary>
        /// <param name="index">取得する音源のインデックス</param>
        /// <returns>未解析音源の設定</returns>
        ConfDB *getRawDBConf( int index );
        
        /// <summary>
        /// アクセスを受け付けるポート番号を取得します．
        /// </summary>
        /// <returns>アクセスを受け付けるポート番号．</returns>
        int getPort();
        
    private:
        /// <summary>
        /// vcnctd.confファイルのパス
        /// </summary>
        string mConfPath;

        /// <summary>
        /// 未解析のUTAU音源の設定リスト．
        /// </summary>
        vector<ConfDB *> mRawDBConf;
        
        /// <summary>
        /// アクセスを受け付けるポート番号．
        /// </summary>
        int mPort;
    };

}
#endif
