/*
 * EncodingConverter.h
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
#ifndef __EncodingConverter_h__
#define __EncodingConverter_h__

#include <errno.h>
#include <iconv.h>
#include <string>

using namespace std;

namespace vconnect
{
    /**
     * マルチバイト文字列のテキストエンコーディングを変換するためのコンバータ
     */
    class EncodingConverter
    {
    private:
        /**
         * システム内部のマルチバイト文字列が使用しているテキストエンコーディング名
         */
        string internalEncoding;

        /**
         * libiconv のコンバータ
         */
        iconv_t converter;

    public:
        /**
         * 変換元と変換先のエンコーディングを指定し、コンバータを初期化する
         * @param from 変換前のエンコーディング名
         * @param to 変換後のエンコーディング名
         */
        EncodingConverter( string from, string to )
        {
            this->converter = iconv_open( to.c_str(), from.c_str() );
            if( false == isValidConverter( this->converter ) ){
                this->converter = NULL;
            }
            this->internalEncoding = getInternalEncoding();
        }

        ~EncodingConverter()
        {
            if( this->converter && isValidConverter( this->converter ) ){
                iconv_close( this->converter );
            }
            this->converter = NULL;
        }

        /**
         * 文字列のテキストエンコーディングを変換する
         * @param source 変換する文字列
         * @return エンコーディングを変換した文字列
         */
        string convert( string source )
        {
            if( NULL == this->converter ){
                return source;
            }

            string result;
            char *input = const_cast<char *>( source.c_str() );
            size_t remainingInputBytes = source.size();

            char *buffer = new char[remainingInputBytes + 1];
            char *output = buffer;
            size_t remainingOutputBytes = remainingInputBytes;
            size_t outputBytes = remainingInputBytes;

            while( remainingInputBytes > 0 ){
                char *originalInput = input;
                size_t n = iconv( this->converter, &input, &remainingInputBytes, &output, &remainingOutputBytes );
                int error = errno;
                if( (n != (size_t) - 1 && remainingInputBytes == 0) || (error == EINVAL) ){
                    remainingInputBytes = 0;
                    result.append( buffer, 0, outputBytes - remainingOutputBytes );
                }else{
                    switch( error ){
                        case E2BIG:{
                            result.append( buffer, 0, outputBytes - remainingOutputBytes );
                            output = buffer;
                            remainingOutputBytes = outputBytes;
                            break;
                        }
                        case EILSEQ:{
                            result.append( buffer, 0, outputBytes - remainingOutputBytes );
                            result.append( input, 0, 1 );
                            input++;
                            remainingInputBytes--;
                            output = buffer;
                            remainingOutputBytes = outputBytes;
                            break;
                        }
                        default:{
                            result.append( originalInput );
                            remainingInputBytes = 0;
                            break;
                        }
                    }
                }
            }
            output = buffer;
            remainingOutputBytes = outputBytes;
            if( iconv( this->converter , NULL, NULL, &output, &remainingOutputBytes ) != (size_t) - 1 ){
                result.append( buffer, 0, outputBytes - remainingOutputBytes );
            }
            delete [] buffer;
            return result;
        }

        /**
         * char の内部のエンコーディングを調べる
         * @return
         */
        static string getInternalEncoding()
        {
            string result = "";
            char *localeNameRaw = setlocale( LC_CTYPE, "" );
            if( NULL != localeNameRaw ){
                // localeName = "ja_JP.UTF-8" (MacOSX Lion)
                // localeName = "ja_JP.UTF-8" (openSUSE, g++)
                // localeName = "ja_JP.UTF-8" (CentOS, g++)
                // localeName = "Japanese_Japan.932" (Windows XP, g++)
                // localeName = "Japanese_Japan.932" (Windows XP, VC++2008)
                string localeName = localeNameRaw;
                result = getCodeset( localeName );

                if( false == isValidEncoding( result ) && 0 != atoi( result.c_str() ) ){
                    // result が全部数字だったら、"CP" という文字列を付けてリトライする
                    result = "CP" + result;
                    if( false == isValidEncoding( result ) ){
                        result = "";
                    }
                }
            }
            return result;
        }

        /**
         * コードページの名称から、読込み時の読込単位(バイト)を調べます
         * @return テキストファイルからの読み込み単位
         */
        static int getBytesPerWord( string encoding )
        {
            encoding = toLower( encoding );
            if( encoding.compare( "utf-16le" ) == 0 ){
                return 2;
            }else if( encoding.compare( "utf-16be" ) == 0 ){
                return 2;
            }else if( encoding.compare( "utf-16" ) == 0 ){
                return 2;
            }else if( encoding.compare( "utf-32le" ) == 0 ){
                return 4;
            }else if( encoding.compare( "utf-32be" ) == 0 ){
                return 4;
            }else if( encoding.compare( "utf-32" ) == 0 ){
                return 4;
            }else{
                return 1;
            }
        }

        /**
         * 有効なエンコーディングかどうかを取得する
         * @param codeset エンコーディング名
         * @return 有効なエンコーディングであれば true を、そうでなければ false を返す
         */
        static bool isValidEncoding( string codeset )
        {
            // まずUTF-8が有効かどうか
            iconv_t cnv = iconv_open( "UTF-8", "UTF-8" );
            if( false == isValidConverter( cnv ) ){
                return false;
            }
            iconv_close( cnv );

            iconv_t cnv2 = iconv_open( "UTF-8", codeset.c_str() );
            if( false == isValidConverter( cnv2 ) ){
                return false;
            }
            iconv_close( cnv2 );

            iconv_t cnv3 = iconv_open( codeset.c_str(), "UTF-8" );
            if( false == isValidConverter( cnv3 ) ){
                return false;
            }
            iconv_close( cnv3 );
            return true;
        }

    protected:
        EncodingConverter()
        {
            this->converter = NULL;
        }

        /**
         * setlocale( LC_CTYPE, "" ) の戻り値から、コードセット名を取得する
         * @param locale setlocale 関数の戻り値
         * @return コードセット名。取得できない場合は空文字
         * @ref http://linuxjm.sourceforge.jp/html/LDP_man-pages/man3/setlocale.3.html
         */
        static string getCodeset( string locale )
        {
            string::size_type indexCollon = locale.find( "." );
            string::size_type indexAtmark = locale.find( "@" );
            if( string::npos == indexCollon ){
                return "";
            }
            if( string::npos == indexAtmark ){
                return locale.substr( indexCollon + 1 );
            }else{
                return locale.substr( indexCollon + 1, indexAtmark - indexCollon - 1 );
            }
        }

    private:
        /**
         * 小文字に変換する
         * @param text 変換元の文字列
         * @return 小文字に変換後の文字列
         */
        static string toLower( string text )
        {
            string::size_type length = text.length();
            string::size_type i;
            for( i = 0; i < length; i++ ){
                text[i] = tolower( text[i] );
            }
            return text;
        }

        /**
         * 有効なコンバータかどうかを調べる
         * @param converter 調べる対象のコンバータ
         * @return コンバータが有効であれば true を、そうでなければ false を返す
         */
        static bool isValidConverter( iconv_t converter )
        {
            iconv_t invalid = (iconv_t) - 1;
            return (converter == invalid) ? false : true;
        }
    };
}

#endif
