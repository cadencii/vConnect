#include <stdio.h>
#include <string.h>

#include <string>
using namespace std;

#include "vConnectPhoneme.h"
#include "vConnectConverter.h"
#include "waveFileEx/waveFileEx.h"
#include "time.h"

const int bufSize = 4096;

bool vConnectConverter::convert(const char* otoIni, const char *dstDir)
{
    FILE *fp = fopen(otoIni, "r");
    FILE *out;
    if(fp == NULL) { return false; }

    char buf[bufSize];

    string srcDir = otoIni;
    string dstDir_s = dstDir;
    string fileName, alias;
    srcDir = srcDir.substr(0, srcDir.rfind("\\"));
    out = fopen((dstDir_s + "oto.ini").c_str(), "w");

    float leftBlank, fixedLength, rightBlank, preUtterance, voiceOverlap;

    int count = 0;

    while(fgets(buf, bufSize, fp) != NULL) {
        count++;
        double *wave;
        int waveLength;
        waveFileEx waveFile;
        vConnectPhoneme phoneme;

        string tmp = buf, line;
        fileName = tmp.substr(0, tmp.find("="));
        alias = tmp.substr(tmp.find("=") + 1, tmp.find(","));
        line = tmp.substr(tmp.find("="));
        tmp = tmp.substr(tmp.find(",")+1);

        leftBlank    = atof(tmp.c_str());
        tmp = tmp.substr(tmp.find(",")+1);

        fixedLength  = atof(tmp.c_str());
        tmp = tmp.substr(tmp.find(",")+1);

        rightBlank   = atof(tmp.c_str());
        tmp = tmp.substr(tmp.find(",")+1);

        preUtterance = atof(tmp.c_str());
        tmp = tmp.substr(tmp.find(",")+1);

        voiceOverlap = atof(tmp.c_str());

        sprintf(buf, "%d", count);
        tmp = buf;
        line = tmp + ".vvd" + line;

        fprintf(stderr,"====\n");
        fprintf(stderr, "Begin analysis : %s @ %s\n", alias.c_str(), fileName.c_str());

        if(waveFile.readWaveFile(srcDir + "\\" + fileName) != 1)
        {
            cout << "error ; can't open the file, " << (srcDir + "\\" + fileName).c_str() << endl;
            continue;
        }
        waveLength = waveFile.getWaveLength(leftBlank, rightBlank);
        wave = new double[waveLength];
        waveFile.getWaveBuffer(wave, leftBlank, rightBlank, waveLength);

        // 音量の正規化を行う．
        long index = (long)( 44100.0 * fixedLength / 1000.0 );
        double sum1 = 0.0, sum2 = 0.0;
        // 固定長終了位置の音量を得る．
        for( int i = index - 1024; 0 <= i && i < index + 1024 && i < waveLength; i++ ){
            sum1 += wave[i] * wave[i];
        }
        // 左ブランク終了位置の音量を得る．
        for( int i = 0; i < 2048 && i < waveLength; i++){
            sum2 += wave[i] * wave[i];
        }
        // 大きい方が正規化のための音量．
        sum1 = max(sum1, sum2);
        sum1 = VOL_NORMALIZE / sqrt( ( sum1 / 2048.0 ) );

        for(int i = 0; i < waveLength; i++)
        {
            wave[i] *= sum1;
        }

        fprintf(stderr, "  computing wave....\n");
        clock_t cl = clock();
        phoneme.computeWave(wave, waveLength, 44100, 2.0);
        fprintf(stderr, "  Done. Elapsed time = %d[ms]\n", clock() - cl);

        fileName = dstDir;
        fileName = fileName + buf + ".vvd";
        if(phoneme.writePhoneme(fileName.c_str())) {
            fprintf(stderr, "Wrote file : %s\n", fileName.c_str());
        } else {
            fprintf(stderr, "Error. Failed to write : %s\n", fileName.c_str());
        }
        fprintf(stderr, "====\n");

        fprintf(out, "%s", line.c_str());

        delete[] wave;
    }

    fclose(out);
    fclose(fp);

    return true;
}
