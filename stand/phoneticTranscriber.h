#ifndef __phoneticTranscriber_h__
#define __phoneticTranscriber_h__

#include "utauVoiceDB/UtauDB.h"

class phoneticTranscriber : public UtauDB
{
public:
    phoneticTranscriber();
    // 自身の原音設定を相手の原音設定に転写する．
    void phoneticTranscribe(UtauDB &target, string outputPath, string suffix);
private:
    static int calculateVolumeLength(int waveLen);
    static void phoneticTranscriber::calculateVolume(double *dst, double *wave, int waveLen, int tLen);
    static double han[2049];
};

#endif
