/*
 * TextOutputStream.h
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
#ifndef __TextOutputStream_h__
#define __TextOutputStream_h__

#include "EncodingConverter.h"

using namespace std;

namespace vconnect
{

    /**
     * テキストファイルへの書き込みを行うためのクラス
     */
    class TextOutputStream
    {
    private:
        EncodingConverter *converter;
        FILE *fileHandle;
        string newLine;

    public:
        /**
         * テキストエンコーディング、ファイルパス、改行文字を指定してファイルを開く
         * @param path ファイルパス
         * @param encoding 書きこむテキストのエンコーディング
         * @param newLine 改行文字列
         */
        TextOutputStream( string path, string encoding, string newLine )
        {
            this->fileHandle = fopen( path.c_str(), "wb" );
            this->converter = new EncodingConverter( EncodingConverter::getInternalEncoding(), encoding );
            this->newLine = newLine;
        }

        /**
         * デストラクタ
         */
        ~TextOutputStream()
        {
            this->close();
        }

        /**
         * バッファをフラッシュしファイルを閉じる
         */
        void close()
        {
            if( this->fileHandle ){
                fflush( this->fileHandle );
                fclose( this->fileHandle );
            }
            if( this->converter ){
                delete this->converter;
            }
            this->fileHandle = NULL;
            this->converter = NULL;
        }

        /**
         * 新しい行を書き込む
         * @param line 書きこむ行データ
         */
        void writeLine( string line )
        {
            string converted;
            converted = this->converter->convert( line );
            converted += this->newLine;
            fprintf( this->fileHandle, "%s", converted.c_str() );
        }

    private:
        TextOutputStream()
        {
        };
    };

}

#endif
