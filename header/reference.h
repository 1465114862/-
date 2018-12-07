#ifndef REFERENCE_H
#define REFERENCE_H

#include "mathused.h"

enum referenceOrigin{P1,P2,CenterOfMass};
enum referenceRotate{P1toP2,NoneRotate};

//在某时刻通过两个矢量旋转参考系
void CoordinatesTransform(const double *q,const double *p,const double *positon,double *out);
//得到相对速度
template <class T1,class T2>
void RelativePosition(T1 &p1,T2 &p2,double t,double *out);
//得到相对位置
template <class T1,class T2>
void RelativeVelocity(T1 &p1,T2 &p2,double t,double *out);
//换系类
template <class T1,class T2,class T3>
class ProbeCoordinatesTransform {
public:
    ProbeCoordinatesTransform(T1 &ip1,T2 &ip2,T3 &ip3,referenceOrigin iorigin,referenceRotate irotate);//初始化输入参考目标
    double value(int dimension,double it);//得到某一维度某时刻换系后的坐标
private:
    void evaluate();
    double t{},x[3]{};
    T1 *p1;
    T2 *p2;
    T3 *p3;
    referenceOrigin origin;
    referenceRotate rotate;
};
//得到相对速度
template <class T1,class T2>
void RelativePosition(T1 &p1,T2 &p2,double t,double *out) {
    for(int i=0;i<3;i++)
        out[i]=(*p1).value(i,t)-(*p2).value(i,t);
}
template <class T1,class T2,class T3>
void RelativePositionCOM(T1 &p1,T2 &p2,T3 &p3,double t,double *out) {
    for(int i=0;i<3;i++)
        out[i]=(*p3).value(i,t)-((*p2).value(i,t)*(*p2).mass+(*p1).value(i,t)*(*p1).mass)/((*p2).mass+(*p1).mass);
}
//得到相对位置
template <class T1,class T2>
void RelativeVelocity(T1 &p1,T2 &p2,double t,double *out) {
    for(int i=0;i<3;i++)
        out[i]=(*p1).velocity(i,t)-(*p2).velocity(i,t);
}
template <class T1,class T2,class T3>
ProbeCoordinatesTransform<T1,T2,T3>::ProbeCoordinatesTransform(T1 &ip1,T2 &ip2,T3 &ip3,referenceOrigin iorigin,referenceRotate irotate): p1(&ip1),p2(&ip2),p3(&ip3),rotate(irotate),origin(iorigin){t=0;evaluate();}
template <class T1,class T2,class T3>
void ProbeCoordinatesTransform<T1,T2,T3>::evaluate() {
    switch(rotate) {
    case P1toP2:
        double q[3], p[3], position[3];
        RelativePosition(p1, p2, t, q);
        RelativeVelocity(p1, p2, t, p);
        switch(origin) {
        case P1:
            RelativePosition(p3, p1, t, position);
            CoordinatesTransform(q, p, position, x);
            break;
        case P2:
            RelativePosition(p3, p2, t, position);
            CoordinatesTransform(q, p, position, x);
            break;
        case CenterOfMass:
            RelativePositionCOM(p1, p2,p3, t, position);
            CoordinatesTransform(q, p, position, x);
            break;
        }
        break;
    case NoneRotate:
        switch(origin) {
        case P1:
            RelativePosition(p3, p1, t, x);
            break;
        case P2:
            RelativePosition(p3, p2, t, x);
            break;
        case CenterOfMass:
            RelativePositionCOM(p1, p2,p3, t, x);
            break;
        }
        break;
    }
    /*double q[3], p[3], position[3];
    RelativePosition(p1, p2, t, q);
    RelativePosition(p3, p1, t, position);
    RelativeVelocity(p1, p2, t, p);
    CoordinatesTransform(q, p, position, x);*/
}
template <class T1,class T2,class T3>
double ProbeCoordinatesTransform<T1,T2,T3>::value(int dimension,double it) {
    if(it==t)
        return x[dimension];
    else {
        t=it;
        evaluate();
        return x[dimension];
    }
}

#endif // REFERENCE_H
