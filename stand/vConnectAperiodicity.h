#ifndef __vConnectAperiodicity_h__
#define __vConnectAperiodicity_h__

#include <string>

using namespace std;

class standSpecgram;

// PLATINUM 0.0.4 用励起信号保持クラス．
class vConnectAperiodicity
{
public:
    vConnectAperiodicity();
    ~vConnectAperiodicity(){destroy();}

    bool writeAperiodicityWave(string output);
    bool readAperiodicityWave(string input);

    void getOneFrameSegment(double *dst, int index, int fftl);
    void computeExciteWave(standSpecgram *specgram);

private:
    void destroy(void);
    void createBuffer(int waveLength, int tLen);

    double *wave;
    int waveLength;

    int *apIndex;
    int *apLength;
    int tLen;
};

#endif
