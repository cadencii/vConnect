/*
 * UtauParameter.h
 * Copyright © 2009-2012 HAL, 2012 kbinani
 *
 * This file is part of vConnect-STAND.
 *
 * vConnect-STAND is free software; you can redistribute it and/or
 * modify it under the terms of the GPL License.
 *
 * vConnect-STAND is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * ---------------------------------------------------------------------
 * class UtauParameter contains the data of utau Voice Settings.
 */
#ifndef __UtauParameter_h__
#define __UtauParameter_h__

#include <string>
#include "../Path.h"

namespace vconnect
{
    using namespace std;

    /**
     * oto.ini の1行分のデータを保持するクラス
     */
    class UtauParameter
    {
    public:
        string lyric;
        string fileName;
        float msLeftBlank;
        float msFixedLength;
        float msRightBlank;
        float msPreUtterance;
        float msVoiceOverlap;
        bool isWave;

    public:
        UtauParameter()
        {
            this->lyric = "";
            this->fileName = "";
            this->msLeftBlank = 0.0f;
            this->msFixedLength = 0.0f;
            this->msRightBlank = 0.0f;
            this->msPreUtterance = 0.0f;
            this->msVoiceOverlap = 0.0f;
            this->isWave = false;
        }

        /**
         * oto.ini の 1 行分のデータからのコンストラクタ
         * @param line 1 行分のデータ
         */
        UtauParameter( string line )
        {
            string::size_type index;
            if( (index = line.find( ".wav" )) == string::npos ){
                if( (index = line.find( ".stf" )) == string::npos ){
                    index = line.find( "=" );
                }
                this->isWave = false;
            }else{
                this->isWave = true;
            }

            this->fileName = line.substr( 0, index );
            this->fileName = Path::normalize( this->fileName );

            /* When no lyric symbol exists, voiceDB will use fileName instead. */
            if( line.find( "=" ) + 1 == line.find( "," ) ){
                this->lyric = this->fileName;
            }else{
                this->lyric = line.substr( line.find( "=" ) + 1 );
                this->lyric = this->lyric.substr( 0, this->lyric.find( "," ) );
            }

            line = line.substr( line.find( "," ) + 1 );
            this->msLeftBlank = (float)atof( line.c_str() );

            line = line.substr( line.find( "," ) + 1 );
            this->msFixedLength = (float)atof( line.c_str() );

            line = line.substr( line.find( "," ) + 1 );
            this->msRightBlank = (float)atof( line.c_str() );

            line = line.substr( line.find( "," ) + 1 );
            this->msPreUtterance = (float)atof( line.c_str() );

            line = line.substr( line.find( "," ) + 1 );
            this->msVoiceOverlap = (float)atof( line.c_str() );
        }
    };
}
#endif
