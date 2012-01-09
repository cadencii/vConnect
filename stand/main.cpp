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
#include "vConnectConverter.h"
#include "vConnectTranscriber.h"
#include "EncodingConverter.h"
#include "Configuration.h"
#include <locale>
//#define __TEST__

#define VCONNECT_VERSION "v.Connect-STAND ver2.0.0"

using namespace vconnect;

int main( int argc, char *argv[] ){
    string input = "";
    string output = "";
    vConnect vC;
    runtimeOptions options;
    bool print_list_charset = false; // サポートするキャラクタセットのリストをプリントアウトするかどうか

    // 引数を読み取る
    string current_parse = "";  // いま読んでるオプション(-i, -o等)
    for( int i = 1; i < argc; i++ ){
        string s = argv[i];
        if( s.substr( 0, 1 ) == "-" ){
            // "-"で始まるので
            if( s == "-c" ){
                options.convert = true;
                options.transcribe = false;
                current_parse = "";
            }else if( s == "-t" ){
                options.transcribe = true;
                options.convert = false;
                current_parse = "";
            }else if( s == "-list-charset" ){
                print_list_charset = true;
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
            }else{
                cout << "unknown argument: " << s << endl;
            }
            current_parse = "";
        }
    }

    if( print_list_charset ){
        list<string> supportedCodesets = Configuration::supportedCodesets();
        list<string>::iterator i = supportedCodesets.begin();
        for( ;i != supportedCodesets.end(); i++ ){
            string codeset = *i;
            if( EncodingConverter::isValidEncoding( codeset ) ){
                cout << codeset << endl;
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
    cout << "::main; options.encodingOtoIni=" << options.encodingOtoIni << endl;
    cout << "::main; options.encodingVsqText=" << options.encodingVsqText << endl;
#endif

    if( ( input == "" || output == "" ) && !print_list_charset ){
        // ファイルの指定のどちらかが欠けている場合
        // 説明文を表示してbailout
        cout << VCONNECT_VERSION << endl;
        cout << "usage:" << endl;
        cout << "    vConnect -i [vsq_meta_text_path] -o [out_wave_path] {options}" << endl;
        cout << "    vConnect [vsq_meta_text_path] [out_wave_path]" << endl;
        cout << "      or" << endl;
        cout << "    vConnect -c -i [utau_oto_ini_path] -o [out_stand_path]" << endl;
        cout << "    vConnect -t -i [src_vConnect_ini_path] -o [dst_vConnect_ini_path]" << endl;
        cout << "options:" << endl;
        cout << "    -i [path]                       path of input-file" << endl;
        cout << "    -o [path]                       path of output-file" << endl;
        cout << "    -c                              convert mode" << endl;
        cout << "    -t                              transcribe mode" << endl;
        cout << "    -charset-otoini [charset-name]  charset of oto.ini (default: Shift_JIS)" << endl;
        cout << "    -charset-vxt [charset-name]     charset of input-file (default: Shift_JIS)" << endl;
        cout << "    -list-charset                   print supported charset list" << endl;
        return 0;
    }
#endif

#ifdef _DEBUG
    cout << "::main; input=" << input << endl;
    cout << "::main; output=" << output << endl;
#endif
    if( options.convert == false ){
        if( options.transcribe ){
            const char *encoding = options.encodingOtoIni.c_str();
            vConnectTranscriber::transcribe( input, output, encoding );
        } else {
            vC.synthesize( input, output, options );
        }
    } else {
        vConnectConverter converter;
        converter.convert(input.c_str(), output.c_str());
    }

    return 0;
}
