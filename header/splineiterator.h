#ifndef SPLINEITERATOR_H
#define SPLINEITERATOR_H

#include "spline.h"

class SplineIterator
{
public:
    SplineIterator();
    SplineIterator(const Spline & spline);
    void setSpline(const Spline & spline);
    double value(const double & in);//得到函数值
private:
    double *h,*M,*A,*B,*x;
    int DataLength,history;
};

#endif // SPLINEITERATOR_H
