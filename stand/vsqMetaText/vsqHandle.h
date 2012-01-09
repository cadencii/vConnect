/**
 * vsqHandle.h
 * Copyright (C) 2010 HAL, kbinani
 *
 *  This files is a part of v.Connect.
 * vsqHandle class contains vsq-handle.
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#ifndef __vsqHandle_h__
#define __vsqHandle_h__

#include "vsqBase.h"
#include "vsqVibratoBP.h"
#include "vsqLyric.h"

class vsqHandle// : public vsqBase 
{
public:
    void    setParameter( string left, string right );

    string    getLyric( void ){ return lyric.lyric; }
    string    getPronounce( void ){ return lyric.pronounce; }
    string    getIDS( void ){ return IDS; }
    /**
     * ビブラート深さの値を取得しする
     * @param position 値を取得する位置．ビブラートの開始位置が0，終了位置が1となるような値を渡す
     * @return 指定した位置でのビブラート深さの値
     */
    short    getVibratoDepth( double position );
    /**
     * ビブラート速さの値を取得しする
     * @param position 値を取得する位置．ビブラートの開始位置が0，終了位置が1となるような値を渡す
     * @return 指定した位置でのビブラート速さの値
     */
    short    getVibratoRate( double position );

    string toString()
    {
        string ret = "";
        string str_icon_id;
        string str_ids;
        mb_conv( iconID, str_icon_id );
        mb_conv( IDS, str_ids );
        ret += "{iconID=" + str_icon_id + ",IDS=" + str_ids + ",lyric=" + lyric.toString() + "}";
        return ret;
    }


private:
    string    iconID;
    string    IDS;
    string    caption;
    short    original;
    short    length;
    short    language;
    short    program;

    /**
     * Vibrato Depthのカーブを保持する．
     * StartDepthがインデックス0に，BPX, BPYによる指定がインデックス1以降に記録される
     */
    vector<vsqVibratoBP> vibratoDepth;
    /**
     * Vibrato Rateのカーブを保持する．
     * StartRateがインデックス0に，BPX, BPYによる指定がインデックス1以降に記録される
     */
    vector<vsqVibratoBP> vibratoRate;
    vsqLyric    lyric;

    /**
     * DepthBPNum, RateBPNumの解釈を行う．setParameterから呼ばれる内部関数
     * @param list 解釈を行う対象のBPリスト．vibratoDepthまたはvibratoRate
     * @param str 解釈するソース文字列の，"="の右側．例："DepthBPNum=1"なら"1"を渡す
     * @return [void]
     */
    void parseBPNum( vector<vsqVibratoBP>& list, string str );

    /**
     * DepthBPX, DepthBPY, RateBPX, RateBPYの解釈を行う．setParameterから呼ばれる内部関数
     * @param list 解釈を行う対象のBPリスト．vibratoDepthまたはvibratoRate
     * @param str 解釈するソース文字列の，"="の右側．例："DepthBPX=0.1,1.0"なら"0.1,1.0"を渡す
     * @param parse_x *BPXを解釈する場合にtrue, *BPYを解釈する場合にfalse
     * @return [void]
     */
    void parseBP( vector<vsqVibratoBP>& list, string str, bool parse_x );
};

#endif
