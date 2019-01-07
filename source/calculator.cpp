#include "calculator.h"
#include "reference.h"
#include <iostream>
#include <utility>
/*-------------------------------S&L----------------------------------------*/
//保存所有行星数据
void Calculator::SaveAll(){
    std::string str=Location+R"(\splines.dat)";
    const char *location=str.data();
    remove(location);
    std::fstream outdata;
    outdata.open(str, std::ios::out | std::ios::binary);
    if (!outdata.is_open())
        throw std::runtime_error{"无法保存行星轨道"};
    outdata.write((const char *)&PlanetStep, sizeof(int));
    outdata.write((const char *)&PlanetH, sizeof(double));
    for(int i=0;i<planetcount;i++)
        Planets[i].save(outdata);
    outdata.close();
}
//读取所有行星数据
void Calculator::LoadAll(){
    std::fstream indata;
    indata.open(Location+R"(\splines.dat)", std::ios::in | std::ios::binary);
    if (!indata.is_open())
        throw std::runtime_error{"无法读取行星轨道（您保存过吗）"};
    indata.read((char *)&PlanetStep, sizeof(int));
    indata.read((char *)&PlanetH, sizeof(double));
    for(int i=0;i<planetcount;i++)
        Planets[i].load(indata);
    indata.close();
}
/*-------------------------------S&L----------------------------------------*/
Calculator::Calculator(const std::string & iLocation)
    :Location(iLocation)
{
}
void Calculator::setPlanetSolve(const int & iPlanetStep,const double & iPlanetH) {
    PlanetStep=iPlanetStep;
    PlanetH=iPlanetH;
}
void Calculator::resetLocation(const std::string & iLocation) {
    Location=iLocation;
}
void Calculator::getData() {
    planetcount=GetPlanetCount(Location);
    Planets=new Planet[planetcount];
    InitializationPlanets(&Planets[0],planetcount,CenterOfMass,Location);
    InitializationOriginalProbe(CenterOfMass,OriginalProbe);
    OriginalProbe.name="OriginalProbe";
    for(int i=0;i<planetcount;i++){
        planetIndex.insert(std::pair<std::string,int> ((*(Planets+i)).name,i));
    }
    probes.insert(std::pair<std::string,Probe> (OriginalProbe.name,OriginalProbe));
}
void Calculator::loadPlanet() {
    LoadAll();
}
void Calculator::savePlanet() {
    ISPRK4ForPlanets(Planets,planetcount,PlanetStep,PlanetH);
    SaveAll();
}
void Calculator::solveProbe(Probe &probe,const int & step,const double & h,const double & tmin,const double & tmax) {
    probe.SolveOrbit(Planets,planetcount,step,h,tmin,tmax);
}
void Calculator::Gamma3SolveProbe(Probe &probe,const int & step,const double & epsilon,const double & tmin,const double & tmax) {
    probe.Gamma3SolveOrbit(Planets,planetcount,step,epsilon,tmin,tmax);
}
std::pair<double,SingleManeuver> Calculator::ReverseGamma3SolveProbe(Probe &probe,const int & step,const double & epsilon,const double & tmin,const double & tmax) {
    return probe.ReverseGamma3SolveOrbit(Planets,planetcount,step,epsilon,tmin,tmax);
}
void Calculator::ReverseGamma3SolveProbe(Probe &probe,const int & step,const double & epsilon,const double & tmin,const double & tmax,double *outQ,double *outP) {
    probe.ReverseGamma3SolveOrbit(Planets,planetcount,step,epsilon,tmin,tmax,outQ,outP);
}
void Calculator::probeRelativeOrbit(const int & step,const int & maxwide,const std::string & p1,const std::string & p2,const std::string & p3,const double & minCosTheta,referenceOrigin origin,referenceRotate rotate,double tmin,double tmax) {
    ProbeCoordinatesTransform Orbit_1(getAbstractPoint(p1),getAbstractPoint(p2),getAbstractPoint(p3),origin,rotate,tmin);
    std::function<double (double)> fx = [&](double t)->double{return Orbit_1.value(0,t);};
    std::function<double (double)> fy = [&](double t)->double{return Orbit_1.value(1,t);};
    std::function<double (double)> fz = [&](double t)->double{return Orbit_1.value(2,t);};
    std::vector<double> TGroup;
    if(!tmax) {
        switch (getType(p3)) {
        case probe:
            TGroup.push_back(tmin);
            for(int i=1;i<static_cast<Probe*>(getAbstractPoint(p3))->maneuver.length+1;i++)
                TGroup.push_back(static_cast<Probe*>(getAbstractPoint(p3))->tGroup[i]);
            TGroup.push_back(static_cast<Probe*>(getAbstractPoint(p3))->EndT);
            break;
        case planet:
            TGroup.push_back(tmin);
            TGroup.push_back((PlanetStep)*PlanetH);
            break;
        }
    }
    if(getAbstractPoint(p3)->plot) {delete getAbstractPoint(p3)->plot;getAbstractPoint(p3)->plot=nullptr;}
    getAbstractPoint(p3)->plot=new GlPlot3D(fx,fy,fz,maxwide,step,minCosTheta,TGroup);
    getAbstractPoint(p3)->plot->plot();
}
GLfloat *Calculator::getVertices(const std::string & name) {
    return getAbstractPoint(name)->plot->Vbo;
}
GLfloat *Calculator::getNormedT(const std::string & name) {
    return getAbstractPoint(name)->plot->normedT;
}
int Calculator::getStep(const std::string & name) {
    return (*(getAbstractPoint(name)->plot)).Step();
}
Probe *Calculator::getProbe(const std::string & name) {
    probesIter=probes.find(name);
    return &(probesIter->second);
}
Probe *Calculator::getOriginProbe() {
    probesIter=probes.find("OriginalProbe");
    return &(probesIter->second);
}
std::vector<double> Calculator::getScaleAndOrigin(const std::string & name) {
    return getAbstractPoint(name)->plot->getScaleAndOrigin();
}
GlPlot3D *Calculator::getPlot3D(const std::string & name) {
    return getAbstractPoint(name)->plot;
}

int Calculator::planetToIndex(const std::string & name) {
    return planetIndex.find(name)->second;
}

const std::string & Calculator::indexToPlanets(const int & index) {
    return (*(Planets+index)).name;
}

int Calculator::getPlanetcount() {
    return planetcount;
}
const std::string & Calculator::getLocation() {
    return Location;
}
Calculator::~Calculator() {
    if(Planets){delete [] Planets;}
}
AbstractPoint* Calculator::getAbstractPoint(const std::string & name) {
    probesIter=probes.find(name);
    if(probesIter!=probes.end()){return &(probesIter->second);}
    std::map<std::string,int>::iterator planetIndexIter;
    planetIndexIter=planetIndex.find(name);
    if(planetIndexIter!=planetIndex.end()){return Planets+planetIndexIter->second;}
    throw std::runtime_error{"找不到点基类"};
}
PointType Calculator::getType(const std::string & name) {
    probesIter=probes.find(name);
    if(probesIter!=probes.end()){return probe;}
    std::map<std::string,int>::iterator planetIndexIter;
    planetIndexIter=planetIndex.find(name);
    if(planetIndexIter!=planetIndex.end()){return planet;}
    throw std::runtime_error{"找不到点基类"};
}
void Calculator::newProbe(const std::string & name) {
    probesMutex.lock();
    probesIter=probes.emplace(std::piecewise_construct,std::make_tuple(name),std::make_tuple()).first;
    probesIter->second.name=name;
    probesMutex.unlock();
}
void Calculator::deleteProbe(const std::string & name) {
    probesMutex.lock();
    probes.erase(name);
    probesMutex.unlock();
}
