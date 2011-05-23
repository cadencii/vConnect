/*
 * Config.h
 *
 * Copyright (C) 2011 kbinani.
 */
#ifndef __Config_h__
#define __Config_h__

#include <string>

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
        /// <returns>読み込み失敗した場合0，成功した場合は0以外の値を返します．</returns>
        static int load();

        /// <summary>
        /// 設定ファイルのパスを取得します．
        /// </summary>
        /// <returns>設定ファイルのパス．</returns>
        static string getConfPath();


    private:
        /// <summary>
        /// vcnctd.confファイルのパス
        /// </summary>
        static string mConfPath;
    };

}
#endif
