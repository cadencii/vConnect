#include "MFCCSet.h"

#include "MathSet.h"

using namespace stand::math;

MFCCSet::MFCCSet(unsigned int fftl)
{
    _ffft = new FFTSet(fftl, FFTSet::FFTW_C2R);
    _ifft = new FFTSet(fftl, FFTSet::FFTW_R2C);
}

MFCCSet::~MFCCSet()
{
    delete _ffft;
    delete _ifft;
}

fftw_complex *MFCCSet::compute(double *spectrum, int fs)
{
    double *logSpectrum = (double *)_ifft->in();
    unsigned int i;
    stretchToMelScale(logSpectrum, spectrum, length() / 2 + 1, fs / 2);
    for(i = 0; i <= length() / 2; i++)
    {
        logSpectrum[i] = log(logSpectrum[i] + 1.0e-55); // safe guard.
    }
    for(; i < length(); i++)
    {
        logSpectrum[i] = logSpectrum[length() - i];
    }

    _ifft->execute();
    return (fftw_complex *)_ifft->out();
}

double *MFCCSet::extract(double *melCepstrum, unsigned int l, int fs)
{
    fftw_complex *melCepstrum_c = (fftw_complex *)_ffft->in();
    double *result = (double *)_ifft->in();
    unsigned int i;
    for(i = 0; i < l && i < length() / 2; i++)
    {
        melCepstrum_c[i][0] = melCepstrum[i];
        melCepstrum_c[i][1] = 0.0;
    }
    for(; i < length() / 2 + 1; i++)
    {
        melCepstrum_c[i][0] = melCepstrum_c[i][1] = 0.0;
    }

    _ffft->execute();
    stretchFromMelScale(result, (double *)_ffft->out(), length() / 2 + 1, fs / 2);
    for(int i = 0; i <= length() / 2; i++)
    {
        result[i] = exp(result[i]);
    }
    return result;
}

// コードは一緒…うーん．
double *MFCCSet::extract(float *melCepstrum, unsigned int l, int fs)
{
    fftw_complex *melCepstrum_c = (fftw_complex *)_ffft->in();
    double *result = (double *)_ifft->in();
    unsigned int i;
    for(i = 0; i < l && i < length() / 2; i++)
    {
        melCepstrum_c[i][0] = melCepstrum[i];
        melCepstrum_c[i][1] = 0.0;
    }
    for(; i < length() / 2 + 1; i++)
    {
        melCepstrum_c[i][0] = melCepstrum_c[i][1] = 0.0;
    }

    _ffft->execute();
    stretchFromMelScale(result, (double *)_ffft->out(), length() / 2 + 1, fs / 2);
    for(int i = 0; i <= length() / 2; i++)
    {
        result[i] = exp(result[i]);
    }
    return result;
}

void MFCCSet::stretchToMelScale(double *melSpectrum, const double *spectrum, int spectrumLength, int maxFrequency)
{
    double tmp = getMelScale(maxFrequency);
    for(int i = 0; i < spectrumLength; i++)
    {
        double dIndex = getFrequency((double)i / (double)spectrumLength * tmp) / (double)maxFrequency * (double)spectrumLength;
        double val;
        if(dIndex <= spectrumLength-1.0){
            val = interpolateArray(dIndex, spectrum);
        }else{
            val = spectrum[spectrumLength - 1];
        }
        melSpectrum[i] = val;
    }
}

void MFCCSet::stretchFromMelScale(double *spectrum, const double *melSpectrum, int spectrumLength, int maxFrequency)
{
    double tmp = getMelScale(maxFrequency);
    for(int i = 0; i < spectrumLength; i++)
    {
        double dIndex = getMelScale((double)i / (double)spectrumLength * (double)maxFrequency) / tmp * (double)spectrumLength;
        double val;
        if(dIndex <= spectrumLength-1.0){
            val = interpolateArray(dIndex, melSpectrum);
        }else{
            val = melSpectrum[spectrumLength - 1];
        }
        spectrum[i] = val;
    }
}

double  MFCCSet::getMelScale(double freq){
    double ret = 1127.01048 * log(1 + freq / 700);
    return ret;
}

double  MFCCSet::getFrequency(double melScale){
    double ret = 700 * (exp(melScale / 1127.01048) - 1);
    return ret;
}
