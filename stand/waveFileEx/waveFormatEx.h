/**
 * waveFormatEx.h
 * Copyright (C) 2010 HAL, kbinani
 *
 * This file is part of v.Connect.
 *
 * v.Connect is free software; you can redistribute it and/or
 * modify it under the terms of the GNU GPL Lisence.
 *
 * v.Connect is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifndef __waveFormatEx_h__
#define __waveFormatEx_h__

struct waveFormatEx {
    short             chunkID;
    long              chunkSize;
    short             formatTag;
    unsigned    short numChannels;
    unsigned    long  samplePerSecond;
    unsigned    long  bytesPerSecond;
    unsigned    short blockAlign;
    unsigned    short bitsPerSample;
};

#endif // __waveFormatEx_h__
