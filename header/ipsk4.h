#ifndef IPSK4_H
#define IPSK4_H

#include "planet.h"
#include "probe.h"

//得到某一个行星受所有行星的力
void PlanetsAcceleration(Planet *Planets,int planetcount);
//清除所有行星的相空间增量缓存
void ClearAlldpdq(Planet *Planets,int planetcount);
//更新动量增量
void dqUpdate(Planet *Planets,int planetcount,int now,int step,double hB);
//更新位置增量
void dpUpdate(Planet *Planets,int planetcount,int now,int step,double hb);
//更新位置
void PositionUpdate(Planet *Planets,int planetcount,int next,int step);
//更新速度
void VelocityUpdate(Planet *Planets,int planetcount,int next,int step);
//内插得到所有行星的解
void SplineAll(Planet *Planets,int planetcount,double *XSource);
//清除所有行星数据点缓存
void ClearAllPositon(Planet *Planets,int planetcount);
//对于行星的ISPRK4求解器
void ISPRK4ForPlanets(Planet *Planets,int planetcount,int step, double h);
//初始化原飞行器
void InitializationOriginalProbe(Probe &CenterOfMass,Probe &OriginalProbe);

#endif // IPSK4_H
