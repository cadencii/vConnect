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
 * コードページの名称から、読込み時の読込単位(バイト)を調べます
 */
int mb_unitlen_from_charset_name( const char *name ){
    string sname = name;
    mb_tolower( sname );
    if( sname.compare( "shift_jis" ) == 0 ){
        return 1;
    }else if( sname.compare( "euc-jp" ) == 0 ){
        return 1;
    }else if( sname.compare( "iso-2022-jp" ) == 0 ){
        return 1;
    }else if( sname.compare( "utf-8" ) == 0 ){
        return 1;
    }else if( sname.compare( "utf-16le" ) == 0 ){
        return 2;
    }else if( sname.compare( "utf-16be" ) == 0 ){
        return 2;
    }else if( sname.compare( "utf-16" ) == 0 ){
        return 2;
    }else if( sname.compare( "utf-32le" ) == 0 ){
        return 4;
    }else if( sname.compare( "utf-32be" ) == 0 ){
        return 4;
    }else if( sname.compare( "utf-32" ) == 0 ){
        return 4;
    }else{
        return 1;
    }
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
 * コードページの名称から、コードページ番号を取得します
 */
unsigned int mb_codepage_from_charset_name( const char *name ){
    string sname = name;
    mb_tolower( sname );
    if( sname.compare( "shift_jis") == 0 ){
        return 932;
    }else if( sname.compare( "euc-jp" ) == 0 ){
        return 51932;
    }else if( sname.compare( "iso-2022-jp" ) == 0 ){
        return 50220;
    }else if( sname.compare( "utf-8" ) == 0 || sname.compare( "utf8" ) == 0 ){
        return 1208;
    }else if( sname.compare( "utf-16le"  ) == 0 || sname.compare( "utf16le" ) == 0 ){
        return 1202;
    }else if( sname.compare( "utf-16be" ) == 0 || sname.compare( "utf16be" ) == 0 ){
        return 1200;
    }else if( sname.compare( "utf-16" ) == 0 || sname.compare( "utf16" ) == 0 ){
        return 1204;
    }else if( sname.compare( "utf-32le" ) == 0 || sname.compare( "utf32le" ) == 0 ){
        return 1234;
    }else if( sname.compare( "utf-32be" ) == 0 || sname.compare( "utf32be" ) == 0 ){
        return 1232;
    }else if( sname.compare( "utf-32" ) == 0 || sname.compare( "utf32" ) == 0 ){
        return 1236;
    }else{
        return 0;
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
 * 指定したコードページを使って、読込み専用のテキストファイルを開く。
 * MB_FILEから1行ずつ読み込むのは多重定義されたmb_fgetsを使うが、
 * mb_fgets( wstring &line, MB_FILE *fp )のほうは指定したコードページで、
 * mb_fgets( string &line, MB_FILE *fp )のほうはコードページを無視して読み込む。
 *
 * @param file_name ファイルの名前
 * @param codepage コードページ
 * @returns ファイル記述子
 */
MB_FILE *mb_fopen( const char *file_name, const char *codepage ){
    mb_init();
    MB_FILE *fp = (MB_FILE *)malloc( sizeof( MB_FILE ) );
#ifdef __GNUC__
    fp->file = fopen( file_name, "rb" );
#else
    fopen_s( &(fp->file), file_name, "rb" );
#endif
    if( !(fp->file) ){
#ifdef _DEBUG
        cout << "::mb_fopen; fopen failed; file_name=" << file_name << endl;
#endif
        free( fp );
        return NULL;
    }
    fp->unit_len = mb_unitlen_from_charset_name( codepage );
    if( fp->unit_len < 1 ){
        fp->unit_len = 1;
    }
#ifdef _DEBUG
    cout << "::mb_fopen; fp->unit_len=" << fp->unit_len << endl;
#endif
#ifdef MB_USE_ICONV
    fp->descripter_for_wchar = iconv_open( mb_charset_wchar.c_str(), codepage );
    fp->descripter_for_char = iconv_open( mb_charset_char.c_str(), codepage );
#else // MB_USE_ICONV
    fp->descripter_for_wchar = MB_INVALID;
    if( strcmp( codepage, "UTF-8" ) == 0 || 
        strcmp( codepage, "UTF8" ) == 0 ){
        fp->descripter_for_wchar = CP_UTF8;
    }else if( strcmp( codepage, "UTF-7" ) == 0 || 
              strcmp( codepage, "UTF7" ) == 0 ){
        fp->descripter_for_wchar = CP_UTF7;
    }else if( strcmp( codepage, "SJIS" ) == 0 ||
              strcmp( codepage, "Shift_JIS" ) == 0 ){
        fp->descripter_for_wchar = 932;
    }else if( strcmp( codepage, "JIS" ) == 0 ||
              strcmp( codepage, "ISO-2022-JP" ) == 0 ){
        fp->descripter_for_wchar = 50220;
    }else if( strcmp( codepage, "UTF-16" ) == 0 ||
              strcmp( codepage, "UTF16" ) == 0 ){
        fp->descripter_for_wchar = 1200;
    }else{
        fp->descripter_for_wchar = CP_ACP;
    }
    if( !IsValidCodePage( fp->descripter_for_wchar ) ){
        fp->descripter_for_wchar = MB_INVALID;
    }
    fp->descripter_for_char = fp->descripter_for_wchar;
#endif // MB_USE_ICONV

    // BOMがある場合は読み飛ばす
    unsigned int codepage_number = mb_codepage_from_charset_name( codepage );
    if( codepage_number == 1208 || codepage_number == 1209 ){
        // UTF-8
        int c1 = fgetc( fp->file );
        int c2 = fgetc( fp->file );
        int c3 = fgetc( fp->file );
        if( c1 == 0xEF && c2 == 0xBB && c3 == 0xBF ){
            // BOM
        }else{
            // BOMではないようなので戻る
            fseek( fp->file, -3, SEEK_CUR );
        }
    }else if( 1200 <= codepage_number && codepage_number <= 1205 ){
        // UTF-16
        int c1 = fgetc( fp->file );
        int c2 = fgetc( fp->file );
        if( (c1 == 0xFF && c2 == 0xFE) ||
            (c1 == 0xFE && c2 == 0xFF) ){
            // BOM
        }else{
            // BOMではないようなので戻る
            fseek( fp->file, -2, SEEK_CUR );
        }
    }else if( 1232 <= codepage_number && codepage_number <= 1237 ){
        // UTF-32
        int c1 = fgetc( fp->file );
        int c2 = fgetc( fp->file );
        int c3 = fgetc( fp->file );
        int c4 = fgetc( fp->file );
        if( (c1 == 0x00 && c2 == 0x00 && c3 == 0xFE && c4 == 0xFF) ||
            (c1 == 0xFF && c2 == 0xFE && c3 == 0x00 && c4 == 0x00) ){
            // BOM
        }else{
            // BOMではないようなので戻る
            fseek( fp->file, -4, SEEK_CUR );
        }
    }
    return fp;
}

MB_FILE *mb_fopen( string file_name, const char *codepage ){
    return mb_fopen( file_name.c_str(), codepage );
}

MB_FILE *mb_fopen( wstring file_name, const char *codepage ){
    mb_init();
    string s;
    mb_conv( file_name, s );
#ifdef _DEBUG
    cout << "::mb_fopen(wstring,const char*); s=" << s << endl;
#endif
    return mb_fopen( s.c_str(), codepage );
}

/**
 * コードページ記述子を閉じる
 */
int mb_fclose( MB_FILE *fp ){
    if( !fp ){
        return 0;
    }

    // コードページ記述子を閉じる
#ifdef MB_USE_ICONV
    if( fp->descripter_for_wchar != MB_INVALID ){
        iconv_close( fp->descripter_for_wchar );
    }
    if( fp->descripter_for_char != MB_INVALID ){
        iconv_close( fp->descripter_for_char );
    }
#else
    // do nothing
#endif

    // ファイルを閉じる
    int ret = 0;
    if( fp->file ){
        ret = fclose( fp->file );
    }
    free( fp );
    return ret;
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

/**
 * マルチバイト文字列が、指定したコードの制御文字を表すかどうかを調べます
 * @param buf 調査対象のマルチバイト文字列のバッファ
 * @param len バッファbufの長さ
 * @param check_char チェックする制御文字コード
 * @return バッファの文字列が指定した制御文字を表すならtrue、そうでなければfalse
 */
bool mb_code_check( char *buf, int len, char check_char ){
    if( len < 1 ){
        return false;
    }
    int i;

    // LEを仮定
    if( buf[0] == check_char ){
        for( i = 1; i < len; i++ ){
            if( buf[i] != 0x00 ){
                return false;
            }
        }
        return true;
    }

    // BEを仮定
    if( buf[len - 1] == check_char ){
        for( i = 0; i < len - 1; i++ ){
            if( buf[i] != 0x00 ){
                return false;
            }
        }
        return true;
    }

    return false;
}

/**
 * マルチバイト文字列が、復帰制御文字(キャリッジリターン, CR)を表すかどうかを調べます
 * @param buf 調査対象のマルチバイト文字列のバッファ
 * @param len バッファbufの長さ
 * @param check_char チェックする制御文字コード
 * @return バッファの文字列がCRを表すならtrue、そうでなければfalse
 */
bool mb_is_cr( char *buf, int len ){
    return mb_code_check( buf, len, 0x0D );
}

/**
 * マルチバイト文字列が、改行制御文字(ラインフィード, LF)を表すかどうかを調べます
 * @param buf 調査対象のマルチバイト文字列のバッファ
 * @param len バッファbufの長さ
 * @param check_char チェックする制御文字コード
 * @return バッファの文字列がLFを表すならtrue、そうでなければfalse
 */
bool mb_is_lf( char *buf, int len ){
    return mb_code_check( buf, len, 0x0A );
}

/**
 * ファイルから指定されたバイト数を読み込みます。
 * 読込の途中でEOFになった場合true、それ以外はfalseを返します
 */
int mb_fread( char *buf, int len, FILE *fp ){
    int ret = 0;
#ifdef _DEBUG
    //cout << "::mb_fread; len=" << len << ";\"";
#endif
    memset( buf, 0, len * sizeof( char ) );
    for( int i = 0; i < len; i++ ){
        int c = fgetc( fp );
        if( c == EOF ){
            return ret;
        }
        buf[i] = (char)c;
        ret++;
#ifdef _DEBUG
        //cout << "c=" << c << ";";
        //char pch[2];
        //pch[0] = (char)c;
        //pch[1] = 0;
        //string s( pch );
        //cout << s;
#endif
    }
#ifdef _DEBUG
    //cout << "\"" << endl;
#endif
    return ret;
}

bool mb_fgets_core( char *buf, int buflen, MB_FILE *file ){
    FILE *fp = file->file;
    int unit_buflen = file->unit_len;
    int unit_bufbytes = sizeof( char ) * unit_buflen;
    char *unit_buf = (char *)malloc( unit_bufbytes );
    int i;
    int bufbytes = sizeof( char ) * buflen;
    memset( buf, 0, bufbytes );
    int offset = -unit_buflen;
    for( i = 0; i < buflen - 1; i++ ){
        // このループ中でbuf[offset]からbuf[offset+buflen]までを埋めます
        offset += unit_buflen;
        int j;

        // 1文字分読み込む
        int len = mb_fread( unit_buf, unit_buflen, fp );

        if( len != unit_buflen ){
            // EOFまで読んだ場合
            for( j = 0; j < unit_buflen; j++ ){
                buf[j + offset] = '\0';
            }
            if( i == 0 ){
                // 最初の文字でEOFの場合
                //free( buf );
                free( unit_buf );
                return false;
            }else{
                // それ以外は単にbreakするだけ
                break;
            }
        }else if( mb_is_cr( unit_buf, unit_buflen ) ){
            // 読んだのがCRだった場合
            // 次の文字がLFかどうかを調べる
            len = mb_fread( unit_buf, unit_buflen, fp );
            if( len == unit_buflen ){
                if( mb_is_lf( unit_buf, unit_buflen ) ){
                    // LFのようだ
                }else{
                    // LFでないので、ファイルポインタを戻す
                    fseek( fp, -unit_bufbytes, SEEK_CUR );
                }
            }
            break;
        }else if( mb_is_lf( unit_buf, unit_buflen ) ){
            // 読んだのがLFだった場合
            // 次の文字がCRかどうかを調べる
            len = mb_fread( unit_buf, unit_buflen, fp );
            if( len == unit_buflen ){
                if( mb_is_cr( unit_buf, unit_buflen ) ){
                    // CRのようだ
                    // LF-CRという改行方法があるかどうかは知らないけれどサポートしとこう
                }else{
                    // CRでないので、ファイルポインタを戻す
                    fseek( fp, -unit_bufbytes, SEEK_CUR );
                }
            }
            break;
        }else{
            // 通常の処理
            for( j = 0; j < unit_buflen; j++ ){
                buf[offset + j] = unit_buf[j];
            }
        }
    }

    free( unit_buf );

    return true;
}

/**
 * ファイルから1行読み込む．
 * ファイルはdescipterで指定したコードページとみなして読み込む．
 * 改行はCR,CRLF,LFの3種類に対応．
 */
bool mb_fgets( wstring& line, MB_FILE *file ){
    int unit_buflen = file->unit_len;
    int unit_bufbytes = sizeof( char ) * unit_buflen;
    int buflen = LINEBUFF_LEN;
    int bufbytes = unit_bufbytes * buflen;
    char *buf = (char *)malloc( bufbytes );
    line.clear();

    bool ret = mb_fgets_core( buf, buflen, file );
    if( !ret ){
        free( buf );
        return ret;
    }

    // コードページの読み替え
    if( file->descripter_for_wchar != MB_INVALID ){
        mb_code_conv( buf, bufbytes, file->descripter_for_wchar );
        ret = true;
    }

    wstring ws = (wchar_t *)buf;
    line += ws;
    free( buf );

    return ret;
}

/**
 * ファイルから1行読み込む。
 */
bool mb_fgets( string& line, MB_FILE *file ){
    int unit_buflen = file->unit_len;
    int unit_bufbytes = sizeof( char ) * unit_buflen;
    int buflen = LINEBUFF_LEN;
    int bufbytes = unit_bufbytes * buflen;
    char *buf = (char *)malloc( bufbytes );
    line.clear();

    bool ret = mb_fgets_core( buf, buflen, file );
    if( !ret ){
        free( buf );
        return ret;
    }

    // コードページの読み替え
    if( file->descripter_for_char != MB_INVALID ){
        mb_code_conv( buf, bufbytes, file->descripter_for_char );
        ret = true;
    }

    string s = (char *)buf;
    line += s;
    free( buf );

    return ret;
}

#ifdef _DEBUG
/**
 * いろんなエンコーディングのテキストファイルを読み込むテストを実行します．
 */
#include <iostream>
#if defined( WIN32 )
    #include <windows.h>
#endif
using namespace std;
int mb_test(){
    //do nothing
    return 0;

    const char *cp = "UTF-16LE"; // ここを，in.txtのエンコーディングに変える（コンパイル時に手動で）
    MB_FILE *fp = mb_fopen( "in.txt", cp );
    if( !fp ){
        printf( "error; io-error; in.txt\n" );
        return 0;
    }
    FILE *fout_ws = fopen( "out_ws.txt", "wb" );
    fprintf( fout_ws, "%c%c", 0xff, 0xfe );
    wstring buf_ws = L"";
    while( mb_fgets( buf_ws, fp ) ){
        int i;
        // UNICODE =>
        for( i = 0; i < buf_ws.length(); i++ ){
            wchar_t c = buf_ws[i];
            int hi = 0xff & (c >> 8);
            int lo = 0xff & c;
            fputc( lo, fout_ws );
            fputc( hi, fout_ws );
        }
        fprintf( fout_ws, "%c%c%c%c", 0x0d, 0x00, 0x0a, 0x00 );
    }
    mb_fclose( fp );
    fclose( fout_ws );

    FILE *fout_mbs = fopen( "out_mbs.txt", "wb" );
    fp = mb_fopen( "in.txt", cp );
    string buf_mbs = "";
    while( mb_fgets( buf_mbs, fp ) ){
        fprintf( fout_mbs, "%s\n", buf_mbs.c_str() );
    }
    mb_fclose( fp );
    fclose( fout_mbs );
    int d;
    printf( "type eny key to continue..." );
#ifdef __GNUC__
    scanf( "%d", &d );
#else
    scanf_s( "%d", &d );
#endif
    return 0;
}
#endif

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
