/*
 *
 *    standFrame.h                                         
 *                              (c) HAL 2010-           
 *
 *  This file is a part of STAND Library.
 * standFrame is a structure that contains
 * STAR spectrum, PLATINUM aperiodicity and f0 pointers.
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#ifndef __standFrame_h__
#define __standFrame_h__

struct standFrame {
    double *f0;
    double *t;
    double *aperiodicity;
    double *spectrum;
    int fftl;
    int aperiodicityLength;
    bool isFast;
};

#endif // __standFrame_h__
