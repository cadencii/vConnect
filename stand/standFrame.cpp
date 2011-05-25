/*
 * standFrame.cpp
 * Copyright © 2011 HAL, kbinani
 *
 * This file is part of vConnect-STAND.
 *
 * org.kbinani.cadencii is free software; you can redistribute it and/or
 * modify it under the terms of the GPL License.
 *
 * org.kbinani.cadencii is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#include "stand.h"
#include "standFrame.h"

#ifndef NULL
#define NULL (0)
#endif

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

void standFrame::createCepstrum( int num_cepstrum )
{
    if( num_cepstrum <= 0 ) return;

    this->cepstrumNumber = num_cepstrum;
    melCepstra           = new standComplex*[num_cepstrum];
    mixRatio             = new double[num_cepstrum];
    noiseRatio           = new double[num_cepstrum];
    cepstrumLengths      = new int[num_cepstrum];
    
    for( int i = 0; i < num_cepstrum; i++ )
    {
        mixRatio[i] = 0.0;
        noiseRatio[i] = 0.0;
        cepstrumLengths[i] = 0;
    }
}
