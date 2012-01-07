/*
 * TextReader.h
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
#ifndef __TextReader_h__
#define __TextReader_h__

#include <stdio.h>
#include "EncodingConverter.h"

namespace vconnect
{
    /**
     * テキストファイルを読み込むためのクラス
     * @todo 1 行が BUFFER_SIZE を超えるテキストを読む場合動作がデタラメ
     */
    class TextReader
    {
    private:
        /**
         * 1行の最大文字数
         */
        static const int BUFFER_SIZE = 1024;

        /**
         * テキストエンコーディングのコンバータ
         */
        EncodingConverter *converter;

        /**
         * ファイルハンドル
         */
        FILE *fileHandle;

        /**
         * 指定されたエンコーディングの読み込み単位バイト数
         * たとえば UTF8 なら 1 バイト、UTF16 なら 2 バイト単位で読み込む
         */
        int bytesPerWord;

        /**
         * bytesPerWord バイト分のバッファー
         */
        char *unitBuffer;

        /**
         * ファイルからデータを読み込むときに使うバッファー
         */
        char *buffer;

    public:
        /**
         * テキストエンコーディングを指定して、テキストファイルを開く
         * @param path 開くファイルのパス
         * @param encoding テキストエンコーディング
         */
        TextReader( string path, string encoding )
        {
            this->converter = new EncodingConverter( encoding, EncodingConverter::getInternalEncoding() );
            this->fileHandle = fopen( path.c_str(), "rb" );
            this->bytesPerWord = EncodingConverter::getBytesPerWord( encoding );
            this->unitBuffer = new char[this->bytesPerWord];

            int bufferBytes = this->bytesPerWord * BUFFER_SIZE;
            this->buffer = new char[bufferBytes];
        }

        /**
         * デストラクタ
         */
        ~TextReader()
        {
            this->close();
        }

        /**
         * テキストファイルから 1 行読み込む
         * @return 行データ
         */
        string readLine()
        {
            FILE *fp = this->fileHandle;
            int unit_buflen = this->bytesPerWord;
            int unit_bufbytes = sizeof( char ) * unit_buflen;
            int i;
            int bufbytes = this->bytesPerWord * BUFFER_SIZE;

            string result = "";
            bool isEndOfLine = false;
            while( false == isEndOfLine ){
                memset( this->buffer, 0, bufbytes );
                int offset = -unit_buflen;
                for( i = 0; i < bufbytes - 1; i++ ){
                    // このループ中でbuf[offset]からbuf[offset+buflen]までを埋めます
                    offset += unit_buflen;
                    int j;

                    // 1文字分読み込む
                    int len = fillUnitBuffer( this->unitBuffer, unit_buflen, fp );

                    if( len != unit_buflen ){
                        // EOFまで読んだ場合
                        for( j = 0; j < unit_buflen; j++ ){
                            this->buffer[j + offset] = '\0';
                        }
                        isEndOfLine = true;
                        break;
                    }else if( isCR( this->unitBuffer, unit_buflen ) ){
                        // 読んだのがCRだった場合
                        // 次の文字がLFかどうかを調べる
                        len = fillUnitBuffer( this->unitBuffer, unit_buflen, fp );
                        if( len == unit_buflen ){
                            if( isLF( this->unitBuffer, unit_buflen ) ){
                                // LFのようだ
                            }else{
                                // LFでないので、ファイルポインタを戻す
                                fseek( fp, -unit_bufbytes, SEEK_CUR );
                            }
                        }
                        isEndOfLine = true;
                        break;
                    }else if( isLF( this->unitBuffer, unit_buflen ) ){
                        // 読んだのがLFだった場合
                        // 次の文字がCRかどうかを調べる
                        len = fillUnitBuffer( this->unitBuffer, unit_buflen, fp );
                        if( len == unit_buflen ){
                            if( isCR( this->unitBuffer, unit_buflen ) ){
                                // CRのようだ
                                // LF-CRという改行方法があるかどうかは知らないけれどサポートしとこう
                            }else{
                                // CRでないので、ファイルポインタを戻す
                                fseek( fp, -unit_bufbytes, SEEK_CUR );
                            }
                        }
                        isEndOfLine = true;
                        break;
                    }else{
                        // 通常の処理
                        for( j = 0; j < unit_buflen; j++ ){
                            this->buffer[offset + j] = this->unitBuffer[j];
                        }
                    }
                }

                string source = this->buffer;
                result += this->converter->convert( source );
            }

            return result;
        }

        /**
         * ファイル読み込みがファイル末尾に達したかどうか
         * @return ファイル末尾に達している場合 true を、そうでなければ false を返す。
         */
        bool isEOF()
        {
            return feof( this->fileHandle ) ? true : false;
        }

        /**
         * ファイルを閉じる
         */
        void close()
        {
            if( this->fileHandle ){
                fclose( this->fileHandle );
            }
            if( this->converter ){
                delete this->converter;
            }
            if( this->unitBuffer ){
                delete [] this->unitBuffer;
            }
            if( this->buffer ){
                delete [] this->buffer;
            }
            this->fileHandle = NULL;
            this->converter = NULL;
            this->unitBuffer = NULL;
            this->buffer = NULL;
        }

    private:
        TextReader()
        {
        }

        /**
         * マルチバイトの一文字分のデータをファイルから読み込む
         * @param buffer 読み込んだデータの格納先
         * @param length buffer のバイト数
         * @param fileHandle 読み込み対象のファイル
         * @return 読み込んだデータのバイト数
         */
        static int fillUnitBuffer( char *buffer, int length, FILE *fileHandle )
        {
            int ret = 0;
            memset( buffer, 0, length * sizeof( char ) );
            for( int i = 0; i < length; i++ ){
                int c = fgetc( fileHandle );
                if( c == EOF ){
                    return ret;
                }
                buffer[i] = (char)c;
                ret++;
            }
            return ret;
        }

        /**
         * 指定したマルチバイトの一文字が、指定した制御文字を表すかどうかを調べる
         * @param buffer マルチバイトの一文字
         * @param length buffer のバイト数
         * @param expected 期待値
         * @return 一致したら ture を、そうでなければ false を返す
         */
        static bool isExpectedCode( char *buffer, int length, char expected )
        {
            if( length < 1 ){
                return false;
            }
            int i;

            // LEを仮定
            if( buffer[0] == expected ){
                for( i = 1; i < length; i++ ){
                    if( buffer[i] != 0x00 ){
                        return false;
                    }
                }
                return true;
            }

            // BEを仮定
            if( buffer[length - 1] == expected ){
                for( i = 0; i < length - 1; i++ ){
                    if( buffer[i] != 0x00 ){
                        return false;
                    }
                }
                return true;
            }

            return false;
        }

        /**
         * マルチバイト文字列が、改行制御文字(キャリッジリターン, CR)を表すかどうかを調べます
         * @param buf 調査対象のマルチバイト文字列のバッファ
         * @param len バッファbufの長さ
         * @param check_char チェックする制御文字コード
         * @return バッファの文字列がLFを表すならtrue、そうでなければfalse
         */
        static bool isCR( char *buffer, int length )
        {
            return isExpectedCode( buffer, length, 0x0D );
        }

        /**
         * マルチバイト文字列が、改行制御文字(ラインフィード, LF)を表すかどうかを調べます
         * @param buf 調査対象のマルチバイト文字列のバッファ
         * @param len バッファbufの長さ
         * @param check_char チェックする制御文字コード
         * @return バッファの文字列がLFを表すならtrue、そうでなければfalse
         */
        static bool isLF( char *buffer, int length )
        {
            return isExpectedCode( buffer, length, 0x0A );
        }
    };
}

#endif

