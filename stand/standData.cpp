#include "standData.h"

int standData::getPosition(int frame, double freq, double briRate)
{
    double position = frame;
    double msFrame = frame * framePeriod;
    double hi = 1000.0, low = -1000.0, base;

    if(this->brightnessSetting && this->brightnessSetting->enabled)
    {
        position = position * (1.0 - briRate) + this->brightness.getStretchedPosition(msFrame) * briRate;
    }

    if(!baseSetting || baseSetting->enabled){
        return (int)position;
    }
    base = baseSetting->frequency;

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
        if(lowSetting && lowSetting->enabled){
            double ratio = fabs(base - freq) / fabs(base - low);
            position = position * (1.0 - ratio) + this->low.getStretchedPosition(msFrame) * ratio;
        }
    }else{
        if(lowSetting && lowSetting->enabled){
            double ratio = fabs(freq - base) / fabs(hi - base);
            position = position * (1.0 - ratio) + this->hi.getStretchedPosition(msFrame) * ratio;
        }
    }

    return (int)position;
}

bool standData::getBrightness(int frame, standComplex **dst, int *length, double freq, double *noise)
{
    // 今のところ保持している MelCepstrum からデータをコピー
    if(!this->brightnessSetting || !this->brightnessSetting->enabled) return false;

    int tmpLen;
    *dst = this->brightness.getMelCepstrum((double)frame * framePeriod, &tmpLen);

    *length = tmpLen;

    *noise    = brightness.getNoiseRatio(frame * framePeriod);
    return true;
}

bool standData::getFreqInterp(int frame, standComplex **dst, int *length, double freq, double *rate, double *noise)
{
    int tmpLen;
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
        *dst = this->low.getMelCepstrum((double)frame * framePeriod, &tmpLen);
        *rate = fabs(base - freq) / fabs(base - low);
        *noise    = this->low.getNoiseRatio(frame * framePeriod);
    }else{
        if(!hiSetting || !hiSetting->enabled) return false;
        *dst = this->hi.getMelCepstrum((double)frame * framePeriod, &tmpLen);
        *rate = fabs(freq - base) / fabs(hi - base);
        *noise    = this->hi.getNoiseRatio(frame * framePeriod);
    }
    if(*rate < 0.0) *rate = 0.0;
    if(*rate > 1.0) *rate = 1.0;

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
