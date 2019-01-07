#include "mathused.h"



//点乘
double vdot(const double * v1, const double * v2,int length,int h1,int h2) {
    double result=0;
    for(int i=0;i<length;i++)
        result+=(*(v1+h1*i))*(*(v2+h2*i));
    return result;
}
//取模
double norm(const double *v,int length,int h) {
    double result=sqrt(vdot(v,v,length,h,h));
    return result;
}
//正规化
void normalize(const double *in,double *out,int length,int hin,int hout) {
    double InNorm=norm(in,length,hin);
    for(int i=0;i<length;i++)
        *(out+hout*i)=*(in+hin*i)/InNorm;
}
//矢量加
void vplus(const double * in1, const double * in2,double *out,int length,int inh1,int inh2,int outh) {
    for(int i=0;i<length;i++)
        *(out+outh*i)=(*(in1+inh1*i))+(*(in2+inh2*i));
}
void vselfplus(const double * in,double *out,int length,int inh,int outh) {
    for(int i=0;i<length;i++)
        *(out+outh*i)+=(*(in+inh*i));
}
//矢量减
void vminus(const double * in1, const double * in2,double *out,int length,int inh1,int inh2,int outh) {
    for(int i=0;i<length;i++)
        *(out+outh*i)=(*(in1+inh1*i))-(*(in2+inh2*i));
}
void vequ(const double * in,double *out,int length,int inh,int outh) {
    for(int i=0;i<length;i++)
        *(out+outh*i)=(*(in+inh*i));
}
//数乘
void ndot(const double * in,double *out, double n,int length,int inh,int outh) {
    for(int i=0;i<length;i++)
        *(out+outh*i)=(*(in+inh*i))*n;
}
//立方
double cube(double x) {
    return x*x*x;
}
//三对角矩阵的LU分解
void thomas(int n,const double *a, const double *b, const double *c, const double *B,double *x) {
    double alpha[n],beta[n],xz[n];
    alpha[0]=a[0];
    beta[0]=0;
    for(int i=1;i<n;i++) {
        beta[i]=b[i]/alpha[i-1];
        alpha[i]=a[i]-beta[i]*c[i-1];
    }
    xz[0]=B[0];
    for(int i=1;i<n;i++)
        xz[i]=B[i]-beta[i]*xz[i-1];
    x[n-1]=xz[n-1]/alpha[n-1];
    for(int i=n-2; i >= 0;i--)
        x[i]=(xz[i]-c[i]*x[i+1])/alpha[i];
}
//矩阵乘矢量
void mvdot(const double *inm,const double *inv,double *out,int vlength,int m,int hout,int hin) {
    for(int i=0;i<m;i++)
        out[i*hout]=vdot(&inm[i*vlength],inv,vlength,1,hin);
}
void mvdotminus(const double *inm,const double *inv,double *out,int vlength,int m,int hout,int hin) {
    for(int i=0;i<m;i++)
        out[i*hout]-=vdot(&inm[i*vlength],inv,vlength,1,hin);
}
void mvdotplus(const double *inm,const double *inv,double *out,int vlength,int m,int hout,int hin) {
    for(int i=0;i<m;i++)
        out[i*hout]+=vdot(&inm[i*vlength],inv,vlength,1,hin);
}
//矩阵乘法
void mdot(const double *inm1,const double *inm2,double *outm,int row,int rank) {
    for(int i=0;i<rank;i++)
        mvdot(inm2,&inm1[i],&outm[i],row,rank,rank,rank);
}
//三维叉积
void cross3(const double *in1,const double *in2,double *out,int in1h,int in2h,int outh) {
    for(int i=0;i<3;i++)
        out[i*outh]=in1[((i+1)%3)*in1h]*in2[((i+2)%3)*in2h]-in1[((i+2)%3)*in1h]*in2[((i+1)%3)*in2h];
}
//非球引力场J2修正
void J2Coefficient(double *f,double *r,double *j, double j2, double d,double sqrtd) {
    double c=vdot(j,r,3,1,1),e=j2/(d*d*sqrtd),a=3*c*e,b=3*e*(1-5*c*c/d)*0.5;
    for(int i=0;i<3;i++)
        f[i]+=a*j[i]+b*r[i];
}
//最大值或最小值
double FindMinOrMax(bool Max,const double *a,int length,int step) {
    double cache=a[0];
    for(int i=1;i<length;i++) {
        if((Max ? a[i*step]>cache : a[i*step]<cache))
            cache=a[i*step];
    }
    return cache;
}
