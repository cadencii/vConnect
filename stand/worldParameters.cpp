#include "worldParameters.h"
#include "world/world.h"

worldParameters::worldParameters()
{
    t = f0 = NULL;
    pulseLocations = NULL;
    tLen = 0;
    framePeriod = 2.0;
}

worldParameters::~worldParameters()
{
    destroy();
}

void worldParameters::destroy()
{
    delete[] t;
    delete[] f0;
    delete[] pulseLocations;
    tLen = 0;
    framePeriod = 2.0;

    t = f0 = NULL;
    pulseLocations = NULL;
}

bool worldParameters::computeWave(double *wave, int waveLength, int fs, double framePeriod)
{
    if(!wave || waveLength < 0)
    {
        return false;
    }

    destroy();
    tLen = getSamplesForDIO(fs, waveLength, framePeriod);
    f0 = new float[tLen];
    t = new float[tLen];
    double *dF0 = new double[tLen];
    double *dT  = new double[tLen];
    pulseLocations = new int[tLen];
    this->framePeriod = framePeriod;

    // t, f0 は DIO だけで O.K.
    dio(wave, waveLength, fs, framePeriod, dT, dF0);
    for(int i = 0; i < tLen; i++)
    {
        f0[i] = dF0[i];
        t[i] = dT[i];
    }

    // PLATINUM 用にパルス位置を計算． platinum_v4 の機能限定版．
    int i, j, index;
    int fftl = (int)pow(2.0, 1.0+(int)(log(3.0*fs/FLOOR_F0+1) / log(2.0)));

    int vuvNum;
    vuvNum = 0;
    for(i = 1;i < tLen;i++)
    {
        if(f0[i]!=0.0 && f0[i-1]==0.0) vuvNum++;
    }
    vuvNum+=vuvNum-1; // 島数の調整 (有声島と無声島)
    if(f0[0] == 0) vuvNum++;
    if(f0[tLen-1] == 0) vuvNum++;

    int stCount, edCount;
    int *stList, *edList;
    stList = (int *)malloc(sizeof(int) * vuvNum);
    edList = (int *)malloc(sizeof(int) * vuvNum);
    edCount = 0;

    stList[0] = 0;
    stCount = 1;
    index = 1;
    if(f0[0] != 0)
    {
        for(i = 1;i < tLen;i++)
        {
            if(f0[i]==0 && f0[i-1]!=0)
            {
                edList[0] = i-1;
                edCount++;
                stList[1] = i;
                stCount++;
                index = i;
            }
        }
    }

    edList[vuvNum-1] = tLen-1;
    for(i = index;i < tLen;i++)
    {
        if(f0[i]!=0.0 && f0[i-1]==0.0) 
        {
            edList[edCount++] = i-1;
            stList[stCount++] = i;
        }
        if(f0[i]==0.0 && f0[i-1]!=0.0) 
        {
            edList[edCount++] = i-1;
            stList[stCount++] = i;
        }
    }

    int *wedgeList;
    wedgeList = (int *)malloc(sizeof(int) * vuvNum);
    getWedgeList(wave, waveLength, vuvNum, stList, edList, fs, framePeriod, dF0, wedgeList);

    double *signalTime, *f0interpolatedRaw, *totalPhase;
    double *fixedF0;
    fixedF0                = (double *)malloc(sizeof(double) * tLen);
    signalTime            = (double *)malloc(sizeof(double) * waveLength);
    f0interpolatedRaw    = (double *)malloc(sizeof(double) * waveLength);
    totalPhase            = (double *)malloc(sizeof(double) * waveLength);

    for(i = 0;i < tLen;i++) fixedF0[i] = f0[i] == 0 ? DEFAULT_F0 : f0[i];
    for(i = 0;i < waveLength;i++) signalTime[i] = (double)i / (double)fs;
    interp1(dT, fixedF0, tLen, signalTime, waveLength, f0interpolatedRaw);
    totalPhase[0] = f0interpolatedRaw[0]*2*PI/(double)fs;
    for(i = 1;i < waveLength;i++) totalPhase[i] = totalPhase[i-1] + f0interpolatedRaw[i]*2*PI/(double)fs;

    double *tmpPulseLocations;
    tmpPulseLocations        = (double *)malloc(sizeof(double) * waveLength);
    int pCount;
    pCount = getPulseLocations(wave, waveLength, totalPhase, vuvNum, stList, edList, fs, framePeriod, wedgeList, tmpPulseLocations);

    pulseLocations[0] = 0;
    for(i = 1;i < tLen;i++)
    {
        double currentF0 = f0[i] <= FLOOR_F0 ? DEFAULT_F0 : f0[i];
        int tmpIndex;

        double tmp;
        double tmpValue = 100000.0; // safeGuard
        for(i = 0;j < pCount; j++)
        {
            tmp = fabs(tmpPulseLocations[j] - (double)i * framePeriod);
            if(tmp < tmpValue)
            {
                tmpValue = tmp;
                tmpIndex = j;
            }
            index = 1 + (int)(0.5 + tmpPulseLocations[tmpIndex] * fs);
        }
        pulseLocations[i] = index;
    }

    free(fixedF0);
    free(pulseLocations);
    free(totalPhase);
    free(f0interpolatedRaw);
    free(signalTime);
    free(wedgeList);
    free(edList);
    free(stList);

    delete[] dT;
    delete[] dF0;

    return true;
}

bool worldParameters::writeParameters(const char *path)
{
    if(!t || !f0 || !pulseLocations || tLen <= 0)
    {
        return false;
    }
    FILE *fp = fopen(path, "wb");

    if(fp == NULL)
    {
        return false;
    }

    fwrite(&tLen, sizeof(int), 1, fp);
    fwrite(&framePeriod, sizeof(float), 1, fp);
    fwrite(t, sizeof(float), tLen, fp);
    fwrite(f0, sizeof(float), tLen, fp);
    fwrite(pulseLocations, sizeof(int), tLen, fp);

    fclose(fp);

    return true;
}

bool worldParameters::readParameters(const char *path)
{
    FILE *fp = fopen(path, "rb");
    if(fp == NULL)
    {
        return false;
    }

    int c;
    bool ret = false;
    destroy();
    c  = fread(&tLen, sizeof(int), 1, fp);
    c += fread(&framePeriod, sizeof(float), 1, fp);
    float *t = new float[tLen];
    float *f0 = new float[tLen];
    c += fread(t, sizeof(float), tLen, fp);
    c += fread(f0, sizeof(float), tLen, fp);
    c += fread(pulseLocations, sizeof(int), tLen, fp);

    if(c == 3 * tLen + 2)
    {
        ret = true;
    }
    else
    {
        tLen = 0;
    }

    fclose(fp);

    return ret;
}

bool worldParameters::getParameters(float *f0, float *t, int *pulseLocations, int fs, double beginTime, double endTime, double framePeriod)
{
    int beginIndex = beginTime / framePeriod * 1000.0;
    int endIndex = endTime / framePeriod * 1000.0;
    int index, i, tmp;

    // 開始時刻がマイナスの可能性があることに注意．
    for(i = 0, index = beginIndex; index < 0; i++, index++)
    {
        f0[i] = 0;
        t[i] = 0;
        pulseLocations[i] = 0;
    }
    tmp = index;
    // データをコピー．
    for(; index < endIndex; i++, index++)
    {
        int tmpIndex = (double)index * framePeriod / this->framePeriod;
        if(tmpIndex >= tLen)
        {
            break;
        }
        f0[i] = this->f0[tmpIndex];
        t[i] = this->t[tmpIndex] - this->t[tmp];
        pulseLocations[i] = this->pulseLocations[tmpIndex] - beginTime * fs;
    }

    // 指定された長さほどにデータが無い場合．
    for(; index < endIndex; i++, index++)
    {
        f0[i] = 0;
        t[i] = 0;
        pulseLocations[i] = 0;
    }

    return true;
}
