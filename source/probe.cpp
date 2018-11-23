#include "probe.h"
#include <iostream>

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
void Probe::Acceleration(Planet &Planets,double t) {
    double r[3],d,sqrtd,e;
    for(int i=0;i<3;i++)
        r[i]=QCache[i]-Planets.SplinePosition[i].value(t);
    d=vdot(r,r,3,1,1);
    sqrtd=sqrt(d);
    e=Planets.mass/(d*sqrtd);
    for(int i=0;i<3;i++)
        acceleration[i]-=e*r[i];
    if(!Planets.IsSphericalSymmetry)
        J2Coefficient(acceleration,r,Planets.RotationAxis,Planets.JJ2,d,sqrtd);
}
void Probe::ClearPositon() {
    delete []Position;
    delete []Velocity;
}
//得到某个飞行器受所有行星的力
void Probe::ProbeAcceleration(Planet *Planets,int planetcount,double t) {
    ClearAcceleration();
    for(int i=0;i<planetcount;i++) {
        Acceleration(Planets[i],t);
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
            r[i]=Position[number+i*DataLength]-Planets[j].SplinePosition[i].value(TCache);
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
void Probe::NewInitializationPhaseSpace(int step,double tbegin,Planet *Planets,int planetcount) {
    SplinePosition=new Spline[3*(maneuver.length+1)];
    SplineVelocity=new Spline[3*(maneuver.length+1)];
    history=0;
    Position = new double[3*step];
    Velocity = new double[3*step];
    T=new double[step];
    p0=new double[step];
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
}
void Probe::NewInitializationAfterManeuverPhaseSpace(int number,Planet *Planets,int planetcount) {
    maneuver.iteratorReach(number);
    T[SplineEnd+1]=T[SplineEnd];
    for(int i=0;i<3;i++) {
        Position[DataLength*i+SplineEnd+1] = Position[DataLength*i+SplineEnd];
        Velocity[DataLength*i+SplineEnd+1] = Velocity[DataLength*i+SplineEnd]+maneuver.maneuverIterator->v[i];
    }
    SplineStart=SplineEnd+1;
    p0[SplineEnd+1]=-getE(SplineEnd+1,Planets,planetcount);
}
void Probe::NewAcceleration(Planet &Planets) {
    double r[3],d,sqrtd,e,v[3],g;
    for(int i=0;i<3;i++){
        r[i]=QCache[i]-Planets.SplinePosition[i].value(TCache);
        v[i]=Planets.SplineVelocity[i].value(TCache);
    }
    d=vdot(r,r,3,1,1);
    sqrtd=sqrt(d);
    e=Planets.mass/(d*sqrtd);
    V+=e*d;
    for(int i=0;i<3;i++){
        g=e*r[i];
        acceleration[i]-=g;
        Vt-=g*v[i];
    }
    if(!Planets.IsSphericalSymmetry)
        NewJ2Coefficient(acceleration,r,Planets.RotationAxis,Planets.JJ2,d,sqrtd,V,Vt,v);
}
void Probe::NewClearAcceleration() {
    for (double &i : acceleration)
        i =0;
    V=0;
    Vt=0;
}
//得到某个飞行器受所有行星的力
void Probe::NewProbeAcceleration(Planet *Planets,int planetcount) {
    NewClearAcceleration();
    for(int i=0;i<planetcount;i++) {
        NewAcceleration(Planets[i]);
    }
}
//更新位置增量
void Probe::NewProbeDqUpdate(int now,double B,const double &epsilon) {
    P0Cache=*(p0+now+SplineStart)+dp0;
    vplus(Velocity+now+SplineStart,dp,PCache,3,DataLength);
    fq=epsilon/(P0Cache+vdot(PCache,PCache,3)*0.5)*B;
    ndot(PCache,PCache,fq,3);
    dT+=fq;
    vplus(dq,PCache,dq,3);
}
//更新动量增量
void Probe::NewProbeDpUpdate(Planet *Planets,int planetcount,int now,double b,const double &epsilon) {
    TCache=*(T+now+SplineStart)+dT;
    vplus(Position+now+SplineStart,dq,QCache,3,DataLength);
    NewProbeAcceleration(Planets,planetcount);
    fp=epsilon/V*b;
    ndot(acceleration,QCache,fp,3);
    dp0-=fp*Vt;
    vplus(dp,QCache,dp,3);
}
//更新位置
void Probe::NewProbePositionUpdate(int next) {
    vplus(Position+next-1+SplineStart,dq,Position+next+SplineStart,3,DataLength,1,DataLength);
    *(T+next+SplineStart)=*(T+next-1+SplineStart)+dT;
}
//更新动量
void Probe::NewProbeVelocityUpdate(int next) {
    vplus(Velocity+next-1+SplineStart,dp,Velocity+next+SplineStart,3,DataLength,1,DataLength);
    *(p0+next+SplineStart)=*(p0+next-1+SplineStart)+dp0;
}
void Probe::NewCleardpdq() {
    for (double &i : dq)
        i =0;
    for (double &i : dp)
        i =0;
    dT=0;
    dp0=0;
}
void Probe::NewClearPositon() {
    delete []Position;
    delete []Velocity;
    delete []T;
    delete []p0;
}
//对于飞行器的ISPRK4求解器
void Probe::NewISPRK4ForProbe(Planet *Planets,int planetcount,int step, double epsilon,double tmax,double tbegin) {
    double w = (2 + pow(2, 1.0 / 3) + 1 / pow(2, 1.0 / 3)) / 3;
    double v = 1 - 2 * w,b[4] = {w, v, w, 0},B[4] = {w / 2, (w + v) / 2, (w + v) / 2, w / 2};
    double Ttol=0.5;
    bool end=false;
    if(SplineIndex==1)
        NewInitializationPhaseSpace(step,tbegin,Planets,planetcount);
    else
        NewInitializationAfterManeuverPhaseSpace(SplineIndex-1,Planets,planetcount);
    for(int i=0;i<step-1;i++) {
        NewCleardpdq();
        for(int j=0;j<4;j++) {
            NewProbeDqUpdate(i,B[j],epsilon);
            if(j!=4)
                NewProbeDpUpdate(Planets,planetcount,i,b[j],epsilon);
        }
        NewProbePositionUpdate(i+1);
        NewProbeVelocityUpdate(i+1);
        if(T[i+1+SplineStart]>=tmax) {
            SplineEnd=i+1+SplineStart;
            SplineDataLength=SplineEnd-SplineStart+1;
            end=true;
            double solEpsilon[3],solT{T[i+SplineStart]-tmax};
            solEpsilon[0]=0;
            solEpsilon[1]=epsilon;
            solEpsilon[2]=solEpsilon[1]-(solEpsilon[1]-solEpsilon[0])*(T[i+1+SplineStart]-tmax)/(T[i+1+SplineStart]-tmax-solT);
            solT=T[i+1+SplineStart]-tmax;
            while(true) {
                NewCleardpdq();
                for(int j=0;j<4;j++) {
                    NewProbeDqUpdate(i,B[j],solEpsilon[2]);
                    if(j!=4)
                        NewProbeDpUpdate(Planets,planetcount,i,b[j],solEpsilon[2]);
                }
                NewProbePositionUpdate(i+1);
                NewProbeVelocityUpdate(i+1);
                if((T[i+1+SplineStart]-tmax<=Ttol)&&(T[i+1+SplineStart]-tmax>=-Ttol))
                    break;
                solEpsilon[0]=solEpsilon[1];
                solEpsilon[1]=solEpsilon[2];
                solEpsilon[2]=solEpsilon[1]-(solEpsilon[1]-solEpsilon[0])*(T[i+1+SplineStart]-tmax)/(T[i+1+SplineStart]-tmax-solT);
                solT=T[i+1+SplineStart]-tmax;
            }
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
void Probe::NewSolveOrbit(Planet *Planets,int planetcount,int step, double epsilon,double tmax) {
    tGroup=new double[maneuver.length+2];
    StepRunOut=false;
    tGroup[0]=0;
    for(int i=0;i<maneuver.length;i++) {
        maneuver.iteratorReach(1+i);
        tGroup[i+1]=maneuver.maneuverIterator->t;
    }
    tGroup[maneuver.length+1]=tmax;
    SplineIndex=1;
    NewISPRK4ForProbe(Planets,planetcount,step,epsilon,tGroup[1],tGroup[0]);
    SplineIndex++;
    for(;(SplineIndex<=1+maneuver.length)&&(!StepRunOut);SplineIndex++) {
        NewISPRK4ForProbe(Planets,planetcount,DataLength-SplineEnd-1,epsilon,tGroup[SplineIndex],tGroup[SplineIndex-1]);
    }
    NewClearPositon();
}
void Probe::SolveOrbit(Planet *Planets,int planetcount,int step, double h,double tmax) {
    tGroup=new double[maneuver.length+2];
    StepRunOut=false;
    tGroup[0]=0;
    for(int i=0;i<maneuver.length;i++) {
        maneuver.iteratorReach(1+i);
        tGroup[i+1]=maneuver.maneuverIterator->t;
    }
    tGroup[maneuver.length+1]=tmax;
    SplineIndex=1;
    ISPRK4ForProbe(Planets,planetcount,step,h,tGroup[1],tGroup[0]);
    SplineIndex++;
    for(;(SplineIndex<=1+maneuver.length)&&(!StepRunOut);SplineIndex++) {
        ISPRK4ForProbe(Planets,planetcount,DataLength-SplineEnd-1,h,tGroup[SplineIndex],tGroup[SplineIndex-1]);
    }
    ClearPositon();
}
double Probe::value(int dimension,double t) {
    if((t<0)||(t>tGroup[maneuver.length+1])||(StepRunOut?(t>EndT):false))
        throw std::out_of_range{"time out"};
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
    delete []Position;
    delete []Velocity;
    delete []T;
    delete []p0;
    delete []W;
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
void Probe::Gamma3Acceleration(Planet &Planets) {
    double r[3],d,sqrtd,e,v[3],g,SingleDAcceleration[9],f;
    for(int i=0;i<3;i++){
        r[i]=QCache[i]-Planets.SplinePosition[i].value(TCache);
        v[i]=Planets.SplineVelocity[i].value(TCache);
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
    theW=0;
}
void Probe::Gamma3ProbeAcceleration(Planet *Planets,int planetcount) {
    Gamma3ClearAcceleration();
    for(int i=0;i<planetcount;i++) {
        Gamma3Acceleration(Planets[i]);
    }
    for(int i=0;i<3;i++){
        dAcceleration[i]*=acceleration[i];
    }
    dAcceleration[0]+=dAcceleration[1]+dAcceleration[2];
}
void Probe::Gamma3W() {
    mvdot(MdAcceleration,PCache,VdAcceleration,3,3);
    for(int i=0;i<3;i++){
        VdAcceleration[i]*=acceleration[i];
    }
    theW=dAcceleration[0]+VdAcceleration[0]+VdAcceleration[1]+VdAcceleration[2];
    theW/=vdot(acceleration,acceleration,3);
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
        Gamma3D(Planets,planetcount,i,Para[1]);
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
                if((T[i+1+SplineStart]-tmax<=Ttol)&&(T[i+1+SplineStart]-tmax>=-Ttol))
                    break;
                solEpsilon[0]=solEpsilon[1];
                solEpsilon[1]=solEpsilon[2];
                solEpsilon[2]=solEpsilon[1]-(solEpsilon[1]-solEpsilon[0])*(T[i+1+SplineStart]-tmax)/(T[i+1+SplineStart]-tmax-solT);
                solT=T[i+1+SplineStart]-tmax;
            }
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
void Probe::Gamma3SolveOrbit(Planet *Planets,int planetcount,int step, double epsilon,double tmax) {
    tGroup=new double[maneuver.length+2];
    StepRunOut=false;
    tGroup[0]=0;
    for(int i=0;i<maneuver.length;i++) {
        maneuver.iteratorReach(1+i);
        tGroup[i+1]=maneuver.maneuverIterator->t;
    }
    tGroup[maneuver.length+1]=tmax;
    SplineIndex=1;
    Gamma3ISPRK4ForProbe(Planets,planetcount,step,epsilon,tGroup[1],tGroup[0]);
    SplineIndex++;
    for(;(SplineIndex<=1+maneuver.length)&&(!StepRunOut);SplineIndex++) {
        Gamma3ISPRK4ForProbe(Planets,planetcount,DataLength-SplineEnd-1,epsilon,tGroup[SplineIndex],tGroup[SplineIndex-1]);
    }
    Gamma3ClearPositon();
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
    p0[SplineEnd+1]=-getE(SplineEnd+1,Planets,planetcount);
    W[SplineEnd+1]=getW(SplineEnd+1,Planets,planetcount)*epsilon;
}
double Probe::getW(int number,Planet *Planets,int planetcount) {
    double i[3]={0,0,0};
    TCache=T[number];
    vplus(Position+number,i,QCache,3,DataLength);
    Gamma3ClearAcceleration();
    Gamma3ProbeAcceleration(Planets,planetcount);
    return norm(acceleration,3,1);
}
void Gamma3J2Coefficient(double *f,double *r,double *j, double j2, double d,double sqrtd,double &Vt,double *vp) {
    double c=vdot(j,r,3,1,1),e=j2/(d*d*sqrtd),a=3*c*e,b=3*e*(1-5*c*c/d)*0.5,g;
    for(int i=0;i<3;i++){
        g=a*j[i]+b*r[i];
        Vt+=g*vp[i];
        f[i]+=g;
    }
}
