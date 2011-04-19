#include "stand.h"
#include "standFrame.h"

// んー．．．
#define NULL 0

standFrame::standFrame()
{
    noiseRatio = mixRatio = f0 = t = spectrum = aperiodicity = NULL;
    melCepstra = NULL;
    cepstrumLengths = NULL;
    cepstrumNumber = 0;
}

standFrame::~standFrame()
{
    delete[] melCepstra;
    delete[] mixRatio;
    delete[] noiseRatio;
    delete[] cepstrumLengths;
}

void standFrame::createCepstrum(int cepstrumNumber)
{
    if(cepstrumNumber > 0)
    {
        this->cepstrumNumber = cepstrumNumber;
        melCepstra = new standComplex*[cepstrumNumber];
        mixRatio = new double[cepstrumNumber];
        noiseRatio = new double[cepstrumNumber];
        cepstrumLengths = new int[cepstrumNumber];
        for(int i = 0; i < cepstrumNumber; i++)
        {
            mixRatio[i] = 0.0;
            noiseRatio[i] = 0.0;
            cepstrumLengths[i] = 0;
        }
    }
}
