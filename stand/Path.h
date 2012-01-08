/*
 * Path.h
 * Copyright © 2012 kbinani
 *
 * This file is part of vConnect-STAND.
 *
 * vConnect-STAND is free software; you can redistribute it and/or
 * modify it under the terms of the GPL License.
 *
 * vConnect-STAND is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifndef __Path_h__
#define __Path_h__

#ifdef _WIN32
#include <windows.h>
#include <shlwapi.h>
#endif
#include <string>
#include <sys/stat.h>

namespace vconnect
{

    using namespace std;

    class Path
    {
    public:
        /**
         * 絶対パスを取得する
         * @param path パス
         * @return path の絶対パス
         */
        static string getFullPath( string path )
        {
#ifdef _WIN32
            char resolvedPath[MAX_PATH];
            GetFullPathNameA( path.c_str(), MAX_PATH * sizeof( char ), resolvedPath, NULL );
            string result = resolvedPath;
            if( exists( result ) ){
                return result;
            }else{
                return "";
            }
#else
            char resolvedPath[PATH_MAX];
            string result = "";
            if( realpath( path.c_str(), resolvedPath ) ){
                result = resolvedPath;
            }
            return result;
#endif
        }

        /**
         * パス文字列をつなげる
         * @param path1 パス
         * @param path2 パス
         * @return 連結されたパス文字列
         */
        static string combine( string path1, string path2 )
        {
            string separator = getDirectorySeparator();
            if( path1.length() > 0 && path1.rfind( separator ) == path1.length() - separator.length() ){
                path1 = path1.substr( 0, path1.length() - separator.length() );
            }
            if( path2.rfind( separator ) == 0 ){
                path2 = path2.substr( separator.length() );
            }
            return path1 + separator + path2;
        }

        /**
         * ディレクトリの区切り文字を取得する
         * @return 区切り文字
         */
        static string getDirectorySeparator()
        {
            #ifdef _WIN32
                return "\\";
            #else
                return "/";
            #endif
        }

        /**
         * ディレクトリのパスを取得する
         * @param path パス
         * @return ディレクトリのパス
         */
        static string getDirectoryName( string path )
        {
            string separator= getDirectorySeparator();
            string::size_type index = path.rfind( separator );
            if( index != string::npos ){
                return path.substr( 0, index );
            }else{
                return path;
            }
        }

        /**
         * パスが存在するかどうか調べる
         * @param path パス
         * @return ファイルまたはディレクトリが存在すれば true を返す
         */
        static bool exists( string path )
        {
            #ifdef _WIN32
                return PathFileExists( path.c_str() ) ? true : false;
            #else
                struct stat statResult;
                return (stat( path.c_str(), & statResult ) == 0) ? true : false;
            #endif
        }

    private:
        /**
         * 隠蔽されたコンストラクタ
         */
        Path()
        {
        }
    };

}

#endif
