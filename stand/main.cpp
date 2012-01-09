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

int main( int argc, char *argv[] )
{
    RuntimeOption option( argc, argv );

    if( option.isPrintCodeset() ){
        list<string> supportedCodesets = Configuration::supportedCodesets();
        list<string>::iterator i = supportedCodesets.begin();
        for( ; i != supportedCodesets.end(); i++ ){
            string codeset = *i;
            if( EncodingConverter::isValidEncoding( codeset ) ){
                cout << codeset << endl;
            }
        }
    }

    string input = option.getInputPath();
    string output = option.getOutputPath();
#ifdef __TEST__
    input = "test.txt";
    output = "log.wav";
#else

#ifdef _DEBUG
    cout << "::main; encodingOtoIni=" << option.getEncodingOtoIni() << endl;
    cout << "::main; encodingVsqText=" << option.getEncodingVsqText() << endl;
#endif

    if( (input == "" || output == "") && false == option.isPrintCodeset() ){
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

    if( option.isConvert() ){
        vConnectConverter converter;
        converter.convert( input.c_str(), output.c_str() );
    }else if( option.isTranscribe() ){
        const char *encoding = option.getEncodingOtoIni().c_str();
        vConnectTranscriber::transcribe( input, output, encoding );
    }else{
        vConnect vC;
        vC.synthesize( input, output, option );
    }

    return 0;
}
