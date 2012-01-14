/*
 * Sequence.cpp
 * Copyright (C) 2009-2011 HAL,
 * Copyright (C) 2011-2012 kbinani.
 */
#include "Sequence.h"
#include "../utauVoiceDB/UtauDBManager.h"

namespace vconnect
{
    void Sequence::setParamOtoIni( string singerName, string otoIniPath, string encoding )
    {
        // 名前登録して
        int index = UtauDBManager::find( otoIniPath );
        if( index < 0 ){
            // 中身読んで
            UtauDB *p = new UtauDB( otoIniPath, encoding );
            // リストに追加
            UtauDBManager::regist( p );
            index = UtauDBManager::find( otoIniPath );
        }
        singerMap.insert( std::make_pair( singerName, index ) );
    }

    void Sequence::setParamEvent( Event *target, string left, string right )
    {
        if( left.compare( "Type" ) == 0 ){
            target->type = right;
        }else if( left.compare( "Length" ) == 0 ){
            target->length = atoi( right.c_str() );
        }else if( left.compare( "Note#" ) == 0 ){
            target->note = atoi( right.c_str() );
        }else if( left.compare( "Dynamics" ) == 0 ){
            target->velocity = atoi( right.c_str() );
        }else if( left.compare( "PMBendDepth" ) == 0 ){
            target->portamentoDepth = atoi( right.c_str() );
        }else if( left.compare( "PMBendLength" ) == 0 ){
            target->portamentoLength = atoi( right.c_str() );
        }else if( left.compare( "PMbPortamentoUse" ) == 0 ){
            target->portamentoUse = atoi( right.c_str() );
        }else if( left.compare( "DEMdecGainRate" ) == 0 ){
            target->decay = atoi( right.c_str() );
        }else if( left.compare( "DEMaccent" ) == 0 ){
            target->attack = atoi( right.c_str() );
        }else if( left.compare( "VibratoDelay" ) == 0 ){
            target->vibratoDelay = atoi( right.c_str() );
        }else if( left.compare( "PreUtterance" ) == 0 ){
            target->utauSetting.msPreUtterance = (float)atof( right.c_str() );
        }else if( left.compare( "VoiceOverlap" ) == 0 ){
            target->utauSetting.msVoiceOverlap = (float)atof( right.c_str() );
        }else if( left.compare( "LyricHandle" ) == 0 ){
            right = "[" + right + "]";
            mMapHandles.insert( make_pair( right, &target->lyricHandle ) );
        }else if( left.compare( "VibratoHandle" ) == 0 ){
            right = "[" + right + "]";
            mMapHandles.insert( make_pair( right, &target->vibratoHandle ) );
        }else if( left.compare( "IconHandle" ) == 0 ){
            right = "[" + right + "]";
            mMapHandles.insert( make_pair( right, &target->iconHandle ) );
        }else{
            string message = "warning: unknown Property in VsqEvent : " + left;
            cout << message << endl;//outputError( message.c_str() );
        }

        return;
    }

    long Sequence::getEndTick()
    {
        return this->events.endTick;
    }

    double Sequence::getTempo()
    {
        return this->vsqTempoBp.getTempo();
    }

    Sequence::Sequence()
    {
        string temp;

        this->controlCurves.resize( CONTROL_CURVE_NUM );

        for( vector<BPList>::size_type i = 0; i < this->controlCurves.size(); i++ ){
            this->controlCurves[i].setParameter( -10000, controlCurveDefaultValue[i] );
            this->mMapCurves.insert( make_pair( controlCurveName[i], &this->controlCurves[i] ) );
        }
    }

    bool Sequence::read( string file_name, RuntimeOption option )
    {
        bool result = false;
        TextInputStream *stream = new TextInputStream( file_name, option.getEncodingVsqText() );
        bool ret = this->readCore( stream, file_name, option.getEncodingOtoIni() );
        delete stream;
        return ret;
    }

    bool Sequence::readCore( InputStream *stream, string vsqFilePath, string encodingOtoIni )
    {
        if( !stream ) return false;

        string temp, search, left, right;
        //Map<string, vsqBase *>::iterator i;

        while( stream->ready() ){
            temp = stream->readLine();

            // 空文字の場合は次へ
            if( temp.size() <= 0 ) continue;

            if( temp.find( "[" ) == 0 ){
                search = temp;
                continue;
            }
            string::size_type indx_equal = temp.find( "=" );
            if( indx_equal == string::npos ){
                left = temp;
                right = "";
            }else{
                left = temp.substr( 0, indx_equal );
                right = temp.substr( indx_equal + 1 );
            }

            if( search.compare( OBJ_NAME_EVENT_LIST ) == 0 ){
                // [EventList]
                string::size_type indx_comma = right.find( "," );
                while( indx_comma != string::npos ){
                    // コンマが見つからなくなるまでループ
                    string tright = right.substr( 0, indx_comma );
                    this->events.setParameter( left, tright, mMapIDs );
                    right = right.substr( indx_comma + 1 );
                    indx_comma = right.find( "," );
                }
                this->events.setParameter( left, right, mMapIDs );
            }else if( search.compare( OBJ_NAME_OTOINI ) == 0 ){
                // [oto.ini]
                string::size_type index = left.find( "\t" );
                string singerName, otoIniPath;

                if( index == string::npos ){
                    singerName = "";
                    otoIniPath = left;
                }else{
                    singerName = left.substr( 0, index );
                    otoIniPath = left.substr( index + 1 );
                }
                if( false == Path::exists( otoIniPath ) ){
                    string directory = Path::getDirectoryName( vsqFilePath );
                    otoIniPath = Path::combine( directory, otoIniPath );
                }
                this->setParamOtoIni( singerName, otoIniPath, encodingOtoIni );
            }else if( search.compare( OBJ_NAME_TEMPO ) == 0 ){
                // [Tempo]
                this->vsqTempoBp.setParameter( left, right );
            }else if( search.find( "[ID#" ) == 0 ){
                // ID
                Map<string, Event *>::iterator i;
                i = mMapIDs.find( search );
                if( i != mMapIDs.end() && i->second ){
                    setParamEvent( i->second, left, right );
                }
            }else if( search.find( "[h#" ) == 0 ){
                // handle
                Map<string, Handle *>::iterator i;
                i = mMapHandles.find( search );
                if( i != mMapHandles.end() && i->second ){
                    i->second->setParameter( left, right );
                }
            }else{
                // たぶんコントロールカーブ
                Map<string, BPList *>::iterator i;
                i = mMapCurves.find( search );
                if( i != mMapCurves.end() && i->second ){
                    i->second->setParameter( left, right );
                }else{
                    // そんなセクション名知らねー
                    string message;
                    message = "vsqFileEx::readCore; not found: " + search;
                    cout << message << endl;
                }
            }
        }

        // utau音源が無ければ合成しようがないので false.
        int size = UtauDBManager::size();

    #if defined( _DEBUG )
        dumpEvents();
        dumpMapIDs();
        dumpMapHandles();
        cout << "vsqFileEx::readCore; size=" << size << endl;
    #endif

        return (size > 0);
    }

    double Sequence::getEndSec()
    {
         return this->vsqTempoBp.tickToSecond( getEndTick() );
    }

    int Sequence::getSingerIndex( string t_search )
    {
        int ret = 0;
        Map<string, int>::iterator i = singerMap.find( t_search );
        if( i != singerMap.end() )
        {
            ret = i->second;
        }
        return ret;
    }
}
