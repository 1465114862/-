#include "reference.h"
void CoordinatesTransform(const double *q,const double *p,const double *positon,double *out) {
    double axe[9],AxeCache[3];
    normalize(q,axe,3);
    cross3(q,p,AxeCache);
    normalize(AxeCache,&axe[6],3);
    cross3(&axe[6],axe,&axe[3]);
    mvdot(axe,positon,out,3,3);
}
