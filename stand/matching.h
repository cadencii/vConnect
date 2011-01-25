/*
 *
 *    matching.h
 *                              (c) HAL 2010-           
 *
 *   void calculateMatcing( double* T, double* H,       
 *               double* src, double* dst, int length );
 *
 *    Calculate matching function between src & dst;    
 *  where T is the destination for writing result,      
 *  H is difference between src & 'stretched' dst.      
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#ifndef __matching_h__
#define __matching_h__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#define MAX_MAT_LENGTH 2048

/* 許容する傾きの最大値 */
#define GRAD 2.0
/* 変形関数の傾きに対する重み */
#define MAGIC_K 1.5

double  interpolateArray( double x, const double* p );

void    calculateMatching( double* T, double* H, double* src_s, double* dst_s, int length );

void    applyStretching( double *T, double* target, int length );

double dp_matching(int *path, double *src, int length_s, double *dst, int length_d, int width);

#endif
