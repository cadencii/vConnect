#include "standData.h"

void standData::getMelCepstrum(int frame, standComplex *dst, int *length, double freq, double briRate)
{
    // 今のところ保持している MelCepstrum からデータをコピー
    if(this->enableExtention == false) return;

    int tmpLen, i;
    standComplex *tmp = this->melCepstrum.getMelCepstrum((double)frame * framePeriod, &tmpLen);
    for(i = 0; i < tmpLen && i < *length; i++){
        dst[i].re = tmp[i].re;
        dst[i].im = tmp[i].im;
    }
    if(*length > tmpLen)
        *length = tmpLen;
}

bool standData::readMelCepstrum(vConnectSetting &setting, string_t alias)
{
    bool ret = false;
    librarySetting *tmp;
    // とりあえず Brightness に関して読み込めたかどうか．
    tmp = setting.getLibrarySetting(SETTING_BRIGHTNESS);
    if(tmp && tmp->enabled){
        string_t path = tmp->path;
        this->enableExtention = ret = melCepstrum.readMelCepstrum(path + alias + ".stt");
        this->cepstrumLength = melCepstrum.getMelCepstrumLength();
    }

    return ret;
}
