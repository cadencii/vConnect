#ifndef SMOOTMATCHING_H
#define SMOOTMATCHING_H

namespace stand
{
namespace math
{
void smoothMatching(double* dst_to_src, double* src_to_dst, double* src_s, double* dst_s, int length);
void applyStretching(double *T, double* target, int length);
}
}

#endif // SMOOTMATCHING_H
