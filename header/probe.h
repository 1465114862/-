#ifndef PROBE_H
#define PROBE_H

#include "splineiterator.h"
#include "planet.h"
#include "maneuver.h"
#include "plot.h"
#include "abstractpoint.h"

#include <stdexcept>
//飞行器类
class Probe : public AbstractPoint
{
public:
    Probe();
    SplineIterator *planetsOrbit{nullptr};
    int DataLength,SplineDataLength,SplineStart,SplineEnd,SplineIndex,history,ReverseStep;
    double hmin{0.001},rmin,InitialPosition[3],InitialVelocity[3],EndPosition[3],EndVelocity[3],StartT,*Position{nullptr},*Velocity{nullptr},*T{nullptr},*p0{nullptr},acceleration[3],dp[3],dq[3],PCache[3],QCache[3],*tGroup{nullptr},EndT,TCache,P0Cache,dT,dp0,Vt,V,fq,fp,*tau{nullptr},tauCache,dtau,*W{nullptr},WCache,dW,theW,MdAcceleration[9],dAcceleration[3],VdAcceleration[3];
    bool StepRunOut,crashed,reach,stop;
    Spline *SplinePosition{nullptr},*SplineVelocity{nullptr};
    Maneuver maneuver;
    double getDistanceToPlanet(const int & in,const bool & next);
    void GetPhaseSpaceInitialPosition(const double *QSource, const double *PSource);//从数组得到初始条件
    void InitializationPhaseSpace(int step);//求解前初始化
    void ClearAcceleration();//清除加速度缓存
    void Cleardpdq();//清除坐标和动量空间增量缓存
    void GetSpline(double *XSource);//由数据点插值
    void Acceleration(Planet &Planets,double t,SplineIterator *pos);//得到某个星球对其加速度
    void ClearPositon();//解出方程后清除数据点存储
    friend void ISPRK4ForProbe(Planet *Planets,int planetcount,Probe &probe,int step, double h);
    void ProbeAcceleration(Planet *Planets,int planetcount,double t);
    void ProbeDqUpdate(int now,double hB);
    void ProbeDpUpdate(Planet *Planets,int planetcount,double t,int now,double hb);
    void ProbePositionUpdate(int next);
    void ProbeVelocityUpdate(int next);
    void ISPRK4ForProbe(Planet *Planets,int planetcount,int step, double h,double tmax,double tbegin);
    void InitializationAfterManeuverPhaseSpace(int number);
    void SolveOrbit(Planet *Planets,int planetcount,int step, double h,double tmin,double tmax);
    double getE(int number,Planet *Planets,int planetcount);
    double value(int dimension,double t) override;
    double velocity(int dimension,double t) override;
    void Gamma3ClearPositon();
    void Gamma3Cleardpdq();
    void Gamma3ClearAcceleration();//清除加速度缓存
    void Gamma3InitializationAfterManeuverPhaseSpace(int number,Planet *Planets,int planetcount, double epsilon);
    void Gamma3InitializationPhaseSpace(int step,double tbegin,Planet *Planets,int planetcount, double epsilon);//求解前初始化
    void Gamma3Acceleration(Planet &Planets,SplineIterator *pos);//得到某个星球对其加速度
    void Gamma3ProbeAcceleration(Planet *Planets,int planetcount);
    void Gamma3A(int now,const double &h);
    void Gamma3W();
    void Gamma3D(Planet *Planets,int planetcount,int now,const double &h);
    void FirstGamma3D(Planet *Planets,int planetcount,int now,const double &h);
    void FirstGamma3ProbeAcceleration(Planet *Planets,int planetcount);
    void FirstGamma3Acceleration(Planet &Planets,SplineIterator *pos);
    void Gamma3ProbeUpdate(int next);
    void Gamma3ISPRK4ForProbe(Planet *Planets,int planetcount,int step, double epsilon,double tmax,double tbegin);

    void SetPhaseSpaceEndPosition(const double *QSource, const double *PSource);//从数组得到初始条件
    void GetPhaseSpaceStartPosition(double *QSource, double *PSource,double *oStartT);
    void ReverseGamma3InitializationAfterManeuverPhaseSpace(int number,Planet *Planets,int planetcount, double epsilon);
    void ReverseGamma3InitializationPhaseSpace(int step,double tbegin,Planet *Planets,int planetcount, double epsilon);//求解前初始化
    void ReverseGamma3A(const double &h);
    void ReverseUpdate();
    void ReverseGamma3D(Planet *Planets,int planetcount,const double &h);
    void FirstReverseGamma3D(Planet *Planets,int planetcount,const double &h);
    void ReverseGamma3ProbeUpdate();
    void ReverseGamma3ISPRK4ForProbe(Planet *Planets,int planetcount,int step, double epsilon,double tbegin,double tmax);
    void ReverseGamma3SolveOrbit(Planet *Planets,int planetcount,int step, double epsilon,double tmin,double tmax,double *outQ,double *outP);
    std::pair<double,SingleManeuver> ReverseGamma3SolveOrbit(Planet *Planets,int planetcount,int step, double epsilon,double tmin,double tmax);

    void Gamma3SolveOrbit(Planet *Planets,int planetcount,int step, double epsilon,double tmin,double tmax);
    double getW(int number,Planet *Planets,int planetcount);
    void dataRefresh();
    ~Probe();
};
void Gamma3J2Coefficient(double *f,double *r,double *j,const double & j2, const double & d,const double & sqrtd,double &Vt,double *vp);

#endif // PROBE_H
