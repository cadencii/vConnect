/*!
 * Stand Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU GPL License
 *
 * Stand Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  @file Dio.h
 *  @brief F0 estimation method DIO
 *         Original source codes are below.
 *         http://www.aspl.is.ritsumei.ac.jp/morise/world/
 *         This code is redistributed to support thread safety with Qt Library.
 *  @author HAL@shurabaP
 */
#ifndef DIO_H
#define DIO_H

namespace stand
{
namespace math
{
/*!
 *  @brief ナットール窓
 *  @param[in] yLen 信号長
 *  @param[out] y ナットール窓を適用する信号列
 */
void nuttallWindow(int yLen, double *y);

namespace world
{
const double DIO_FLOOR_F0 = 80;
const double DIO_CEIL_F0 = 640;
const double DIO_CHANNNELS = 2;
const double DIO_TARGET_FS = 4000;

/*! @brief F0 estimation by DIO(Distributed Inline filter Operation)
 *  @param[in] x Input signal.
 *  @param[in] xLen Length of Input [sample].
 *  @param[out] f0 F0 Estimated by DIO.
 *  @param[in] framePeriod Frame shift [msec]
 *  @param[out] timeAxis Time axis; e.g. f0[0] is the F0 estimated at timeAxis[0] sec.
 *  @param[in] fs Sampling frequency [Hz]
 *  @param[in] f0Ceil F0 high-limit
 *  @param[in] f0Floor F0 low-limit
 *  @param[in] channnelsInOctave
 *  @param[in] targetFs Sampling frequency that DIO uses.
 */
void dio(const double *x, int xLen, int fs, double framePeriod, double *timeAxis, double *f0,
         double f0Ceil = DIO_CEIL_F0, double f0Floor = DIO_FLOOR_F0, double channelsInOctave = DIO_CHANNNELS, double targetFs = DIO_TARGET_FS);

/*! @brief Necessary samples for DIO estimation.
 *  @param[in] fs Sampling frequency
 *  @param[in] xLen Signal length [samples]
 *  @param[in] framePeriod Frame shift [msec]
 */
int samplesForDio(int fs, int xLen, double framePeriod);
}
}
}


#endif // DIO_H
