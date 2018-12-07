#ifndef PROBE_H
#define PROBE_H

#include "planet.h"
#include "maneuver.h"
#include "plot.h"

#include <stdexcept>
//飞行器类
class Probe
{
public:
    int DataLength,SplineDataLength,SplineStart,SplineEnd,SplineIndex,history;
    double InitialPosition[3],InitialVelocity[3],*Position,*Velocity,*T,*p0,acceleration[3],dp[3],dq[3],PCache[3],QCache[3],*tGroup,EndT,TCache,P0Cache,dT,dp0,Vt,V,fq,fp,*tau,tauCache,dtau,*W,WCache,dW,theW,MdAcceleration[9],dAcceleration[3],VdAcceleration[3];
    bool StepRunOut,crashed;
    Spline *SplinePosition,*SplineVelocity;
    Maneuver maneuver;
    std::string name;
    GlPlot3D *probeplot;
    void GetPhaseSpaceInitialPosition(const double *QSource, const double *PSource);//从数组得到初始条件
    void InitializationPhaseSpace(int step);//求解前初始化
    void ClearAcceleration();//清除加速度缓存
    void Cleardpdq();//清除坐标和动量空间增量缓存
    void GetSpline(double *XSource);//由数据点插值
    void Acceleration(Planet &Planets,double t);//得到某个星球对其加速度
    void ClearPositon();//解出方程后清除数据点存储
    friend void ISPRK4ForProbe(Planet *Planets,int planetcount,Probe &probe,int step, double h);
    void ProbeAcceleration(Planet *Planets,int planetcount,double t);
    void ProbeDqUpdate(int now,double hB);
    void ProbeDpUpdate(Planet *Planets,int planetcount,double t,int now,double hb);
    void ProbePositionUpdate(int next);
    void ProbeVelocityUpdate(int next);
    void ISPRK4ForProbe(Planet *Planets,int planetcount,int step, double h,double tmax,double tbegin);
    void InitializationAfterManeuverPhaseSpace(int number);
    void NewClearPositon();
    void NewCleardpdq();
    void NewClearAcceleration();//清除加速度缓存
    void NewInitializationAfterManeuverPhaseSpace(int number,Planet *Planets,int planetcount);
    void NewInitializationPhaseSpace(int step,double tbegin,Planet *Planets,int planetcount);//求解前初始化
    void NewAcceleration(Planet &Planets);//得到某个星球对其加速度
    void NewProbeAcceleration(Planet *Planets,int planetcount);
    void NewProbeDqUpdate(int now,double B,const double &epsilon);
    void NewProbeDpUpdate(Planet *Planets,int planetcount,int now,double b,const double &epsilon);
    void NewProbePositionUpdate(int next);
    void NewProbeVelocityUpdate(int next);
    void NewISPRK4ForProbe(Planet *Planets,int planetcount,int step, double epsilon,double tmax,double tbegin);
    void NewSolveOrbit(Planet *Planets,int planetcount,int step, double epsilon,double tmax);
    void SolveOrbit(Planet *Planets,int planetcount,int step, double h,double tmax);
    double getE(int number,Planet *Planets,int planetcount);
    double value(int dimension,double t);
    double velocity(int dimension,double t);
    void Gamma3ClearPositon();
    void Gamma3Cleardpdq();
    void Gamma3ClearAcceleration();//清除加速度缓存
    void Gamma3InitializationAfterManeuverPhaseSpace(int number,Planet *Planets,int planetcount, double epsilon);
    void Gamma3InitializationPhaseSpace(int step,double tbegin,Planet *Planets,int planetcount, double epsilon);//求解前初始化
    void Gamma3Acceleration(Planet &Planets);//得到某个星球对其加速度
    void Gamma3ProbeAcceleration(Planet *Planets,int planetcount);
    void Gamma3A(int now,const double &h);
    void Gamma3W();
    void Gamma3D(Planet *Planets,int planetcount,int now,const double &h);
    void FirstGamma3D(Planet *Planets,int planetcount,int now,const double &h);
    void FirstGamma3ProbeAcceleration(Planet *Planets,int planetcount);
    void FirstGamma3Acceleration(Planet &Planets);
    void Gamma3ProbeUpdate(int next);
    void Gamma3ISPRK4ForProbe(Planet *Planets,int planetcount,int step, double epsilon,double tmax,double tbegin);
    void Gamma3SolveOrbit(Planet *Planets,int planetcount,int step, double epsilon,double tmax);
    double getW(int number,Planet *Planets,int planetcount);
    void dataRefresh();
};
template<class T>
void safeDelete(T p) {
    if(!p) {
        delete []p;
        p=0;
    }
}
void Gamma3J2Coefficient(double *f,double *r,double *j,const double & j2, const double & d,const double & sqrtd,double &Vt,double *vp);

#endif // PROBE_H
