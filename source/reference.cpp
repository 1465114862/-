#include "reference.h"
void CoordinatesTransform(const double *q,const double *p,const double *positon,double *out) {
    double axe[9],AxeCache[3];
    normalize(q,axe,3);
    cross3(q,p,AxeCache);
    normalize(AxeCache,&axe[6],3);
    cross3(&axe[6],axe,&axe[3]);
    mvdot(axe,positon,out,3,3);
}
//得到相对速度
void RelativePosition(AbstractPoint *p1,AbstractPoint *p2,double t,double *out) {
    for(int i=0;i<3;i++)
        out[i]=(*p1).value(i,t)-(*p2).value(i,t);
}
void RelativePositionCOM(AbstractPoint *p1,AbstractPoint *p2,AbstractPoint *p3,double t,double *out) {
    for(int i=0;i<3;i++)
        out[i]=(*p3).value(i,t)-((*p2).value(i,t)*(*p2).mass+(*p1).value(i,t)*(*p1).mass)/((*p2).mass+(*p1).mass);
}
//得到相对位置
void RelativeVelocity(AbstractPoint *p1,AbstractPoint *p2,double t,double *out) {
    for(int i=0;i<3;i++)
        out[i]=(*p1).velocity(i,t)-(*p2).velocity(i,t);
}
ProbeCoordinatesTransform::ProbeCoordinatesTransform(AbstractPoint *ip1,AbstractPoint *ip2,AbstractPoint *ip3,referenceOrigin iorigin,referenceRotate irotate,double it): p1(ip1),p2(ip2),p3(ip3),rotate(irotate),origin(iorigin){t=it;evaluate();}
void ProbeCoordinatesTransform::evaluate() {
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
}
double ProbeCoordinatesTransform::value(int dimension,double it) {
    if(it==t)
        return x[dimension];
    else {
        t=it;
        evaluate();
        return x[dimension];
    }
}
