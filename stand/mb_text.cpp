/**
 * mb_text.cpp
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
#include "mb_text.h"
#include <cctype>
#ifdef MB_OS_MAC
#include <stdlib.h>
#else
#include <malloc.h>
#endif

#ifdef MB_OS_WIN
//#define MB_USE_STDLIB
#elif defined( MB_OS_MAC )
//#define MB_USE_STDLIB
#else // MB_OS_WIN
//#define MB_USE_STDLIB
#endif // MB_OS_WIN

static bool mb_initialized = false;
static MB_CODEPAGE_DESCRIPTER mb_descripter_for_wchar2char = NULL;
static MB_CODEPAGE_DESCRIPTER mb_descripter_for_char2wchar = NULL;
static string mb_charset_wchar = "UTF-16LE";
static string mb_charset_char = "Shift_JIS";

bool mb_is_valid_codepage( unsigned int codepage ){
#ifdef MB_USE_ICONV
    // まずUTF-8が有効かどうか
    iconv_t cnv = iconv_open( "UTF-8", "UTF-8" );
    if( cnv == MB_INVALID ){
        iconv_close( cnv );
        return false;
    }
    iconv_close( cnv );

    const char *charset_name = mb_charset_name_from_codepage( codepage );
    iconv_t cnv2 = iconv_open( "UTF-8", charset_name );
    if( cnv2 == MB_INVALID ){
        iconv_close( cnv2 );
        return false;
    }
    iconv_close( cnv2 );

    iconv_t cnv3 = iconv_open( charset_name, "UTF-8" );
    if( cnv3 == MB_INVALID ){
        iconv_close( cnv3 );
        return false;
    }
    iconv_close( cnv3 );
    return true;
#else
#ifdef MB_OS_WIN
    return IsValidCodePage( codepage );
#else
    return false;
#endif
#endif
}

/**
 * ASCII文字列のアルファベットを全て小文字に変換します
 */
void mb_tolower( string& s ){
    int len = s.length();
    int i;
    for( i = 0; i < len; i++ ){
        s[i] = tolower( s[i] );
    }
}

/**
 * コードページ番号から、コードページの名称を取得します
 */
const char *mb_charset_name_from_codepage( unsigned int codepage ){
    switch( codepage ){
        case 932:{
            return "Shift_JIS";
        }
        case 51932: {
            return "euc-jp";
        }
        case 50220: {
            return "iso-2022-jp";
        }
        case 1208:
        case 1209: {
            return "UTF-8";
        }
        case 1202:
        case 1203: {
            return "UTF-16LE";
        }
        case 1200:
        case 1201: {
            return "UTF-16BE";
        }
        case 1204:
        case 1205: {
            return "UTF-16";
        }
        case 1234:
        case 1235: {
            return "UTF-32LE";
        }
        case 1232:
        case 1233: {
            return "UTF-32BE";
        }
        case 1236:
        case 1237: {
            return "UTF-32";
        }
    }
    return "";
}

void mb_init(){
    if( !mb_initialized ){
#ifdef _DEBUG
        printf( "::mb_init; calling setlocale...\n" );
#endif

        // wcharの内部エンコーディングが何か？
        //                                                  UTF-16LE : UTF-32LE
        unsigned int charset_wchar = (sizeof( wchar_t ) == 2) ? 1202 : 1234; // 無理やりですね^ ^;
        mb_charset_wchar = mb_charset_name_from_codepage( charset_wchar );
#ifdef _DEBUG
        cout << "::mb_init; sizeof(wchar_t)=" << sizeof( wchar_t ) << endl;
#endif
#if defined( MB_OS_MAC )
        // Macの場合は決め打ち
        charset_wchar = 1234;
#endif

        // charの内部のエンコーディングが何か？
        unsigned int charset_char = 0;
        char *cname = std::setlocale( LC_CTYPE, "" );
#if defined( MB_OS_MAC )
        // Macは決め打ち
        charset_char = 1208;
#else
        string locale_name = "";
        if( cname != NULL ){
            // locale_name="ja_JP.UTF-8" (openSUSE, g++)
            // locale_name="Japanese_Japan.932" (Windows XP, g++)
            locale_name = cname;
#ifdef _DEBUG
            cout << "::mb_init; locale_name=" << locale_name << endl;
#endif
            int indx_collon = locale_name.rfind( "." );
            if( indx_collon != string::npos ){
                string snum = locale_name.substr( indx_collon + 1 );
                // snum部分が数値に変換できるかどうかを調べる
                if( snum.size() == 0 ){
                    charset_char = 1208;
                }else{
                    bool allnum = true;
                    for( int i = 0; i < snum.size(); i++ ){
                        char c = snum[i];
                        if( isdigit( c ) == 0 ){
                            allnum = false;
                            break;
                        }
                    }
                    if( allnum ){
                        int num = atoi( snum.c_str() );
                        charset_char = num;
                    }else{
                        mb_charset_char = snum;
                    }
                }
            }else{
                charset_char = 1208;
            }
        }else{
            charset_char = 1208;
        }
#endif

        if( charset_char > 0 ){
            // コードページ番号からキャラクタセットの名前を取得
            mb_charset_char = mb_charset_name_from_codepage( charset_char );
        }

#ifdef _DEBUG
        cout << "::mb_init; mb_charset_char=" << mb_charset_char << endl;
        cout << "::mb_init; mb_charset_wchar=" << mb_charset_wchar << endl;
#endif

#ifdef MB_USE_ICONV
        mb_descripter_for_wchar2char = iconv_open( mb_charset_char.c_str(), mb_charset_wchar.c_str() );
        mb_descripter_for_char2wchar = iconv_open( mb_charset_wchar.c_str(), mb_charset_char.c_str() );
#else
        mb_descripter_for_wchar2char = charset_char;
        mb_descripter_for_char2wchar = charset_wchar;
#endif

        mb_initialized = true;
    }
}

/**
 * バッファを指定したコードページに変換します
 */
void mb_code_conv( char *buf, int buffer_bytes, MB_CODEPAGE_DESCRIPTER descripter ){
    if( descripter == MB_INVALID ){
        return;
    }
#ifdef MB_USE_ICONV
    iconv( descripter, NULL, 0, NULL, 0 );
    // 変換用のバッファを確保
    char *outbuf = (char *)malloc( buffer_bytes );
    memset( outbuf, 0, buffer_bytes );
    size_t n_src = buffer_bytes;
    size_t n_dst = buffer_bytes;
    char *p_dst = outbuf;
    char *p_src = buf;
    int last_n_src = n_src;
    while( 0 < n_src ){
        if( 0 >= iconv( descripter, &p_src, &n_src, &p_dst, &n_dst ) ){
            break;
        }
        if( n_src == last_n_src ){
            break;
        }
        last_n_src = n_src;
    }
    memcpy( buf, outbuf, buffer_bytes );
    free( outbuf );
#else
    // とりあえずサイズを調べる
    int size = MultiByteToWideChar( descripter, 0, (LPCSTR)buf, -1, NULL, 0 );
    char *buf_utf16 = (char *)malloc( (size + 1) * sizeof( wchar_t ) );
    // 変換
    MultiByteToWideChar( descripter, 0, (LPCSTR)buf, -1, (LPWSTR)buf_utf16, size );
    memcpy( buf, buf_utf16, buffer_bytes );
    free( buf_utf16 );
#endif
}

// ワイド文字列からマルチバイト文字列
// ロケール依存、mainの最初でsetlocale( LC_CTYPE, "" );すること
void mb_conv( const wstring &src, string &dest ){
    mb_init();
#ifdef MB_USE_STDLIB
    int count = src.length() * MB_CUR_MAX + 1;
    char *mbs = new char[count];
#ifdef __GNUC__
    wcstombs( mbs, src.c_str(), src.length() * MB_CUR_MAX + 1 );
#else
    size_t i;
    wcstombs_s( &i, mbs, sizeof( char ) * count, src.c_str(), src.length() * MB_CUR_MAX + 1 );
#endif
    dest = mbs;
    delete [] mbs;
#else
    dest.clear();
    int src_size = src.size() * sizeof( wchar_t );
    if( src_size > 0 ){
        int dest_size = src.size() * sizeof( char ) * 2;
        if( dest_size < src_size ){
            dest_size = src_size;
        }
        dest_size += dest_size % sizeof( char );
        char *mbs = (char *)malloc( dest_size );
        memset( mbs, 0, dest_size );
        memcpy( mbs, src.c_str(), min( src_size, dest_size ) );
        mb_code_conv( mbs, dest_size, mb_descripter_for_wchar2char );
        dest.append( mbs );
        free( mbs );
    }
#endif
}

// マルチバイト文字列からワイド文字列
// ロケール依存、mainの最初でsetlocale( LC_CTYPE, "" );すること
void mb_conv( const string &src, wstring &dest ){
    mb_init();
#ifdef MB_USE_STDLIB
    int count = src.length() + 1;
    wchar_t *wcs = new wchar_t[count];
#ifdef __GNUC__
    mbstowcs( wcs, src.c_str(), src.length() + 1 );
#else
    size_t i;
    mbstowcs_s( &i, wcs, count, src.c_str(), src.length() + 1 );
#endif
    dest = wcs;
    delete [] wcs;
#else
    dest.clear();
    int src_size = src.size() * sizeof( char );
    if( src_size > 0 ){
        int dest_size = src.size() * sizeof( wchar_t ) * 2;
        if( dest_size < src_size ){
            dest_size = src_size;
        }
        dest_size += (dest_size % sizeof( wchar_t ));
        wchar_t *wcs = (wchar_t *)malloc( dest_size );
        memset( wcs, 0, dest_size );
        memcpy( wcs, src.c_str(), min( src_size, dest_size ) );
        mb_code_conv( (char *)wcs, dest_size, mb_descripter_for_char2wchar );
        dest.append( wcs );
        free( wcs );
    }
#endif
}

void mb_conv( const string &src, string &dest ){
    dest.clear();
    dest.append( src );
}

void mb_conv( const wstring &src, wstring &dest ){
    dest.clear();
    dest.append( src );
}
