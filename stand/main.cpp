/**
 * main.cpp
 * Copyright (C) 2010 HAL, kbinani
 *
 * This file is a part of v.Connect-STAND.
 *
 * v.Connect-STAND is free software; you can redistribute it and/or
 * modify it under the terms of the GNU GPL License
 *
 * v.Connect-STAND is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *----------------------------------------------------------------------
 * Mainly, these codes are about File I/O.
 *
 */
#include "vConnect.h"
#include <locale>
//#define __TEST__

#define VCONNECT_VERSION "v.Connect-STAND ver2.0.0"

int main( int argc, char *argv[] ){
#ifdef _DEBUG
    //setlocale( LC_CTYPE, "japanese" );
    /*string ws = "あいうえおかきくけこ ";
    void *v = (void *)ws.c_str();
    FILE *f = fopen( "ws.txt", "wb" );
    fwrite( v, sizeof( string::value_type ), ws.size(), f );
    fclose( f );*/
    mb_test();
#endif
    string input = "";
    string output = "";
    string alias = "";
    vConnect vC;
    runtimeOptions options;
    bool print_list_charset = false; // サポートするキャラクタセットのリストをプリントアウトするかどうか

    // 引数を読み取る
    string current_parse = "";  // いま読んでるオプション(-i, -o等)
    for( int i = 1; i < argc; i++ ){
        string s = argv[i];
        if( s.substr( 0, 1 ) == "-" ){
            // "-"で始まるので
            if( s == "-s" ){
                options.fast = false;
                current_parse = "";
            }else if( s == "-list-charset" ){
                print_list_charset = true;
                current_parse = "";
            }else if( s == "-nf0" ){
                options.f0Transform = false;
                current_parse = "";
            }else if( s == "-nvn" ){
                options.volumeNormalization = false;
                current_parse = "";
            }else if( s == "-w" ){
                options.wspMode = true;
                current_parse = "";
            }else{
                // 次の引数で、オプションsの項目を設定するよ
                current_parse = s;
            }
        }else{
            // "-"以外で始まる
            if( current_parse == "-i" ){
                input = s;
            }else if( current_parse == "-o" ){
                output = s;
            }else if( current_parse == "-charset-otoini" ){
                options.encodingOtoIni = s;
            }else if( current_parse == "-charset-vxt" ){
                options.encodingVsqText = s;
            }else if( current_parse == "-l" ){
                alias = s;
            }else{
                cout << "unknown argument: " << s << endl;
            }
            current_parse = "";
        }
    }

    if( print_list_charset ){
        int num_charset = sizeof( CHARSET_SUPPORT ) / sizeof( unsigned int );
        int i;
        for( i = 0; i < num_charset; i++ ){
            if( mb_is_valid_codepage( CHARSET_SUPPORT[i] ) ){
                cout << mb_charset_name_from_codepage( CHARSET_SUPPORT[i] ) << endl;
            }
        }
    }
    
#ifdef __TEST__
    input = "test.txt";
    output = "log.wav";
#else

    // 引数の読み取りでin,outファイル指定が存在しなかった場合
    if( input == "" || output == "" ){
        // 引数の個数がちょうど3だったらば、引数の順番依存とみなして。
        if( argc == 3 ){
            input = argv[1];
            output = argv[2];
        }
    }

#ifdef _DEBUG
    cout << "::main; options.fast=" << (options.fast ? "true" : "false") << endl;
    cout << "::main; options.encodingOtoIni=" << options.encodingOtoIni << endl;
    cout << "::main; options.encodingVsqText=" << options.encodingVsqText << endl;
#endif

    if( ( ( options.wspMode && alias == "" ) || input == "" || output == "" ) && !print_list_charset ){
        // ファイルの指定のどちらかが欠けている場合
        // あるいはwsp 生成オプションが立っているがエイリアス指定の無い場合
        // 説明文を表示してbailout
        cout << VCONNECT_VERSION << endl;
        cout << "usage:" << endl;
        cout << "    vConnect -i [vsq_meta_text_path] -o [out_wave_path] {options}" << endl;
        cout << "    vConnect [vsq_meta_text_path] [out_wave_path]" << endl;
        cout << "      or" << endl;
        cout << "    vConnect -w -i [utau_oto_ini_path] -l [alias_to_analyze] -o [out_wsp_path] {options}" << endl;
        cout << "options:" << endl;
        cout << "    -i [path]                       path of input-file" << endl;
        cout << "    -o [path]                       path of output-file" << endl;
        cout << "    -s                              slow synthesize mode" << endl;
        cout << "    -nf0                            no spectral transform by f0" << endl;
        cout << "    -nvn                            no volume normalization" << endl;
        cout << "    -w                              wsp output mode" << endl;
        cout << "    -l [alias]                      alias of oto.ini with -w option" << endl;
        cout << "    -charset-otoini [charset-name]  charset of oto.ini (default: Shift_JIS)" << endl;
        cout << "    -charset-vxt [charset-name]     charset of input-file (default: Shift_JIS)" << endl;
        cout << "    -list-charset                   print supported charset list" << endl;
        return 0;
    }
#endif

    if( options.wspMode == false ){
        string_t tinput, toutput;
        mb_conv( input, tinput );
        mb_conv( output, toutput );
#ifdef _DEBUG
        string mbs_input, mbs_output;
        mb_conv( tinput, mbs_input );
        mb_conv( toutput, mbs_output );
        cout << "::main; input=" << input << "; mbs_input=" << mbs_input << endl;
        cout << "::main; output=" << output << "; mbs_output=" << mbs_output << endl;
#endif
        vC.synthesize( tinput, toutput, options );
    }else{
        string_t tinput, toutput, talias;
        mb_conv( input, tinput );
        mb_conv( output, toutput );
        mb_conv( alias, talias );
        vC.createWspFile( tinput, toutput, talias, options );
    }

    return 0;
}
