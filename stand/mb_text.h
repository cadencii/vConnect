/**
 * mb_text.h
 * Copyright (C) 2010 kbinani
 *
 * This file is part of mb_text.
 *
 * mb_text is free software; you can redistribute it and/or
 * modify it under the terms of the BSD License.
 *
 * mb_text is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifndef __mb_text_h__
#define __mb_text_h__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <clocale>
#include <locale>
#include <iostream>
#include "Socket.h"

using namespace std;

#ifndef LINEBUFF_LEN
#define LINEBUFF_LEN 4096
#endif

#if defined(__APPLE__) && (defined(__GNUC__) || defined(__xlC__) || defined(__xlc__))
#  define MB_OS_MAC
#elif !defined(SAG_COM) && (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#  define MB_OS_WIN
#elif !defined(SAG_COM) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#  define MB_OS_WIN
#elif defined(__MWERKS__) && defined(__INTEL__)
#  define MB_OS_WIN
#else
#  define MB_OS_OTHER
#endif

// libiconvを使う場合MB_USE_ICONVを定義
#ifdef MB_OS_WIN
// どうする？
#define MB_USE_ICONV
#else
#define MB_USE_ICONV
#endif

// コードページ記述子
// libiconvを使うならiconv_t，使わないならUINT．
#ifdef MB_USE_ICONV
#ifdef MB_OS_WIN
// static link with libiconv
// (1) prepare libiconv.lib
//     (1-1) download http://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.13.tar.gz
//           and http://www2d.biglobe.ne.jp/~msyk/software/libiconv/libiconv-1.13-ja-1.patch.gz
//     (1-2) unzip
//     (1-3) apply patch: 
//           patch -p1 -N < ../libiconv-1.13-ja-1.patch
//     (1-4) build
//           ./configure --enable-static --disable-shared
//           make
//     (1-5) cd lib/.libs
//     (1-6) generate static library:
//           lib libiconv.a /OUT:libiconv.lib
// (2) prepare libgcc.lib
//     (2-1) find libgcc.a
//           usually found in {install-dir-mingw32}/lib/gcc/mingw32/{gcc-version}/libgcc.a
//     (2-2) generate static library:
//           lib libgcc.a /OUT:libgcc.lib
// (3) copy libiconv.lib and libgcc.lib
//
// VC++の場合、iconv.dllが必要になる。
// g++の場合、libiconvを./configureするとき--enable-static, --disable-sharedとすればstaticリンクされ、dllに依存しないバイナリを作れる
// また、↑のURLで配布されているiconv.dllは、\を含む文字列を正しく変換できない場合があるので、VC++よりもg++でコンパイルするのがお勧めです
//#pragma comment(lib, "iconv.lib")
#endif
#include <iconv.h>
typedef iconv_t MB_CODEPAGE_DESCRIPTER;
#else
#include <windows.h>
typedef UINT MB_CODEPAGE_DESCRIPTER;
#endif

// 無効な記述子
#define MB_INVALID ((MB_CODEPAGE_DESCRIPTER)-1)

// コードページ指定込みのファイルを表す型．
typedef struct{
    // wcharに変換するための記述子
    MB_CODEPAGE_DESCRIPTER descripter_for_wchar;
    // charに変換するための記述子
    MB_CODEPAGE_DESCRIPTER descripter_for_char;
    // ファイルポインタ
    FILE *file;
    // 文字列の読み込み単位．unit_lenバイトずつファイルから読んでいく
    int unit_len;
    // ファイルからunit_lenずつ読み込むときに使うバッファ
    char *unit;
    int socket;
    // シークして戻る操作を行った時の，バッファとして保持しておく文字
    char *charbuf;
} MB_FILE;

/**
 * 指定したコードページを取り扱いできるかどうかを調べます．
 */
bool mb_is_valid_codepage( unsigned int codepage );

/**
 * コードページの名称から、読込み時の読込単位長さ(通常はバイト)を調べます
 */
int mb_unitlen_from_charset_name( const char *name );

/**
 * ASCII文字列のアルファベットを全て小文字に変換します
 */
void mb_tolower( string& s );

/**
 * コードページの名称から、コードページ番号を取得します
 */
unsigned int mb_codepage_from_charset_name( const char *name );

/**
 * コードページ番号から、コードページの名称を取得します
 */
const char *mb_charset_name_from_codepage( unsigned int codepage );

/**
 * 指定したコードページを使って、読込み専用のテキストファイルを開く
 */
MB_FILE *mb_fopen( const char *file_name, const char *codepage );

/**
 * 指定したコードページを使って、読込み専用のテキストファイルを開く
 */
MB_FILE *mb_fopen( string file_name, const char *codepage );

/**
 * 指定したコードページを使って、読込み専用のテキストファイルを開く
 */
MB_FILE *mb_fopen( wstring file_name, const char *codepage );

/// <summary>
/// 指定したコードページを使って，ソケットを読み込む
/// </summary>
MB_FILE *mb_fopen( int socket, const char *codepage );

/**
 * ファイルを閉じる
 */
int mb_fclose( MB_FILE *fp );



/**
 * 内部関数
 * マルチバイト文字列が、指定したコードの制御文字を表すかどうかを調べます
 * @param buf 調査対象のマルチバイト文字列のバッファ
 * @param len バッファbufの長さ
 * @param check_char チェックする制御文字コード
 * @return バッファの文字列が指定した制御文字を表すならtrue、そうでなければfalse
 */
bool mb_code_check( char *buf, int len, char check_char );

/**
 * 内部関数．
 * バイト列を指定したコードページ記述子を用いて変換します．
 */
void mb_code_conv( char *buf, int buflen, MB_CODEPAGE_DESCRIPTER descripter );

/**
 * ファイルから1行読み込む．
 * mb_fopenでファイルを開いたときに指定したコードページは無視される．
 * 改行はCR,CRLF,LFの3種類に対応．
 */
bool mb_fgets( string& line, MB_FILE *fp );

/**
 * ファイルから1行読み込む．
 * mb_fopenでファイルを開いたときに指定したコードページとみなして読み込む．
 * 改行はCR,CRLF,LFの3種類に対応．
 */
bool mb_fgets( wstring& line, MB_FILE *fp );

#ifdef _DEBUG
/**
 * いろんなエンコーディングのテキストファイルを読み込むテストを実行します．
 */
int mb_test();
#endif

/**
 * ワイド文字列をマルチバイト文字列に変換します．
 */
void mb_conv( const wstring &src, string &dest );

/**
 * マルチバイト文字列をワイド文字列に変換します．
 */
void mb_conv( const string &src, wstring &dest );

/**
 * マルチバイト文字列をマルチバイト文字列に変換します．
 */
void mb_conv( const string &src, string &dest );

/**
 * ワイド文字列をワイド文字列に変換します．
 */
void mb_conv( const wstring &src, wstring &dest );

#endif // __mb_text_h__
