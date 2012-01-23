#ifndef DIO_H
#define DIO_H

namespace stand
{
namespace math
{
namespace world
{
/// <summary> F0 estimation by DIO(Distributed Inline filter Operation) </summary>
/// <param name = "x"> Input signal. </input>
/// <param name = "xLen"> Length of Input [sample]. </param>
/// <param name = "f0"> F0 Estimated by DIO. </param>
/// <param name = "framePeriod"> Frame shift [msec] </param>
/// <param name = "timeAxis"> Time axis; e.g. f0[0] is the F0 estimated at timeAxis[0] sec. </param>
/// <param name = "fs"> Sampling frequency [Hz] </param>
void dio(const double *x, int xLen, int fs, double framePeriod, double *timeAxis, double *f0);

/// <summary> Necessary samples for DIO estimation. </summary>
/// <param name = "fs"> Sampling frequency </param>
/// <param name = "xLen"> Signal length [samples] </param>
/// <param name = "framePeriod"> Frame shift [msec] </param>
int samplesForDio(int fs, int xLen, double framePeriod);

const double DIO_FLOOR_F0 = 80;
const double DIO_CEIL_F0 = 640;
const double DIO_CHANNNELS = 2;
const double DIO_TARGET_FS = 4000;
}
}
}


#endif // DIO_H
