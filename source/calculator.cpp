#include "calculator.h"
#include "reference.h"
Calculator::Calculator(std::string iLocation,int iPlanetStep,double iPlanetH)
    :Location(iLocation)
    ,PlanetStep(iPlanetStep)
    ,PlanetH(iPlanetH)
{
}
void Calculator::resetLocation(std::string iLocation) {
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
    probes.push_back(OriginalProbe);
    probeIndex.insert(std::pair<std::string,Probe*> (probes.at(0).name,&(probes.at(0))));
}
void Calculator::loadPlanet() {
    LoadAll(&Planets[0],planetcount,Location);
}
void Calculator::savePlanet() {
    ISPRK4ForPlanets(Planets,planetcount,PlanetStep,PlanetH);
    SaveAll(&Planets[0],planetcount,Location);
}
void Calculator::solveProbe(Probe &probe,int step,double h,double tmax) {
    probe.SolveOrbit(Planets,planetcount,step,h,tmax);
}
void Calculator::NewSolveProbe(Probe &probe,int step,double epsilon,double tmax) {
    probe.NewSolveOrbit(Planets,planetcount,step,epsilon,tmax);
}
void Calculator::Gamma3SolveProbe(Probe &probe,int step,double epsilon,double tmax) {
    probe.Gamma3SolveOrbit(Planets,planetcount,step,epsilon,tmax);
}
void Calculator::probeRelativeOrbit(int step,int maxwide,int i,int j,double tmax,Probe &probe,double minCosTheta,referenceOrigin origin,referenceRotate rotate) {
    ProbeCoordinatesTransform <Planet,Planet,Probe> Orbit_1(Planets[i],Planets[j],probe,origin,rotate);
    std::function<double (double)> fx = [&](double t)->double{return Orbit_1.value(0,t);};
    std::function<double (double)> fy = [&](double t)->double{return Orbit_1.value(1,t);};
    std::function<double (double)> fz = [&](double t)->double{return Orbit_1.value(2,t);};
    delete probe.probeplot;
    probe.probeplot=new GlPlot3D(fx,fy,fz,tmax,maxwide,step,minCosTheta);
    probe.probeplot->plot();
}
GLfloat *Calculator::getVertices(std::string name) {
    probeIndexIter=probeIndex.find(name);
    return probeIndexIter->second->probeplot->Vbo;
}
GLfloat *Calculator::getNormedT(std::string name) {
    probeIndexIter=probeIndex.find(name);
    return probeIndexIter->second->probeplot->normedT;
}
int Calculator::getStep(std::string name) {
    probeIndexIter=probeIndex.find(name);
    return (*(probeIndexIter->second->probeplot)).Step();
}
Probe *Calculator::getProbe(std::string name) {
    probeIndexIter=probeIndex.find(name);
    return probeIndexIter->second;
}
std::vector<double> Calculator::getScaleAndOrigin(std::string name) {
    probeIndexIter=probeIndex.find(name);
    return probeIndexIter->second->probeplot->getScaleAndOrigin();
}
GlPlot3D *Calculator::getPlot3D(std::string name) {
    probeIndexIter=probeIndex.find(name);
    return probeIndexIter->second->probeplot;
}

int Calculator::planetToIndex(std::string name) {
    return planetIndex.find(name)->second;
}

std::string Calculator::indexToPlanets(int index) {
    return (*(Planets+index)).name;
}

int Calculator::getPlanetcount() {
    return planetcount;
}
