/*
 *
 * 仕様変更につきしばらく放置．Qt版を使ってください．
 *
 *
 */

#include "phoneticTranscriber.h"

#include <fstream>
#include "waveFileEx/waveFileEx.h"
#include "matching.h"
#include "standMelCepstrum.h"
#include "world/world.h"

double phoneticTranscriber::han[2049];

int phoneticTranscriber::calculateVolumeLength(int waveLen)
{
    int ret = 1 + (double)waveLen / (double)fs / (double)framePeriod * 1000.0;
    return ret;
}

void phoneticTranscriber::calculateVolume(double *dst, double *wave, int waveLen, int tLen)
{
    for(int i = 0; i < tLen; i++){
        int index = i * fs * framePeriod / 1000 - 1024;
        int c = 0;
        dst[i] = 0.0;
        for(int j = 0; j < 2049; j++, index++){
            if(index < 0)
                continue;
            if(index >= waveLen)
                break;
            dst[i] += wave[index] * han[j] * wave[index] * han[j];
            c++;
        }
        if(c){
            dst[i] = sqrt(dst[i]/(double)c);
        }else{
            dst[i] = sqrt(dst[i]);
        }
    }
}

phoneticTranscriber::phoneticTranscriber()
{
    for(int i = 0; i < 2049; i++){
        han[i] = 0.5 + 0.5 * cos(ST_PI * (double)(i-1024) / 1024.0);
    }
}

void phoneticTranscriber::phoneticTranscribe( UtauDB &target, string outputPath, string suffix )
{
    list<utauParameters*>::iterator i;
    utauParameters targetParameter;
    string targetVoicePath;

    string srcFileName, dstFileName;
    waveFileEx srcWave, dstWave;
    double *srcWaveBuf, *dstWaveBuf;
    int srcWaveLen, dstWaveLen;
    double *srcVolume, *dstVolume, *trans;
    int srcVolumeLen, dstVolumeLen;

    int transIndex;

    target.getDBPath( targetVoicePath );

    ofstream ofs("log.txt");
    ofs << "==============\n====begin transcription.\nsrcPath; " << mDBPath.c_str() << "\ndstPath; " << targetVoicePath.c_str() << endl;
    for(i = mSettingList.begin(); i != mSettingList.end(); i++){
        ofs << "----" << endl;
        ofs << "begin; alias\"" << (*i)->lyric << "\"" << endl;

        string suffixLyric = (*i)->lyric + suffix;
        if( target.getParams( targetParameter, suffixLyric ) ){
            srcFileName = mDBPath + (*i)->fileName + ".wav";
            dstFileName = targetVoicePath + targetParameter.fileName + ".wav";

            double *dstF0, **dstSpecgram, *dstT;
            int dstLen, fftl;
            if(srcWave.readWaveFile(srcFileName) != 1){
                ofs << "error; couldn't open the file" << endl;
                ofs << "     ; " << srcFileName << endl;
                continue;
            }
            if(dstWave.readWaveFile(dstFileName) != 1){
                ofs << "error; couldn't open the file" << endl;
                ofs << "     ; " << dstFileName << endl;
                continue;
            }
            targetParameter.msLeftBlank -= (*i)->msPreUtterance - targetParameter.msPreUtterance;
            srcWaveLen = srcWave.getWaveLength((*i)->msLeftBlank, (*i)->msRightBlank);
            //dstWaveLen = dstWave.getWaveLength(targetParameter.msLeftBlank, targetParameter.msRightBlank);
            dstWaveLen = srcWaveLen;
            targetParameter.msRightBlank = -((double)dstWaveLen / (double)fs * 1000.0);
            targetParameter.msFixedLength += (*i)->msPreUtterance - targetParameter.msPreUtterance;
            srcWaveBuf = new double[srcWaveLen];
            dstWaveBuf = new double[dstWaveLen];
            srcWave.getWaveBuffer(srcWaveBuf, (*i)->msLeftBlank, (*i)->msRightBlank, srcWaveLen);
            dstWave.getWaveBuffer(dstWaveBuf, targetParameter.msLeftBlank, targetParameter.msRightBlank, dstWaveLen);

            dstLen = getSamplesForDIO(fs, dstWaveLen, framePeriod);
            dstF0 = new double[dstLen];
            dstT = new double[dstLen];
            memset(dstF0, 0, sizeof(double)*dstLen);
            fftl = getFFTLengthForStar(fs);
            dstSpecgram = new double*[dstLen];
            for(int j = 0; j < dstLen; j++){
                dstSpecgram[j] = new double[fftl];
                memset(dstSpecgram[j], 0, sizeof(double));
            }

            /* 音量正規化 */
            double v1, v2;
            int index;
            v1 = v2 = 0.0;
            index = targetParameter.msFixedLength * (double)fs / 1000.0;
            for(int j = 0; j < 2048 && j < dstWaveLen; j++){
                v1 += dstWaveBuf[j] * dstWaveBuf[j];
            }
            for(int j = index - 1024; 0 <= j && j < index + 1024 && j < dstWaveLen; j++){
                v2 += dstWaveBuf[j] * dstWaveBuf[j];
            }
            v1 = max(v1, v2);
            if(v1 == 0) cout << "hit" << endl;
            v1 = 0.06 / sqrt(v1 / (double)fftl);
            for(int j = 0; j < dstWaveLen; j++){
                dstWaveBuf[j] *= v1;
            }

            v1 = v2 = 0.0;
            index = (*i)->msFixedLength * (double)fs / 1000.0;
            for(int j = 0; j < 2048; j++){
                v1 += srcWaveBuf[j] * srcWaveBuf[j];
            }
            for(int j = index - 1024; 0 <= j && j < index + 1024 && j < srcWaveLen; j++){
                v2 += srcWaveBuf[j] * srcWaveBuf[j];
            }
            v1 = max(v1, v2);
            if(v1 == 0) cout << "hit" << endl;
            v1 = 0.06 / sqrt(v1 / (double)fftl);
            for(int j = 0; j < srcWaveLen; j++){
                srcWaveBuf[j] *= v1;
            }

            srcVolumeLen = calculateVolumeLength(srcWaveLen);
            dstVolumeLen = calculateVolumeLength(dstWaveLen);
            srcVolume = new double[max(srcVolumeLen,dstVolumeLen)];
            dstVolume = new double[max(srcVolumeLen,dstVolumeLen)];
            calculateVolume(srcVolume, srcWaveBuf, srcWaveLen, max(srcVolumeLen,dstVolumeLen));
            calculateVolume(dstVolume, dstWaveBuf, dstWaveLen, max(srcVolumeLen,dstVolumeLen));
            trans = new double[max(srcVolumeLen,dstVolumeLen)];

            ofs << "src volume length = " << srcVolumeLen << endl;
            ofs << "dst volume length = " << dstVolumeLen << endl;
            if(abs(srcVolumeLen - dstVolumeLen) > 100){
                ofs << "warning; result may be incorrect because the difference of two length is out of range." << endl;
            }

            // 長さが異なる場合は右詰め．ただし値は端の値を使いまわす．
            int offset = 0;
            if(dstVolumeLen > srcVolumeLen){
                offset = dstVolumeLen - srcVolumeLen;
                for(int j = srcVolumeLen - 1; j >= offset; j--){
                    srcVolume[j] = srcVolume[j-offset];
                }
                for(int j = 0; j < offset; j++){
                    srcVolume[j] = srcVolume[offset];
                }
            }
            if(dstVolumeLen < srcVolumeLen){
                offset = dstVolumeLen - srcVolumeLen;
                for(int j = srcVolumeLen - 1; j >= -offset; j--){
                    dstVolume[j] = dstVolume[j+offset];
                }
                for(int j = 0; j < -offset; j++){
                    dstVolume[j] = dstVolume[-offset];
                }
            }

            // 転写をここで行う．
            cout << "begin matching ;" << (*i)->lyric << endl;
            calculateMatching(trans, NULL, srcVolume, dstVolume, max(srcVolumeLen, dstVolumeLen));
            cout << "done" << endl;


            dio(dstWaveBuf, dstWaveLen, fs, framePeriod, dstT, dstF0);
            star(dstWaveBuf, dstWaveLen, fs, dstT, dstF0, dstSpecgram, false); // mode == false で強制分析．
            double **targetSpecgram = new double*[srcVolumeLen];
            double *targetF0 = new double[srcVolumeLen];
            for(int j = 0; j < srcVolumeLen; j++){
                int tIndex = j;
                if(offset > 0){
                    tIndex += offset;
                }
                targetSpecgram[j] = new double[fftl];
                int index = trans[tIndex];
                if(offset < 0){
                    index += offset;
                }
                if(index >= dstLen)
                    index = dstLen - 1;
                if(index < 0)
                    index = 0;
                for(int k = 0; k <= fftl / 2; k++){
                    targetSpecgram[j][k] = dstSpecgram[index][k];
                }
                targetF0[j] = dstF0[index];
            }
            standMelCepstrum melCepstrum;
//            melCepstrum.calculateMelCepstrum(24, targetF0, targetSpecgram, srcVolumeLen, fftl, fs);            
            melCepstrum.writeMelCepstrum(outputPath + (*i)->lyric + ".stt");
            ofs << "done." << endl;

            for(int j = 0; j < srcVolumeLen; j++)
                delete[] targetSpecgram[j];
            delete[] targetSpecgram;
            for(int j = 0; j < dstLen; j++){
                delete[] dstSpecgram[j];
            }
            delete[] targetF0;
            delete[] dstSpecgram;
            delete[] dstT;
            delete[] dstF0;
            delete[] trans;
            delete[] srcVolume;
            delete[] dstVolume;
            delete[] srcWaveBuf;
            delete[] dstWaveBuf;
        }else{
            ofs << "error; utau parameters not found." << endl;
            ofs << "     ; alias \"" << (*i)->lyric << "\" does not exist in dst database." << endl;
        }
    }
}

