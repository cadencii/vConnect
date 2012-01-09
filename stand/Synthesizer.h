/*
 * Synthesizer.h
 * Copyright © 2010-2012 HAL, 2012 kbinani
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
 * vConnect class is a main class that connects UTAU and WORLD.
 * It is consisted of spectral, pitch and dynamics calculation.
 */
#ifndef __Synthesizer_h__
#define __Synthesizer_h__

#include "corpusManager.h"
#include "vsqMetaText/vsqFileEx.h"
#include "Task.h"
#include "RuntimeOption.h"

#define NOTE_NUM 128
#define VIB_NUM 128
#define NOISE_LEN 8192

class vConnectPhoneme;

namespace vconnect
{
    /// <summary>
    /// vConnect-STANDの合成器です．
    /// </summary>
    class Synthesizer : public Task
    {
    public: // public method
        Synthesizer( RuntimeOption option );

        ~Synthesizer();

        /// <summary>
        /// 合成処理を行います．
        /// </summary>
        void run();

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
}
#endif
