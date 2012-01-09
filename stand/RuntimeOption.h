/**
 * runtimeOptions.h
 * Copyright (C) 2010 kbinani, HAL
 *
 *  This files is a part of v.Connect.
 * runtimeOptions contains data that is necessary in v.Connect.
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#ifndef __RuntimeOption_h__
#define __RuntimeOption_h__

#include <string>

namespace vconnect
{
    using namespace std;

    /**
     * vConnectでの合成時のオプション
     */
    class RuntimeOption{
    private:
        string encodingOtoIni;
        string encodingVsqText;
        string encodingVowelTable;
        string encodingVoiceTexture;
        string pathInput;
        string pathOutput;

        bool convert;
        bool transcribe;
        bool printCodesetList;

    public:
        RuntimeOption()
        {
            this->fillDefault();
        }

        RuntimeOption( int argc, char *argv[] )
        {
            this->fillDefault();

            // 引数を読み取る
            string current_parse = "";  // いま読んでるオプション(-i, -o等)
            for( int i = 1; i < argc; i++ ){
                string s = argv[i];
                if( s.substr( 0, 1 ) == "-" ){
                    // "-"で始まるので
                    if( s == "-c" ){
                        this->convert = true;
                        this->transcribe = false;
                        current_parse = "";
                    }else if( s == "-t" ){
                        this->transcribe = true;
                        this->convert = false;
                        current_parse = "";
                    }else if( s == "-list-charset" ){
                        this->printCodesetList = true;
                        current_parse = "";
                    }else{
                        // 次の引数で、オプションsの項目を設定するよ
                        current_parse = s;
                    }
                }else{
                    // "-"以外で始まる
                    if( current_parse == "-i" ){
                        this->pathInput = s;
                    }else if( current_parse == "-o" ){
                        this->pathOutput = s;
                    }else if( current_parse == "-charset-otoini" ){
                        this->encodingOtoIni = s;
                    }else if( current_parse == "-charset-vxt" ){
                        this->encodingVsqText = s;
                    }else{
                        cout << "unknown argument: " << s << endl;
                    }
                    current_parse = "";
                }
            }

            // 引数の読み取りでin,outファイル指定が存在しなかった場合
            if( this->pathInput == "" || this->pathOutput == "" ){
                // 引数の個数がちょうど3だったらば、引数の順番依存とみなして。
                if( argc == 3 ){
                    this->pathInput = argv[1];
                    this->pathOutput = argv[2];
                }
            }
        }

        /**
         * 音源の変換モードで起動されたかどうか
         */
        bool isConvert()
        {
            return this->convert;
        }

        /**
         * 多段化みたいな
         */
        bool isTranscribe()
        {
            return this->transcribe;
        }

        /**
         * oto.ini ファイルを読み込む際のテキストエンコーディングを取得する
         */
        string getEncodingOtoIni()
        {
            return this->encodingOtoIni;
        }

        /**
         * メタテキストファイルを読み込む際のテキストエンコーディング指定を取得する
         */
        string getEncodingVsqText()
        {
            return this->encodingVsqText;
        }

        /**
         * 入力ファイルまたはディレクトリのパスを取得する
         */
        string getInputPath()
        {
            return this->pathInput;
        }

        /**
         * 出力ファイルまたはディレクトリのパスを取得する
         */
        string getOutputPath()
        {
            return this->pathOutput;
        }

        /**
         * サポートするコードセットのリストを出力するかどうか
         */
        bool isPrintCodeset()
        {
            return this->printCodesetList;
        }

    private:
        void fillDefault()
        {
            this->convert = false;
            this->transcribe = false;
            this->printCodesetList = false;
            this->encodingOtoIni = "Shift_JIS";
            this->encodingVsqText = "Shift_JIS";
            this->encodingVowelTable = "Shift_JIS";
            this->encodingVoiceTexture = "Shift_JIS";
            this->pathInput = "";
            this->pathOutput = "";
        }

    };
}

#endif

