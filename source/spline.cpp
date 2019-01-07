#include "spline.h"
#include <fstream>
#include <stdexcept>
#include <iostream>
void Spline::GetSourceData(const int & SourceDataLength,const double *SourceDataX,const double *SourceDataY) {
    history=0;
    x=new double[SourceDataLength];
    h=new double[SourceDataLength-1];
    M=new double[SourceDataLength];
    A=new double[SourceDataLength-1];
    B=new double[SourceDataLength-1];
    double a[SourceDataLength-2],b[SourceDataLength-2],c[SourceDataLength-2],B1[SourceDataLength-2];
    for(int i=0;i<SourceDataLength;i++)
        x[i]=SourceDataX[i];
    for(int i=0;i<SourceDataLength-1;i++)
        h[i]=SourceDataX[i+1]-SourceDataX[i];
    for(int i=0;i<SourceDataLength-2;i++)
        a[i]=(h[i+1]+h[i])/3;
    for(int i=1;i<SourceDataLength-2;i++)
        b[i]=h[i]/6;
    b[0]=0;
    for(int i=0;i<SourceDataLength-3;i++)
        c[i]=h[i+1]/6;
    c[SourceDataLength-3]=0;
    for(int i=0;i<SourceDataLength-2;i++)
        B1[i]=(SourceDataY[i+2]-SourceDataY[i+1])/h[i+1]-(SourceDataY[i+1]-SourceDataY[i])/h[i];
    M[0]=0;
    M[SourceDataLength-1]=0;
    thomas(SourceDataLength-2,a,b,c,B1,&M[1]);
    for(int i=0;i<SourceDataLength-1;i++)
        B[i]=SourceDataY[i]-M[i]*h[i]*h[i]/6;
    for(int i=0;i<SourceDataLength-1;i++)
        A[i]=(SourceDataY[i+1]-SourceDataY[i])/h[i]-(M[i+1]-M[i])*h[i]/6;
    DataLength=SourceDataLength;
}
double Spline::value(const double & in) {
    for(int j=0;j<DataLength-1;j++) {
        if(x[history]<=in && in<=x[history+1])
            return M[history]*cube(x[history+1]-in)/(h[history]*6)+M[history+1]*cube(in-x[history])/(h[history]*6)+A[history]*(in-x[history])+B[history];///问题
        if(in>x[history+1])
            history++;
        if(in<x[history])
            history--;
    }
    return 0;
}
void Spline::save(std::fstream &fs) {
    fs.write(reinterpret_cast<char*>(&DataLength), sizeof(int));
    fs.write(reinterpret_cast<char*>(M), sizeof(double)*DataLength);
    fs.write(reinterpret_cast<char*>(x), sizeof(double)*DataLength);
    fs.write(reinterpret_cast<char*>(A), sizeof(double)*(DataLength-1));
    fs.write(reinterpret_cast<char*>(B), sizeof(double)*(DataLength-1));
}
void Spline::load(std::fstream &fs) {
    fs.read(reinterpret_cast<char*>(&DataLength), sizeof(int));
    history=0;
    x=new double[DataLength];
    h=new double[DataLength-1];
    M=new double[DataLength];
    A=new double[DataLength-1];
    B=new double[DataLength-1];
    fs.read(reinterpret_cast<char*>(M), sizeof(double)*DataLength);
    fs.read(reinterpret_cast<char*>(x), sizeof(double)*DataLength);
    fs.read(reinterpret_cast<char*>(A), sizeof(double)*(DataLength-1));
    fs.read(reinterpret_cast<char*>(B), sizeof(double)*(DataLength-1));
    for(int i=0;i<DataLength-1;i++)
        h[i]=x[i+1]-x[i];
}

Spline::~Spline() {
    if(h) {delete [] h;}
    if(x) {delete [] x;}
    if(A) {delete [] A;}
    if(B) {delete [] B;}
    if(M) {delete [] M;}
}
