#ifndef REFERENCE_H
#define REFERENCE_H

#include "mathused.h"
#include "abstractpoint.h"

enum referenceOrigin{P1,P2,CenterOfMass};
enum referenceRotate{P1toP2,NoneRotate};

//在某时刻通过两个矢量旋转参考系
void CoordinatesTransform(const double *q,const double *p,const double *positon,double *out);
//得到相对速度
void RelativePosition(AbstractPoint *p1,AbstractPoint *p2,double t,double *out);
void RelativePositionCOM(AbstractPoint *p1,AbstractPoint *p2,AbstractPoint *p3,double t,double *out);
//得到相对位置
void RelativeVelocity(AbstractPoint *p1,AbstractPoint *p2,double t,double *out);
//换系类
class ProbeCoordinatesTransform {
public:
    ProbeCoordinatesTransform(AbstractPoint *ip1,AbstractPoint *ip2,AbstractPoint *ip3,referenceOrigin iorigin,referenceRotate irotate,double it);//初始化输入参考目标
    double value(int dimension,double it);//得到某一维度某时刻换系后的坐标
private:
    void evaluate();
    double t{},x[3]{};
    AbstractPoint *p1,*p2,*p3;
    referenceOrigin origin;
    referenceRotate rotate;
};

#endif // REFERENCE_H
