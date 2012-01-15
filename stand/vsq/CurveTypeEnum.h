/**
 * CurveTypeEnum.h
 * Copyright (C) 2012 kbinani
 *
 * This file is part of vConnect-STAND.
 *
 * vConnect-STAND is free software; you can redistribute it and/or
 * modify it under the terms of the GNU GPL Lisence.
 *
 * v.Connect is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifndef __CurveTypeEnum_h__
#define __CurveTypeEnum_h__

namespace vconnect
{
    /**
     * コントロールカーブの種類を表す
     */
    class CurveTypeEnum
    {
    public:
        enum CurveType
        {
            PITCH_BEND = 0,
            PITCH_BEND_SENS,
            DYNAMICS,
            BRETHINESS,
            BRIGHTNESS,
            CLEARNESS,
            GENDER,
        };

        /**
         * カーブの種類から、そのカーブのデフォルト値を取得する
         * @param curveType カーブの種類
         * @return デフォルト値
         */
        static int getDefault( CurveType curveType )
        {
            switch( curveType ){
                case PITCH_BEND:{
                    return 0;
                }
                case PITCH_BEND_SENS:{
                    return 2;
                }
                case DYNAMICS:{
                    return 64;
                }
                case BRETHINESS:{
                    return 0;
                }
                case BRIGHTNESS:{
                    return 64;
                }
                case CLEARNESS:{
                    return 0;
                }
                case GENDER:{
                    return 64;
                }
            }
        }

        /**
         * カーブの種類から、そのカーブの名前を取得する
         * @param curveType カーブの種類
         * @return 名前
         */
        static string getName( CurveType curveType )
        {
            switch( curveType ){
                case PITCH_BEND:{
                    return "[PitchBendBPList]";
                }
                case PITCH_BEND_SENS:{
                    return "[PitchBendSensBPList]";
                }
                case DYNAMICS:{
                    return "[DynamicsBPList]";
                }
                case BRETHINESS:{
                    return "[EpRResidualBPList]";
                }
                case BRIGHTNESS:{
                    return "[EpRESlopeBPList]";
                }
                case CLEARNESS:{
                    return "[EpRESlopeDepthBPList]";
                }
                case GENDER:{
                    return "[GenderFactorBPList]";
                }
            }
        }

        /**
         * カーブの種類の一覧を取得する
         * @return カーブ種類のリスト
         */
        static vector<CurveType> values()
        {
            vector<CurveType> result;
            result.push_back( PITCH_BEND );
            result.push_back( PITCH_BEND_SENS );
            result.push_back( DYNAMICS );
            result.push_back( BRETHINESS );
            result.push_back( BRIGHTNESS );
            result.push_back( CLEARNESS );
            result.push_back( GENDER );
            return result;
        }
    };
}
#endif
