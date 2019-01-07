#include "probe.h"
#include <iostream>
#include <limits.h>
Probe::Probe() {}
void Probe::SetPhaseSpaceEndPosition(const double *QSource, const double *PSource) {
    for(int i=0;i<3;i++) {
        EndPosition[i] = QSource[i];
        EndVelocity[i] = PSource[i];
    }
}
void Probe::GetPhaseSpaceStartPosition(double *QSource, double *PSource,double *oStartT) {
    for(int i=0;i<3;i++) {
        QSource[i]=Position[i*DataLength];
        PSource[i]=Velocity[i*DataLength];
    }
    (*oStartT)=StartT;
}
void Probe::ReverseGamma3InitializationAfterManeuverPhaseSpace(int number,Planet *Planets,int planetcount, double epsilon){
    maneuver.iteratorReach(number);
    for(int i=0;i<3;i++) {
        Velocity[i*2] = Velocity[i*2]-maneuver.maneuverIterator->v[i];
    }
    TCache=T[0];
    p0[0]=-getE(0,Planets,planetcount);
    W[0]=getW(0,Planets,planetcount)*epsilon;
}
void Probe::ReverseGamma3InitializationPhaseSpace(int step,double tbegin,Planet *Planets,int planetcount, double epsilon){
    Position = new double[3*2];
    Velocity = new double[3*2];
    T=new double[2];
    p0=new double[2];
    W=new double[2];
    T[0]=tbegin;
    for(int i=0;i<3;i++) {
        Position[i*(2)] = EndPosition[i];
    }
    for(int i=0;i<3;i++) {
        Velocity[i*(2)] = EndVelocity[i];
    }
    DataLength=2;
    ReverseStep=step;
    p0[0]=-getE(0,Planets,planetcount);
    W[0]=getW(0,Planets,planetcount)*epsilon;
}
void Probe::ReverseGamma3A(const double &h){
    WCache=*W+dW;
    P0Cache=*p0+dp0;
    vplus(Velocity,dp,PCache,3,DataLength);
    fq=h/WCache;//
    ndot(PCache,PCache,fq,3);
    dT+=fq;
    vplus(dq,PCache,dq,3);
}
void Probe::ReverseGamma3D(Planet *Planets,int planetcount,const double &h){
    TCache=*T+dT;
    vplus(Position,dq,QCache,3,DataLength);
    Gamma3ProbeAcceleration(Planets,planetcount);
    vplus(Velocity,dp,PCache,3,DataLength);
    Gamma3W();
    dW+=theW*0.5*h;//
    WCache=*W+dW;
    ndot(acceleration,QCache,h/WCache,3);//
    vplus(dp,QCache,dp,3);
    dp0+=Vt/WCache;//
    vplus(Velocity,dp,PCache,3,DataLength);
    Gamma3W();
    dW+=theW*0.5*h;//
}
void Probe::FirstReverseGamma3D(Planet *Planets,int planetcount,const double &h){
    TCache=*(T)+dT;
    vplus(Position,dq,QCache,3,DataLength);
    FirstGamma3ProbeAcceleration(Planets,planetcount);
    vplus(Velocity,dp,PCache,3,DataLength);
    Gamma3W();
    dW+=theW*0.5*h;//
    WCache=*W+dW;
    ndot(acceleration,QCache,h/WCache,3);//
    vplus(dp,QCache,dp,3);
    dp0+=Vt/WCache;//
    vplus(Velocity,dp,PCache,3,DataLength);
    Gamma3W();
    dW+=theW*0.5*h;//
}
void Probe::ReverseGamma3ProbeUpdate(){
    vplus(Velocity,dp,Velocity+1,3,DataLength,1,DataLength);
    *(p0+1)=*(p0)+dp0;
    vplus(Position,dq,Position+1,3,DataLength,1,DataLength);
    *(T+1)=*(T)+dT;
    *(W+1)=*(W)+dW;
}
void Probe::ReverseUpdate(){
    for(int i=0;i<DataLength*3;i+=DataLength){
        Velocity[i]=Velocity[i+1];
        Position[i]=Position[i+1];
    }
    *(p0)=*(p0+1);
    *(T)=*(T+1);
    *(W)=*(W+1);
}
double Probe::getDistanceToPlanet(const int & in,const bool & next) {
    double temporaryPos[3];
    for(int i=0;i<3;i++)
        temporaryPos[i]=*(Position+static_cast<int>(next)+2*i)-(planetsOrbit+in*6+2*i)->value(T[static_cast<int>(next)]);
    return norm(temporaryPos,3,1);
}
void Probe::ReverseGamma3ISPRK4ForProbe(Planet *Planets,int planetcount,int step, double epsilon,double tbegin,double tmax){
    double w{1/ (2 - pow(2, 1.0 / 3) )},v{1-2*w};
    double Para[7] = {w / 2,w, (1-w) / 2,v, (1-w) / 2,w,w / 2};
    double Ttol=0.01,Rtol=1;
    bool end=false;
    if(SplineIndex==maneuver.length+1)//
        ReverseGamma3InitializationPhaseSpace(step,tmax,Planets,planetcount,epsilon);
    else
        ReverseGamma3InitializationAfterManeuverPhaseSpace(SplineIndex,Planets,planetcount,epsilon);
    for(int i=0;i<step-1;i++) {
        ReverseStep--;
        Gamma3Cleardpdq();
        ReverseGamma3A(Para[0]);
        FirstReverseGamma3D(Planets,planetcount,Para[1]);
        ReverseGamma3A(Para[2]);
        ReverseGamma3D(Planets,planetcount,Para[3]);
        ReverseGamma3A(Para[4]);
        ReverseGamma3D(Planets,planetcount,Para[5]);
        ReverseGamma3A(Para[6]);
        ReverseGamma3ProbeUpdate();
        double temporary;
        temporary=getDistanceToPlanet(6,1);
        if(temporary<rmin){rmin=temporary;}
        if((rmin<(Planets+6)->crashRadius+200000)&&stop){
            end=true;
            reach=true;
            if(true){//
                double solEpsilon[3],solT{getDistanceToPlanet(6,0)-(Planets+6)->crashRadius-200000};
                solEpsilon[0]=0;
                solEpsilon[1]=1;
                solEpsilon[2]=solEpsilon[1]-(solEpsilon[1]-solEpsilon[0])*(rmin-(Planets+6)->crashRadius+200000)/(rmin-(Planets+6)->crashRadius+200000-solT);
                solT=rmin-(Planets+6)->crashRadius-200000;
                while(true) {
                    Gamma3Cleardpdq();
                    ReverseGamma3A(Para[0]*solEpsilon[2]);
                    ReverseGamma3D(Planets,planetcount,Para[1]*solEpsilon[2]);
                    ReverseGamma3A(Para[2]*solEpsilon[2]);
                    ReverseGamma3D(Planets,planetcount,Para[3]*solEpsilon[2]);
                    ReverseGamma3A(Para[4]*solEpsilon[2]);
                    ReverseGamma3D(Planets,planetcount,Para[5]*solEpsilon[2]);
                    ReverseGamma3A(Para[6]*solEpsilon[2]);
                    ReverseGamma3ProbeUpdate();
                    temporary=getDistanceToPlanet(6,1);
                    if((temporary-(Planets+6)->crashRadius-200000<=Rtol)&&(temporary-(Planets+6)->crashRadius-200000>=-Rtol))
                        break;
                    solEpsilon[0]=solEpsilon[1];
                    solEpsilon[1]=solEpsilon[2];
                    solEpsilon[2]=solEpsilon[1]-(solEpsilon[1]-solEpsilon[0])*(temporary-(Planets+6)->crashRadius-200000)/(temporary-(Planets+6)->crashRadius-200000-solT);
                    solT=temporary-(Planets+6)->crashRadius-200000;
                }
                ReverseUpdate();
            }
            break;
        }
        if(T[1]<=tbegin) {
            end=true;
            if(true){//
                double solEpsilon[3],solT{T[0]-tbegin};
                solEpsilon[0]=0;
                solEpsilon[1]=1;
                solEpsilon[2]=solEpsilon[1]-(solEpsilon[1]-solEpsilon[0])*(T[1]-tbegin)/(T[1]-tbegin-solT);
                solT=T[1]-tbegin;
                while(true) {
                    Gamma3Cleardpdq();
                    ReverseGamma3A(Para[0]*solEpsilon[2]);
                    ReverseGamma3D(Planets,planetcount,Para[1]*solEpsilon[2]);
                    ReverseGamma3A(Para[2]*solEpsilon[2]);
                    ReverseGamma3D(Planets,planetcount,Para[3]*solEpsilon[2]);
                    ReverseGamma3A(Para[4]*solEpsilon[2]);
                    ReverseGamma3D(Planets,planetcount,Para[5]*solEpsilon[2]);
                    ReverseGamma3A(Para[6]*solEpsilon[2]);
                    ReverseGamma3ProbeUpdate();
                    if((T[1]-tbegin<=Ttol)&&(T[1]-tbegin>=-Ttol))
                        break;
                    solEpsilon[0]=solEpsilon[1];
                    solEpsilon[1]=solEpsilon[2];
                    solEpsilon[2]=solEpsilon[1]-(solEpsilon[1]-solEpsilon[0])*(T[1]-tbegin)/(T[1]-tbegin-solT);
                    solT=T[1]-tbegin;
                }
                ReverseUpdate();
            }
            break;
        }
        ReverseUpdate();
        if(crashed) {
            end=true;
            StartT=T[1];
            break;
        }
    }
    if(!end){
        StepRunOut=true;
        StartT=T[1];
    }
}
std::pair<double,SingleManeuver> Probe::ReverseGamma3SolveOrbit(Planet *Planets,int planetcount,int step, double epsilon,double tmin,double tmax){
    dataRefresh();
    maneuver.iteratorReach(2);
    maneuver.cut(tmin,tmax);
    stop=true;
    EndT=tmax;
    TCache=tmax;
    planetsOrbit=new SplineIterator[planetcount*6];
    for(int i=0;i<planetcount;i++) {
        for(int j=0;j<3;j++) {
            (planetsOrbit+6*i+j*2)->setSpline((Planets+i)->SplinePosition[j]);
            (planetsOrbit+6*i+j*2+1)->setSpline((Planets+i)->SplineVelocity[j]);
        }
    }
    rmin=10000000000;
    tGroup=new double[maneuver.length+2];
    StepRunOut=false;
    crashed=false;
    reach=false;
    tGroup[0]=tmin;
    for(int i=0;i<maneuver.length;i++) {
        maneuver.iteratorReach(1+i);
        tGroup[i+1]=maneuver.maneuverIterator->t;
    }
    tGroup[maneuver.length+1]=EndT;
    SplineIndex=1+maneuver.length;
    ReverseGamma3ISPRK4ForProbe(Planets,planetcount,step,-epsilon,tGroup[SplineIndex-1],tGroup[SplineIndex]);
    SplineIndex--;
    for(;(1<=SplineIndex)&&(!StepRunOut)&&(!crashed);SplineIndex--) {
        ReverseGamma3ISPRK4ForProbe(Planets,planetcount,ReverseStep,-epsilon,tGroup[SplineIndex-1],tGroup[SplineIndex]);
    }
    StartT=T[0];
    SingleManeuver returnManue;
    if(reach){
        double temporaryPos[3],temporaryVec[3],temporary[3],orbitVelocity{sqrt((Planets+6)->mass/(getDistanceToPlanet(6,0))+200000)},temporaryDv{0};
        for(int i=0;i<3;i++){
            temporaryPos[i]=*(Position+2*i)-(planetsOrbit+36+2*i)->value(T[0]);
            temporaryVec[i]=*(Velocity+2*i)-(planetsOrbit+36+2*i+1)->value(T[0]);
        }
        cross3(temporaryVec,temporaryPos,temporary);
        cross3(temporaryPos,temporary,returnManue.v);
        normalize(returnManue.v,returnManue.v,3);
        ndot(returnManue.v,returnManue.v,orbitVelocity,3);
        vminus(temporaryVec,returnManue.v,returnManue.v,3);
        returnManue.t=T[0];
        temporaryDv+=norm(returnManue.v,3,1);//
        for(auto i : maneuver.maneuver)
            temporaryDv+=norm(i.v,3,1);
        delete [] planetsOrbit;
        return std::pair<double,SingleManeuver>(temporaryDv,returnManue);
    }
    else{
        delete [] planetsOrbit;
        return std::pair<double,SingleManeuver>(rmin,returnManue);
    }
}
void Probe::ReverseGamma3SolveOrbit(Planet *Planets,int planetcount,int step, double epsilon,double tmin,double tmax,double *outQ,double *outP){
    dataRefresh();
    maneuver.cut(tmin,tmax);
    stop=false;
    EndT=tmax;
    TCache=tmax;
    planetsOrbit=new SplineIterator[planetcount*6];
    for(int i=0;i<planetcount;i++) {
        for(int j=0;j<3;j++) {
            (planetsOrbit+6*i+j*2)->setSpline((Planets+i)->SplinePosition[j]);
            (planetsOrbit+6*i+j*2+1)->setSpline((Planets+i)->SplineVelocity[j]);
        }
    }
    rmin=10000000000;
    tGroup=new double[maneuver.length+2];
    StepRunOut=false;
    crashed=false;
    reach=false;
    tGroup[0]=tmin;
    for(int i=0;i<maneuver.length;i++) {
        maneuver.iteratorReach(1+i);
        tGroup[i+1]=maneuver.maneuverIterator->t;
    }
    tGroup[maneuver.length+1]=EndT;
    SplineIndex=1+maneuver.length;
    ReverseGamma3ISPRK4ForProbe(Planets,planetcount,step,-epsilon,tGroup[SplineIndex-1],tGroup[SplineIndex]);
    SplineIndex--;
    for(;(1<=SplineIndex)&&(!StepRunOut)&&(!crashed);SplineIndex--) {
        ReverseGamma3ISPRK4ForProbe(Planets,planetcount,ReverseStep,-epsilon,tGroup[SplineIndex-1],tGroup[SplineIndex]);
    }
    for(int j=0;j<3;j++) {
        outQ[j]=Position[j*2];
        outP[j]=Velocity[j*2];
    }
    StartT=T[0];
    delete [] planetsOrbit;
}

Probe::~Probe() {
    if(SplinePosition){delete [] SplinePosition;}
    if(SplineVelocity){delete [] SplineVelocity;}
    if(Position){delete [] Position;}
    if(Velocity){delete [] Velocity;}
    if(plot){delete plot;}
    if(T){delete [] T;}
    if(p0){delete [] p0;}
    if(tGroup){delete [] tGroup;}
    if(tau){delete [] tau;}
    if(W){delete [] W;}
}
void Probe::GetPhaseSpaceInitialPosition(const double *QSource, const double *PSource) {
    for(int i=0;i<3;i++) {
        InitialPosition[i] = QSource[i];
        InitialVelocity[i] = PSource[i];
    }
}
void Probe::InitializationPhaseSpace(int step) {
    SplinePosition=new Spline[3*(maneuver.length+1)];
    SplineVelocity=new Spline[3*(maneuver.length+1)];
    history=0;
    Position = new double[3*step];
    Velocity = new double[3*step];
    for(int i=0;i<3;i++) {
        Position[i*(step)] = InitialPosition[i];
    }
    for(int i=0;i<3;i++) {
        Velocity[i*(step)] = InitialVelocity[i];
    }
    DataLength=step;
    SplineStart=0;
}
void Probe::InitializationAfterManeuverPhaseSpace(int number) {
    maneuver.iteratorReach(number);
    for(int i=0;i<3;i++) {
        Position[DataLength*i+SplineEnd+1] = Position[DataLength*i+SplineEnd];
        Velocity[DataLength*i+SplineEnd+1] = Velocity[DataLength*i+SplineEnd]+maneuver.maneuverIterator->v[i];
    }
    SplineStart=SplineEnd+1;
}
void Probe::ClearAcceleration() {
    for (double &i : acceleration)
        i =0;
}
void Probe::Cleardpdq() {
    for (double &i : dq)
        i =0;
    for (double &i : dp)
        i =0;
}
void Probe::GetSpline(double *XSource) {
    for(int i=0;i<3;i++) {
        SplinePosition[i+3*(SplineIndex-1)].GetSourceData(SplineDataLength,XSource,&Position[SplineStart+DataLength*i]);
        SplineVelocity[i+3*(SplineIndex-1)].GetSourceData(SplineDataLength,XSource,&Velocity[SplineStart+DataLength*i]);
    }
}
void Probe::Acceleration(Planet &Planets,double t,SplineIterator *pos) {
    double r[3],d,sqrtd,e;
    for(int i=0;i<3;i++)
        r[i]=QCache[i]-(*(pos+2*i)).value(t);
    d=vdot(r,r,3,1,1);
    sqrtd=sqrt(d);
    e=Planets.mass/(d*sqrtd);
    for(int i=0;i<3;i++)
        acceleration[i]-=e*r[i];
    if(!Planets.IsSphericalSymmetry)
        J2Coefficient(acceleration,r,Planets.RotationAxis,Planets.JJ2,d,sqrtd);
}
void Probe::ClearPositon() {
    if(Position) {delete [] Position;Position=nullptr;}
    if(Velocity) {delete [] Velocity;Velocity=nullptr;}
}
//得到某个飞行器受所有行星的力
void Probe::ProbeAcceleration(Planet *Planets,int planetcount,double t) {
    ClearAcceleration();
    for(int i=0;i<planetcount;i++) {
        Acceleration(Planets[i],t,planetsOrbit+i*6);
    }
}
//更新位置增量
void Probe::ProbeDqUpdate(int now,double hB) {
    vplus(Velocity+now+SplineStart,dp,PCache,3,DataLength);
    ndot(PCache,PCache,hB,3);
    vplus(dq,PCache,dq,3);
}
//更新动量增量
void Probe::ProbeDpUpdate(Planet *Planets,int planetcount,double t,int now,double hb) {
    vplus(Position+now+SplineStart,dq,QCache,3,DataLength);
    ProbeAcceleration(Planets,planetcount,t);
    ndot(acceleration,QCache,hb,3);
    vplus(dp,QCache,dp,3);
}
//更新位置
void Probe::ProbePositionUpdate(int next) {
    vplus(Position+next-1+SplineStart,dq,Position+next+SplineStart,3,DataLength,1,DataLength);
}
//更新动量
void Probe::ProbeVelocityUpdate(int next) {
    vplus(Velocity+next-1+SplineStart,dp,Velocity+next+SplineStart,3,DataLength,1,DataLength);
}
//对于飞行器的ISPRK4求解器
void Probe::ISPRK4ForProbe(Planet *Planets,int planetcount,int step, double h,double tmax,double tbegin) {
    double w = (2 + pow(2, 1.0 / 3) + 1 / pow(2, 1.0 / 3)) / 3,XSource[step];
    double v = 1 - 2 * w,b[4] = {w, v, w, 0},B[4] = {w / 2, (w + v) / 2, (w + v) / 2, w / 2},hB[4],hb[4],dt;
    bool end=false;
    XSource[0]=tbegin;
    if(SplineIndex==1)
        InitializationPhaseSpace(step);
    else
        InitializationAfterManeuverPhaseSpace(SplineIndex-1);
    for(int i=0;((i<step-1)&&(!end));i++) {
        Cleardpdq();
        XSource[i+1]=XSource[i]+h;
        if(XSource[i+1]>=tmax) {
            XSource[i+1]=tmax;
            h=tmax-XSource[i];
            SplineEnd=i+1+SplineStart;
            SplineDataLength=SplineEnd-SplineStart+1;
            end=true;
        }
        ndot(B,hB,h,4);
        ndot(b,hb,h,4);
        dt=0;
        for(int j=0;j<4;j++) {
            dt+=hB[j];
            ProbeDqUpdate(i,hB[j]);
            if(j!=4)
                ProbeDpUpdate(Planets,planetcount,XSource[i]+dt,i,hb[j]);
        }
        ProbePositionUpdate(i+1);
        ProbeVelocityUpdate(i+1);
    }
    if(!end){
        SplineEnd=step-1+SplineStart;
        SplineDataLength=SplineEnd-SplineStart+1;
        StepRunOut=true;
        EndT=XSource[step-1];
    }
    GetSpline(XSource);
}
double Probe::getE(int number,Planet *Planets,int planetcount) {
    double result{0};
    for(int i=0;i<3;i++)
        result+=Velocity[number+i*DataLength]*Velocity[number+i*DataLength];
    result/=2;

    for(int j=0;j<planetcount;j++) {
        double r[3],d,sqrtd;
        for(int i=0;i<3;i++)
            r[i]=Position[number+i*DataLength]-(*(planetsOrbit+j*6+i*2)).value(TCache);
        d=vdot(r,r,3,1,1);
        sqrtd=sqrt(d);
        result-=(Planets[j].mass)/sqrtd;
        if(!Planets[j].IsSphericalSymmetry){
            double c=vdot(Planets[j].RotationAxis,r,3,1,1),e=Planets[j].JJ2/(d*d*sqrtd);
            result-=e*0.5*(3*c*c-d);
        }
    }
    return result;
}
void Probe::SolveOrbit(Planet *Planets,int planetcount,int step, double h,double tmin,double tmax) { //为什么不refreshdata?
    TCache=tmin;
    planetsOrbit=new SplineIterator[planetcount*6];
    for(int i=0;i<planetcount;i++) {
        for(int j=0;j<3;j++) {
            (planetsOrbit+6*i+j*2)->setSpline((Planets+i)->SplinePosition[j]);
            (planetsOrbit+6*i+j*2+1)->setSpline((Planets+i)->SplineVelocity[j]);
        }
    }
    tGroup=new double[maneuver.length+2];
    StepRunOut=false;
    crashed=false;
    tGroup[0]=tmin;
    for(int i=0;i<maneuver.length;i++) {
        maneuver.iteratorReach(1+i);
        tGroup[i+1]=maneuver.maneuverIterator->t;
    }
    tGroup[maneuver.length+1]=tmax;
    EndT=tmax;
    SplineIndex=1;
    ISPRK4ForProbe(Planets,planetcount,step,h,tGroup[1],tGroup[0]);
    SplineIndex++;
    for(;(SplineIndex<=1+maneuver.length)&&(!StepRunOut);SplineIndex++) {
        ISPRK4ForProbe(Planets,planetcount,DataLength-SplineEnd-1,h,tGroup[SplineIndex],tGroup[SplineIndex-1]);
    }
    ClearPositon();
    delete [] planetsOrbit;
}
double Probe::value(int dimension,double t) {
    if((t<0)||(t>tGroup[maneuver.length+1])||(StepRunOut?(t>EndT):false))
        throw std::out_of_range{"shit"};
    for(int j=0;(j<maneuver.length+1);j++) {
        if(tGroup[history]<=t && t<=tGroup[history+1])
            return SplinePosition[3*history+dimension].value(t);
        if(t>tGroup[history+1])
            history++;
        if(t<tGroup[history])
            history--;
    }
    return 0;
}
double Probe::velocity(int dimension,double t) {
    if((t<0)||(t>tGroup[maneuver.length+1])||(StepRunOut?(t>EndT):false))
        return 0;
    for(int j=0;(j<maneuver.length+1);j++) {
        if(tGroup[history]<=t && t<=tGroup[history+1])
            return SplineVelocity[3*history+dimension].value(t);
        if(t>tGroup[history+1])
            history++;
        if(t<tGroup[history])
            history--;
    }
    return 0;
}
void Probe::Gamma3ClearPositon() {
    if(W) {delete [] W;W=nullptr;}//
    if(T) {delete [] T;T=nullptr;}
    if(Position) {delete [] Position;Position=nullptr;}
    if(Velocity) {delete [] Velocity;Velocity=nullptr;}
    if(p0) {delete [] p0;p0=nullptr;}
}
void Probe::Gamma3Cleardpdq() {
    for (double &i : dq)
        i =0;
    for (double &i : dp)
        i =0;
    dT=0;
    dp0=0;
    dW=0;
}
void Probe::Gamma3Acceleration(Planet &Planets,SplineIterator *pos) {
    double r[3],d,sqrtd,e,v[3],g,SingleDAcceleration[9],f;
    for(int i=0;i<3;i++){
        r[i]=QCache[i]-(*(pos+2*i)).value(TCache);
        v[i]=(*(pos+2*i+1)).value(TCache);
    }
    d=vdot(r,r,3,1,1);
    sqrtd=sqrt(d);
    e=Planets.mass/(d*sqrtd);
    f=e/d;
    for(int i=0;i<3;i++){
        g=e*r[i];
        acceleration[i]-=g;
        Vt+=g*v[i];
    }
    for(int i=0;i<3;i++) {
        for(int j=i;j<3;j++)
            SingleDAcceleration[i*3+j]=f*3*r[i]*r[j]-(i==j ? e : 0.0);
    }
    SingleDAcceleration[3]=SingleDAcceleration[1];
    SingleDAcceleration[6]=SingleDAcceleration[2];
    SingleDAcceleration[7]=SingleDAcceleration[5];
    mvdotminus(SingleDAcceleration,v,dAcceleration,3,3);
    vselfplus(SingleDAcceleration,MdAcceleration,9);
    if(!Planets.IsSphericalSymmetry)
        Gamma3J2Coefficient(acceleration,r,Planets.RotationAxis,Planets.JJ2,d,sqrtd,Vt,v);
}
void Probe::Gamma3ClearAcceleration() {
    for (double &i : acceleration)
        i =0;
    for (double &i : dAcceleration)
        i =0;
    for (double &i : MdAcceleration)
        i =0;
    for (double &i : VdAcceleration)
        i =0;
    Vt=0;
    //theW=0;
}
void Probe::Gamma3ProbeAcceleration(Planet *Planets,int planetcount) {
    Gamma3ClearAcceleration();
    for(int i=0;i<planetcount;i++) {
        Gamma3Acceleration(Planets[i],planetsOrbit+i*6);
    }
    for(int i=0;i<3;i++){
        dAcceleration[i]*=acceleration[i];
    }
    dAcceleration[0]+=dAcceleration[1]+dAcceleration[2];
}
void Probe::Gamma3W() {/////
    double force,temp;
    mvdot(MdAcceleration,PCache,VdAcceleration,3,3);
    for(int i=0;i<3;i++){
        VdAcceleration[i]*=acceleration[i];
    }
    theW=dAcceleration[0]+VdAcceleration[0]+VdAcceleration[1]+VdAcceleration[2];
    force=norm(acceleration,3,1);
    temp=force+(hmin*hmin)/(hmin+force);
    theW/=force*temp;
    theW*=(1-(hmin*hmin)/((hmin+force)*(hmin+force)));/////
}
void Probe::Gamma3D(Planet *Planets,int planetcount,int now,const double &h) {
    TCache=*(T+now+SplineStart)+dT;
    vplus(Position+now+SplineStart,dq,QCache,3,DataLength);
    Gamma3ProbeAcceleration(Planets,planetcount);
    vplus(Velocity+now+SplineStart,dp,PCache,3,DataLength);
    Gamma3W();
    dW+=theW*0.5*h;
    WCache=*(W+now+SplineStart)+dW;
    ndot(acceleration,QCache,h/WCache,3);
    vplus(dp,QCache,dp,3);
    dp0+=Vt/WCache;
    vplus(Velocity+now+SplineStart,dp,PCache,3,DataLength);
    Gamma3W();
    dW+=theW*0.5*h;
}
void Probe::Gamma3A(int now,const double &h) {
    WCache=*(W+now+SplineStart)+dW;
    P0Cache=*(p0+now+SplineStart)+dp0;
    vplus(Velocity+now+SplineStart,dp,PCache,3,DataLength);
    fq=h/WCache;
    ndot(PCache,PCache,fq,3);
    dT+=fq;
    vplus(dq,PCache,dq,3);
}
//更新动量
void Probe::Gamma3ProbeUpdate(int next) {
    vplus(Velocity+next-1+SplineStart,dp,Velocity+next+SplineStart,3,DataLength,1,DataLength);
    *(p0+next+SplineStart)=*(p0+next-1+SplineStart)+dp0;
    vplus(Position+next-1+SplineStart,dq,Position+next+SplineStart,3,DataLength,1,DataLength);
    *(T+next+SplineStart)=*(T+next-1+SplineStart)+dT;
    *(W+next+SplineStart)=*(W+next-1+SplineStart)+dW;
}
void Probe::FirstGamma3D(Planet *Planets,int planetcount,int now,const double &h) {
    TCache=*(T+now+SplineStart)+dT;
    vplus(Position+now+SplineStart,dq,QCache,3,DataLength);
    FirstGamma3ProbeAcceleration(Planets,planetcount);
    vplus(Velocity+now+SplineStart,dp,PCache,3,DataLength);
    Gamma3W();
    dW+=theW*0.5*h;
    WCache=*(W+now+SplineStart)+dW;
    ndot(acceleration,QCache,h/WCache,3);
    vplus(dp,QCache,dp,3);
    dp0+=Vt/WCache;
    vplus(Velocity+now+SplineStart,dp,PCache,3,DataLength);
    Gamma3W();
    dW+=theW*0.5*h;
}
void Probe::FirstGamma3ProbeAcceleration(Planet *Planets,int planetcount) {
    Gamma3ClearAcceleration();
    for(int i=0;i<planetcount;i++) {
        FirstGamma3Acceleration(Planets[i],planetsOrbit+i*6);
    }
    for(int i=0;i<3;i++){
        dAcceleration[i]*=acceleration[i];
    }
    dAcceleration[0]+=dAcceleration[1]+dAcceleration[2];
}
void Probe::FirstGamma3Acceleration(Planet &Planets,SplineIterator *pos) {
    double r[3],d,sqrtd,e,v[3],g,SingleDAcceleration[9],f;
    for(int i=0;i<3;i++){
        r[i]=QCache[i]-(*(pos+2*i)).value(TCache);
        v[i]=(*(pos+2*i+1)).value(TCache);
    }
    d=vdot(r,r,3,1,1);
    sqrtd=sqrt(d);
    e=Planets.mass/(d*sqrtd);
    f=e/d;
    if(sqrtd<Planets.crashRadius){crashed=true;}
    for(int i=0;i<3;i++){
        g=e*r[i];
        acceleration[i]-=g;
        Vt+=g*v[i];
    }
    for(int i=0;i<3;i++) {
        for(int j=i;j<3;j++)
            SingleDAcceleration[i*3+j]=f*3*r[i]*r[j]-(i==j ? e : 0.0);
    }
    SingleDAcceleration[3]=SingleDAcceleration[1];
    SingleDAcceleration[6]=SingleDAcceleration[2];
    SingleDAcceleration[7]=SingleDAcceleration[5];
    mvdotminus(SingleDAcceleration,v,dAcceleration,3,3);
    vselfplus(SingleDAcceleration,MdAcceleration,9);
    if(!Planets.IsSphericalSymmetry)
        Gamma3J2Coefficient(acceleration,r,Planets.RotationAxis,Planets.JJ2,d,sqrtd,Vt,v);
}
void Probe::Gamma3ISPRK4ForProbe(Planet *Planets,int planetcount,int step, double epsilon,double tmax,double tbegin) {
    double w{1/ (2 - pow(2, 1.0 / 3) )},v{1-2*w};
    double Para[7] = {w / 2,w, (1-w) / 2,v, (1-w) / 2,w,w / 2};
    double Ttol=0.5;
    bool end=false;
    if(SplineIndex==1)
        Gamma3InitializationPhaseSpace(step,tbegin,Planets,planetcount,epsilon);
    else
        Gamma3InitializationAfterManeuverPhaseSpace(SplineIndex-1,Planets,planetcount,epsilon);
    for(int i=0;i<step-1;i++) {
        Gamma3Cleardpdq();
        Gamma3A(i,Para[0]);
        FirstGamma3D(Planets,planetcount,i,Para[1]);
        Gamma3A(i,Para[2]);
        Gamma3D(Planets,planetcount,i,Para[3]);
        Gamma3A(i,Para[4]);
        Gamma3D(Planets,planetcount,i,Para[5]);
        Gamma3A(i,Para[6]);
        Gamma3ProbeUpdate(i+1);
        if(T[i+1+SplineStart]>=tmax) {
            SplineEnd=i+1+SplineStart;
            SplineDataLength=SplineEnd-SplineStart+1;
            end=true;
            if(maneuver.length+1!=SplineIndex){//这里有问题，原maneuver.length==SplineIndex
                double solEpsilon[3],solT{T[i+SplineStart]-tmax};
                solEpsilon[0]=0;
                solEpsilon[1]=1;
                solEpsilon[2]=solEpsilon[1]-(solEpsilon[1]-solEpsilon[0])*(T[i+1+SplineStart]-tmax)/(T[i+1+SplineStart]-tmax-solT);
                solT=T[i+1+SplineStart]-tmax;
                while(true) {
                    Gamma3Cleardpdq();
                    Gamma3A(i,Para[0]*solEpsilon[2]);
                    Gamma3D(Planets,planetcount,i,Para[1]*solEpsilon[2]);
                    Gamma3A(i,Para[2]*solEpsilon[2]);
                    Gamma3D(Planets,planetcount,i,Para[3]*solEpsilon[2]);
                    Gamma3A(i,Para[4]*solEpsilon[2]);
                    Gamma3D(Planets,planetcount,i,Para[5]*solEpsilon[2]);
                    Gamma3A(i,Para[6]*solEpsilon[2]);
                    Gamma3ProbeUpdate(i+1);
                    if((T[i+1+SplineStart]-tmax<=Ttol)&&(T[i+1+SplineStart]-tmax>=-Ttol)){
                        tGroup[SplineIndex]=T[i+1+SplineStart];
                        break;
                    }
                    solEpsilon[0]=solEpsilon[1];
                    solEpsilon[1]=solEpsilon[2];
                    solEpsilon[2]=solEpsilon[1]-(solEpsilon[1]-solEpsilon[0])*(T[i+1+SplineStart]-tmax)/(T[i+1+SplineStart]-tmax-solT);
                    solT=T[i+1+SplineStart]-tmax;
            }
            }
            break;
        }
        if(crashed) {
            SplineEnd=i+1+SplineStart;
            SplineDataLength=SplineEnd-SplineStart+1;
            end=true;
            EndT=T[i+1+SplineStart];
            break;
        }
        }
    if(!end){
        SplineEnd=step-1+SplineStart;
        SplineDataLength=SplineEnd-SplineStart+1;
        StepRunOut=true;
        EndT=T[DataLength-1];
    }
    GetSpline(&T[SplineStart]);
}
void Probe::Gamma3SolveOrbit(Planet *Planets,int planetcount,int step, double epsilon,double tmin, double tmax) {
    dataRefresh();
    maneuver.cut(tmin,tmax);
    TCache=tmin;
    planetsOrbit=new SplineIterator[planetcount*6];
    for(int i=0;i<planetcount;i++) {
        for(int j=0;j<3;j++) {
            (planetsOrbit+6*i+j*2)->setSpline((Planets+i)->SplinePosition[j]);
            (planetsOrbit+6*i+j*2+1)->setSpline((Planets+i)->SplineVelocity[j]);
        }
    }
    tGroup=new double[maneuver.length+2];
    StepRunOut=false;
    crashed=false;
    tGroup[0]=tmin;
    for(int i=0;i<maneuver.length;i++) {
        maneuver.iteratorReach(1+i);
        tGroup[i+1]=maneuver.maneuverIterator->t;
    }
    tGroup[maneuver.length+1]=tmax;
    EndT=tmax;
    SplineIndex=1;
    Gamma3ISPRK4ForProbe(Planets,planetcount,step,epsilon,tGroup[1],tGroup[0]);
    SplineIndex++;
    for(;(SplineIndex<=1+maneuver.length)&&(!StepRunOut)&&(!crashed);SplineIndex++) {
        Gamma3ISPRK4ForProbe(Planets,planetcount,DataLength-SplineEnd-1,epsilon,tGroup[SplineIndex],tGroup[SplineIndex-1]);
    }
    Gamma3ClearPositon();
    delete [] planetsOrbit;
}
void Probe::Gamma3InitializationPhaseSpace(int step,double tbegin,Planet *Planets,int planetcount,double epsilon) {
    SplinePosition=new Spline[3*(maneuver.length+1)];
    SplineVelocity=new Spline[3*(maneuver.length+1)];
    history=0;
    Position = new double[3*step];
    Velocity = new double[3*step];
    T=new double[step];
    p0=new double[step];
    W=new double[step];
    T[0]=tbegin;
    for(int i=0;i<3;i++) {
        Position[i*(step)] = InitialPosition[i];
    }
    for(int i=0;i<3;i++) {
        Velocity[i*(step)] = InitialVelocity[i];
    }
    DataLength=step;
    SplineStart=0;
    p0[0]=-getE(0,Planets,planetcount);
    W[0]=getW(0,Planets,planetcount)*epsilon;
}
void Probe::Gamma3InitializationAfterManeuverPhaseSpace(int number,Planet *Planets,int planetcount,double epsilon) {
    maneuver.iteratorReach(number);
    T[SplineEnd+1]=T[SplineEnd];
    for(int i=0;i<3;i++) {
        Position[DataLength*i+SplineEnd+1] = Position[DataLength*i+SplineEnd];
        Velocity[DataLength*i+SplineEnd+1] = Velocity[DataLength*i+SplineEnd]+maneuver.maneuverIterator->v[i];
    }
    SplineStart=SplineEnd+1;
    TCache=T[SplineEnd+1];
    p0[SplineEnd+1]=-getE(SplineEnd+1,Planets,planetcount);
    W[SplineEnd+1]=getW(SplineEnd+1,Planets,planetcount)*epsilon;
}
double Probe::getW(int number,Planet *Planets,int planetcount) {/////
    double i[3]={0,0,0},force;
    TCache=T[number];
    vplus(Position+number,i,QCache,3,DataLength);
    Gamma3ClearAcceleration();
    Gamma3ProbeAcceleration(Planets,planetcount);
    force=norm(acceleration,3,1);
    return force+(hmin*hmin)/(hmin+force);
}
void Probe::dataRefresh() {
    if(tGroup) {delete [] tGroup;tGroup=nullptr;}
    if(W) {delete [] W;W=nullptr;}//
    if(T) {delete [] T;T=nullptr;}
    if(Position) {delete [] Position;Position=nullptr;}
    if(Velocity) {delete [] Velocity;Velocity=nullptr;}
    if(tau) {delete [] tau;tau=nullptr;}
    if(p0) {delete [] p0;p0=nullptr;}
    if(SplinePosition) {delete [] SplinePosition;SplinePosition=nullptr;}
    if(SplineVelocity) {delete [] SplineVelocity;SplineVelocity=nullptr;}
}
void Gamma3J2Coefficient(double *f,double *r,double *j,const double &  j2,const double & d,const double & sqrtd,double &Vt,double *vp) {
    double c=vdot(j,r,3,1,1),e=j2/(d*d*sqrtd),a=3*c*e,b=3*e*(1-5*c*c/d)*0.5,g;
    for(int i=0;i<3;i++){
        g=a*j[i]+b*r[i];
        Vt+=g*vp[i];
        f[i]+=g;
    }
}
