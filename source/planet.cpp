#include "planet.h"
#include <iostream>
#include "spline.h"
#include <fstream>
#include <stdexcept>
//得到行星数量(pcount.txt)
Planet::~Planet() {
    if(!Position){delete [] Position;}
    if(!Velocity){delete [] Velocity;}
}
int GetPlanetCount(std::string Location) {
    int planetcount;
    std::fstream indata;
    indata.open(Location+R"(\pcount.txt)",std::ios::in);
    if(!indata.is_open ())
        throw std::runtime_error{"打开pcount.txt失败"};
    indata>>planetcount;
    indata.close();
    return planetcount;
}
void Planet::GetMaxPlanetNameLength(int length) {
    MaxPlanetNameLength=length;
}
void Planet::GetName(std::fstream &fs) {
    fs>>name;
}
void Planet::GetParameter(std::fstream &fs) {
    fs>>mass;
    fs>>j2;
    fs>>radius;
    fs>>crashRadius;
    J2=j2*radius*radius;
    JJ2=J2*mass;
    IsSphericalSymmetry= j2 == 0;
}
void Planet::GetRawInformation(std::fstream &fs) {
    for (double &i : RawPosition)
        fs >> i;
    for (double &i : RawVelocity)
        fs >> i;
}
void Planet::GetRotationAxis(std::fstream &fs)  {
    double RawRotationAxis[3];
    for (double &i : RawRotationAxis)
        fs >> i;
    normalize(RawRotationAxis,RotationAxis,3);
}
void Planet::InitializationPhaseSpace( int step,int position) {
    Position = new double[3*step];
    Velocity = new double[3*step];
    for(int i=0;i<3;i++) {
        Position[i*step+position] = InitialPosition[i];
    }
    for(int i=0;i<3;i++) {
        Velocity[i*step+position] = InitialVelocity[i];
    }
    DataLength=step;
}
void Planet::ClearAcceleration() {
    for (double &i : acceleration)
        i =0;
}
void Planet::Cleardpdq() {
    for (double &i : dq)
        i =0;
    for (double &i : dp)
        i =0;
}
void Planet::GetSpline(double *XSource) {
    for(int i=0;i<3;i++) {
        SplinePosition[i].GetSourceData(DataLength,XSource,&Position[DataLength*i]);
        SplineVelocity[i].GetSourceData(DataLength,XSource,&Velocity[DataLength*i]);
    }
}
void gravity(Planet &Planets_1,Planet &Planets_2) {
    double r[3],d,sqrtd,e;
    for(int i=0;i<3;i++)
        r[i]=Planets_1.QCache[i]-Planets_2.QCache[i];
    d=vdot(r,r,3,1,1);
    sqrtd=sqrt(d);
    e=Planets_2.mass/(d*sqrtd);
    for(int i=0;i<3;i++)
        Planets_1.acceleration[i]-=e*r[i];
    if(!Planets_2.IsSphericalSymmetry)
        J2Coefficient(Planets_1.acceleration,r,Planets_2.RotationAxis,Planets_2.J2*Planets_2.mass,d,sqrtd);
    if(!Planets_1.IsSphericalSymmetry)
        J2Coefficient(Planets_1.acceleration,r,Planets_1.RotationAxis,Planets_1.J2*Planets_2.mass,d,sqrtd);
}
void Planet::ClearPositon() {
    for(int i=0;i<3;i++) {
        QCache[i]=Position[i*DataLength];
        PCache[i]=Velocity[i*DataLength];
    }
    delete []Position;
    delete []Velocity;
}
void Planet::save(std::fstream &fs) {
    for(int i=0;i<3;i++) {
        SplinePosition[i].save(fs);
        SplineVelocity[i].save(fs);
    }
}
void Planet::load(std::fstream &fs) {
    for(int i=0;i<3;i++) {
        SplinePosition[i].load(fs);
        SplineVelocity[i].load(fs);
    }
}
double Planet::value(int dimension,double t) {
    return SplinePosition[dimension].value(t);
}
double Planet::velocity(int dimension,double t) {
    return SplineVelocity[dimension].value(t);
}

