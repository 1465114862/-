#include "ipsk4.h"
//得到某一个行星受所有行星的力
void PlanetsAcceleration(Planet *Planets,int planetcount) {
    for(int j=0;j<planetcount;j++) {
        Planets[j].ClearAcceleration();
        for(int i=0;i<planetcount;i++) {
            if(i!=j)
                gravity(Planets[j],Planets[i]);
        }
    }

}
//清除所有行星的相空间增量缓存
void ClearAlldpdq(Planet *Planets,int planetcount) {
    for(int i=0;i<planetcount;i++)
        Planets[i].Cleardpdq();
}
//更新动量增量
void dqUpdate(Planet *Planets,int planetcount,int now,int step,double hB) {
    for(int i=0;i<planetcount;i++)
        vplus(Planets[i].Velocity+now,Planets[i].dp,Planets[i].PCache,3,step);
    for(int i=0;i<planetcount;i++)
        ndot(Planets[i].PCache,Planets[i].PCache,hB,3);
    for(int i=0;i<planetcount;i++)
        vplus(Planets[i].dq,Planets[i].PCache,Planets[i].dq,3);
}
//更新位置增量
void dpUpdate(Planet *Planets,int planetcount,int now,int step,double hb) {
    for(int i=0;i<planetcount;i++)
        vplus(Planets[i].Position+now,Planets[i].dq,Planets[i].QCache,3,step);
    PlanetsAcceleration(Planets,planetcount);
    for(int i=0;i<planetcount;i++)
        ndot(Planets[i].acceleration,Planets[i].QCache,hb,3);
    for(int i=0;i<planetcount;i++)
        vplus(Planets[i].dp,Planets[i].QCache,Planets[i].dp,3);
}
//更新位置
void PositionUpdate(Planet *Planets,int planetcount,int next,int step) {
    for(int i=0;i<planetcount;i++)
        vplus(Planets[i].Position+next-1,Planets[i].dq,Planets[i].Position+next,3,step,1,step);
}
//更新速度
void VelocityUpdate(Planet *Planets,int planetcount,int next,int step) {
    for(int i=0;i<planetcount;i++)
        vplus(Planets[i].Velocity+next-1,Planets[i].dp,Planets[i].Velocity+next,3,step,1,step);
}
//内插得到所有行星的解
void SplineAll(Planet *Planets,int planetcount,double *XSource) {
    for(int i=0;i<planetcount;i++)
        Planets[i].GetSpline(XSource);
}
//清除所有行星数据点缓存
void ClearAllPositon(Planet *Planets,int planetcount) {
    for(int i=0;i<planetcount;i++)
        Planets[i].ClearPositon();
}
//对于行星的ISPRK4求解器
void ISPRK4ForPlanets(Planet *Planets,int planetcount,int step, double h) {
    double w = (2 + pow(2, 1.0 / 3) + 1 / pow(2, 1.0 / 3)) / 3,XSource[step];
    double v = 1 - 2 * w,b[4] = {w, v, w, 0},B[4] = {w / 2, (w + v) / 2, (w + v) / 2, w / 2},hB[4],hb[4];
    for(int i=0;i<step;i++)
        XSource[i]=i*h;
    ndot(B,hB,h,4);
    ndot(b,hb,h,4);
    for(int i=0;i<planetcount;i++)
        Planets[i].InitializationPhaseSpace(step);
    for(int i=0;i<step-1;i++) {
        ClearAlldpdq(Planets,planetcount);
        for(int j=0;j<4;j++) {
            dqUpdate(Planets,planetcount,i,step,hB[j]);
            if(hb[j]!=0)
                dpUpdate(Planets,planetcount,i,step,hb[j]);
        }
        PositionUpdate(Planets,planetcount,i+1,step);
        VelocityUpdate(Planets,planetcount,i+1,step);
    }
    SplineAll(Planets,planetcount,XSource);
    ClearAllPositon(Planets,planetcount);
}
//初始化原飞行器
void InitializationOriginalProbe(Probe &CenterOfMass,Probe &OriginalProbe) {
    for(int i=0;i<3;i++) {
        OriginalProbe.InitialVelocity[i]=-CenterOfMass.InitialVelocity[i];
        OriginalProbe.InitialPosition[i]=-CenterOfMass.InitialPosition[i];
    }
}
