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
        /// 設定値を読み込みます
        /// </summary>
        /// <returns>読み込みに失敗した場合0，成功した場合は0以外の値を返します．</returns>
        static int load();

        /// <summary>
        /// 設定ファイルのパスを取得します．
        /// </summary>
        /// <returns>設定ファイルのパス．</returns>
        static string getConfPath();

        /// <summary>
        /// 未解析の音源の個数を取得します
        /// </summary>
        /// <returns>未解析音源の個数</returns>
        static int getRawDBCount();

        /// <summary>
        /// 第index番目の未解析音源の設定を取得します．
        /// </summary>
        /// <param name="index">取得する音源のインデックス</param>
        /// <returns>未解析音源の設定</returns>
        static ConfDB *getRawDBConf( int index );

        /// <summary>
        /// このクラスで確保したメモリーを破棄します．
        /// </summary>
        static void destroy();
        
    private:
        /// <summary>
        /// vcnctd.confファイルのパス
        /// </summary>
        static string mConfPath;

        /// <summary>
        /// 未解析のUTAU音源の設定リスト．
        /// </summary>
        static vector<ConfDB *> mRawDBConf;
    };

}
#endif
