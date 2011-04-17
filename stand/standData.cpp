#include "standData.h"


bool standData::getBrightness(int frame, standComplex *dst, int *length, double freq, double *position, double *noise)
{
    // 今のところ保持している MelCepstrum からデータをコピー
    if(!this->brightnessSetting || !this->brightnessSetting->enabled) return false;

    int tmpLen, i;
    standComplex *tmp = this->brightness.getMelCepstrum((double)frame * framePeriod, &tmpLen);
    for(i = 0; i < tmpLen; i++){
        dst[i].re = tmp[i].re;
        dst[i].im = tmp[i].im;
    }
    *length = tmpLen;

    *position = brightness.getStretchedPosition(frame * framePeriod);
    *noise    = brightness.getNoiseRatio(frame * framePeriod);
    return true;
}

bool standData::getFreqInterp(int frame, standComplex *dst, int *length, double freq, double *rate, double *position, double *noise)
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

    base = log(base);
    low = log(low);
    hi = log(hi);
    freq = log(freq);

    if(base > freq){
        if(!lowSetting || !lowSetting->enabled) return false;
        tmp = this->low.getMelCepstrum((double)frame * framePeriod, &tmpLen);
        *rate = fabs(base - freq) / fabs(base - low);
        *position = this->low.getStretchedPosition(frame * framePeriod);
        *noise    = this->low.getNoiseRatio(frame * framePeriod);
    }else{
        if(!hiSetting || !hiSetting->enabled) return false;
        tmp = this->hi.getMelCepstrum((double)frame * framePeriod, &tmpLen);
        *rate = fabs(freq - base) / fabs(hi - base);
        *position = this->hi.getStretchedPosition(frame * framePeriod);
        *noise    = this->hi.getNoiseRatio(frame * framePeriod);
    }
    if(*rate < 0.0) *rate = 0.0;
    if(*rate > 1.0) *rate = 1.0;
    for(i = 0; i < tmpLen; i++){
        dst[i].re = tmp[i].re;
        dst[i].im = tmp[i].im;
    }
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

    string_t path;

    if(brightnessSetting && brightnessSetting->enabled){
        path = brightnessSetting->path;
        brightness.readMelCepstrum(path + alias + ".stt");
    }

    if(lowSetting && lowSetting->enabled){
        path = lowSetting->path;
        low.readMelCepstrum(path + alias + ".stt");
    }

    if(hiSetting && hiSetting->enabled){
        path = hiSetting->path;
        hi.readMelCepstrum(path + alias + ".stt");
    }

    this->enableExtention = ret = true;

    return ret;
}
