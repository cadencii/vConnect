#include "standData.h"

void standData::getMelCepstrum(int frame, standComplex *dst, int length, double freq, double briRate)
{
    // 今のところ保持している MelCepstrum からデータをコピー
    int tmpLen, i;
    standComplex *tmp = this->melCepstrum.getMelCepstrum((double)frame * framePeriod, &tmpLen);
    for(i = 0; i < tmpLen && i < length; i++){
        dst[i].re = tmp[i].re;
        dst[i].im = tmp[i].im;
    }
}

