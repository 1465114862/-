#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "planet.h"
#include "probe.h"
#include "plot.h"
#include "clockfortest.h"
#include "intidata.h"
#include "ipsk4.h"
#include "reference.h"
#include <map>
#include <QMutex>

enum PointType{planet,probe};

class Calculator {
public:
    ClockForTest t{};
    Calculator(const std::string & iLocation);
    AbstractPoint* getAbstractPoint(const std::string & name);
    PointType getType(const std::string & name);
    void setPlanetSolve(const int & iPlanetStep,const double & iPlanetH);
    void getData();//启动时必调用，行星数据
    void loadPlanet();//读取行星
    void newProbe(const std::string & name);
    void deleteProbe(const std::string & name);
    void savePlanet();//保存行星
    void probeRelativeOrbit(const int & step,const int & maxwide,const std::string & p1,const std::string & p2,const std::string & p3,const double & maxCosTheta,referenceOrigin origin,referenceRotate rotate,double tmin=0,double tmax=0);//重新换系，得到double数据点
    void solveProbe(Probe &probe,const int & step,const double & h,const double & tmin,const double & tmax);//重新计算
    void Gamma3SolveProbe(Probe &probe,const int & step,const double & epsilon,const double & tmin,const double & tmax);//重新计算
    std::pair<double,SingleManeuver> ReverseGamma3SolveProbe(Probe &probe,const int & step,const double & epsilon,const double & tmin,const double & tmax);
    void ReverseGamma3SolveProbe(Probe &probe,const int & step,const double & epsilon,const double & tmin,const double & tmax,double *outQ,double *outP);
    void resetLocation(const std::string & iLocation);//文件目录设置
    const std::string & getLocation();//文件目录设置
    GLfloat *getVertices(const std::string & name);//得到顶点指针
    GLfloat *getNormedT(const std::string & name);//得到时间
    Probe *getProbe(const std::string & name);//调用飞行器
    Probe *getOriginProbe();
    int getStep(const std::string & name);//得到画图步数
    std::vector<double> getScaleAndOrigin(const std::string & name);//得到推荐缩放以及原点,0是缩放，123原点
    GlPlot3D *getPlot3D(const std::string & name);//调用飞行器
    int planetToIndex(const std::string & name);
    int getPlanetcount();
    const std::string & indexToPlanets(const int & index);
    Probe CenterOfMass{},OriginalProbe{};
    void SaveAll();//保存所有行星数据
    void LoadAll();//读取所有行星数据
    ~Calculator();
    Planet *Planets;//
private:
    std::map<std::string,int> planetIndex;
    QMutex probesMutex;
    std::map<std::string,Probe> probes;
    std::map<std::string,Probe>::iterator probesIter;
    std::string Location;
    int planetcount,PlanetStep;
    double PlanetH;
};

#endif // CALCULATOR_H
