/*
 * vConnect.h
 * Copyright (C) 2010-2011 HAL, kbinani
 *
 *  This files is a part of v.Connect.
 * vConnect class is a main class that connects UTAU and WORLD.
 * It is consisted of spectral, pitch and dynamics calculation.
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#ifndef __vConnect_h__
#define __vConnect_h__

#include <limits.h>
#include <string.h>
#include "corpusManager.h"
#include "vsqMetaText/vsqFileEx.h"
#include "RuntimeOption.h"

#define NOTE_NUM 128
#define VIB_NUM 128
#define NOISE_LEN 8192

using namespace vconnect;

class vConnectPhoneme;

/// <summary>
/// vConnect-STANDの合成器です．
/// </summary>
class vConnect {
public: // public method
    vConnect();

    ~vConnect();

    /// <summary>
    /// 合成処理を行います．
    /// </summary>
    /// <param name="input">読み込むメタテキスト・ファイルのパス．</param>
    /// <param name="output">出力するWAVEファイルのパス．</param>
    /// <param name="options">合成時の設定．</param>
    /// <returns>合成に成功した場合true，それ以外はfalseを返します．</returns>
    bool synthesize(
        string input,
        string output,
        RuntimeOption options );

public: // public static method

    static corpusManager::phoneme *getPhoneme(
        string lyric,
        int singerIndex,
        vector<corpusManager *> *managers
        );
public: // public static field

    // BRE 用ノイズ源
    static double noiseWave[NOISE_LEN];

private: // private method
    /* 内部処理用関数 */
    void calculateVsqInfo();
    void calculateF0(
        double *f0,
        double *dynamics );

private: // private static method
    // vConnect内でしか使わない関数．
    static double getPitchFluctuation( double second );
    static void emptyPath( double secOffset, string output );

private: // private field
    vsqFileEx mVsq;
    corpusManager mManager;
    vector<corpusManager *> mManagerList;
    long mEndFrame;
    double mFluctTheta;
    vector<vector<standBP> > mControlCurves;

private: // private static field
    static double mNoteFrequency[NOTE_NUM];
    static double mVibrato[VIB_NUM];

};

#endif
