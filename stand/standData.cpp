#include "standData.h"

bool standData::getBrightness(int frame, standComplex *dst, int *length, double freq, double briRate)
{
    // 今のところ保持している MelCepstrum からデータをコピー
    if(!this->brightnessSetting || !this->brightnessSetting->enabled) return false;

    int tmpLen, i;
    standComplex *tmp = this->brightness.getMelCepstrum((double)frame * framePeriod, &tmpLen);
    for(i = 0; i < tmpLen && i < *length; i++){
        dst[i].re = tmp[i].re;
        dst[i].im = tmp[i].im;
    }
    if(*length > tmpLen)
        *length = tmpLen;
    return true;
}

bool standData::getFreqInterp(int frame, standComplex *dst, int *length, double freq, double briRate, double *rate)
{
    int tmpLen, i;
    standComplex *tmp;
    double hi = 1000.0, low = -1000.0, base;

    if(baseSetting && baseSetting->enabled){
        base = baseSetting->frequency;
    }else{
        return false;
    }
    if(lowSetting && lowSetting->enabled){
        low = lowSetting->frequency;
    }
    if(hiSetting && hiSetting->enabled){
        hi = hiSetting->frequency;
    }

    if(base > freq){
        if(!lowSetting || !lowSetting->enabled) return false;
        tmp = this->low.getMelCepstrum((double)frame * framePeriod, &tmpLen);
        *rate = fabs(base - freq) / fabs(base - low);
    }else{
        if(!hiSetting || !hiSetting->enabled) return false;
        tmp = this->low.getMelCepstrum((double)frame * framePeriod, &tmpLen);
        *rate = fabs(freq - base) / fabs(hi - base);
    }
    if(*rate < 0.0) *rate = 0.0;
    if(*rate > 1.0) *rate = 1.0;
    for(i = 0; i < tmpLen && i < *length; i++){
        dst[i].re = tmp[i].re;
        dst[i].im = tmp[i].im;
    }
    if(*length > tmpLen)
        *length = tmpLen;
    return true;
}

bool standData::readMelCepstrum(vConnectSetting &setting, string_t alias)
{
    bool ret = false;

    baseSetting       = setting.getLibrarySetting(SETTING_BASE);
    brightnessSetting = setting.getLibrarySetting(SETTING_BRIGHTNESS);
    lowSetting        = setting.getLibrarySetting(SETTING_LOW);
    hiSetting         = setting.getLibrarySetting(SETTING_HI);

    if(brightnessSetting && lowSetting && hiSetting && brightnessSetting->enabled && lowSetting->enabled && hiSetting->enabled){
        string_t path;
        ret = false;

        path = brightnessSetting->path;
        ret |= brightness.readMelCepstrum(path + alias + ".stt");

        path = lowSetting->path;
        ret |= low.readMelCepstrum(path + alias + ".stt");

        path = hiSetting->path;
        ret |= hi.readMelCepstrum(path + alias + ".stt");

        this->enableExtention = ret;
    }

    return ret;
}
