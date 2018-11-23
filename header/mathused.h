#ifndef MATHUSED_H
#define MATHUSED_H

#include <cmath>

//点乘
double vdot(const double * v1, const double * v2,int length,int h1=1,int h2=1);
//取模
double norm(const double *v,int length,int h);
//正规化
void normalize(const double *in,double *out,int length,int hin=1,int hout=1);
//矢量加
void vplus(const double * in1, const double * in2,double *out,int length,int inh1=1,int inh2=1,int outh=1);
void vselfplus(const double * in,double *out,int length,int inh=1,int outh=1);
//矢量减
void vminus(const double * in1, const double * in2,double *out,int length,int inh1=1,int inh2=1,int outh=1);
void vequ(const double * in,double *out,int length,int inh=1,int outh=1);
//数乘
void ndot(const double * in,double *out, double n,int length,int inh=1,int outh=1);
//立方
double cube(double x);
//三对角矩阵的LU分解
void thomas(int n,const double *a, const double *b, const double *c, const double *B,double *x);
//矩阵乘矢量
void mvdot(const double *inm,const double *inv,double *out,int vlength,int m,int hout=1,int hin=1);
void mvdotminus(const double *inm,const double *inv,double *out,int vlength,int m,int hout=1,int hin=1);
void mvdotplus(const double *inm,const double *inv,double *out,int vlength,int m,int hout=1,int hin=1);
//矩阵乘法
void mdot(const double *inm1,const double *inm2,double *outm,int row,int rank);
//三维叉积
void cross3(const double *in1,const double *in2,double *out,int in1h=1,int in2h=1,int outh=1);
//非球引力场J2修正
void J2Coefficient(double *f,double *r,double *j, double j2, double d,double sqrtd);
void NewJ2Coefficient(double *f,double *r,double *j, double j2, double d,double sqrtd,double &V,double &Vt,double *vp);
//最大值或最小值
double FindMinOrMax(bool Max,const double *a,int length,int step=1);

#endif // MATHUSED_H
