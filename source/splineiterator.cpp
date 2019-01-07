#include "splineiterator.h"
#include <iostream>
SplineIterator::SplineIterator(){}
SplineIterator::SplineIterator(const Spline & spline)
{
    setSpline(spline);
}
void SplineIterator::setSpline(const Spline & spline) {
    h=spline.h;
    M=spline.M;
    A=spline.A;
    B=spline.B;
    x=spline.x;
    history=0;
    DataLength=spline.DataLength;
}
double SplineIterator::value(const double & in) {
    for(int j=0;j<DataLength-1;j++) {
        if(x[history]<=in && in<=x[history+1])
            return M[history]*cube(x[history+1]-in)/(h[history]*6)+M[history+1]*cube(in-x[history])/(h[history]*6)+A[history]*(in-x[history])+B[history];
        if(in>x[history+1])
            history++;
        if(in<x[history])
            history--;
    }
    return 0;
}
