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

//TODO:W=F/v选项，-v·F/v·v
//取消行星名字长度
//新建轨道
//画多个图
//打开文件错误
//删去变轨节点错误
//重新计算错误
//粒子群算法
//梯度下降法

class Calculator {
public:
    ClockForTest t{};
    Calculator(std::string iLocation,int iMaxPlanetNameLength,int iPlanetStep,double iPlanetH);
    void getData();//启动时必调用，行星数据
    void loadPlanet();//读取行星
    void savePlanet();//保存行星
    void probeRelativeOrbit(int step,int maxwide,int i,int j,double tmax,Probe &probe,double maxCosTheta);//重新换系，得到double数据点
    void solveProbe(Probe &probe,int step,double h,double tmax);//重新计算
    void NewSolveProbe(Probe &probe,int step,double epsilon,double tmax);//重新计算
    void Gamma3SolveProbe(Probe &probe,int step,double epsilon,double tmax);//重新计算
    void resetLocation(std::string iLocation);//文件目录设置
    GLfloat *getVertices(std::string name);//得到顶点指针
    GLfloat *getNormedT(std::string name);//得到时间
    Probe *getProbe(std::string name);//调用飞行器
    int getStep(std::string name);//得到画图步数
    std::vector<double> getScaleAndOrigin(std::string name);//得到推荐缩放以及原点,0是缩放，123原点
    GlPlot3D *getPlot3D(std::string name);//调用飞行器
    Probe CenterOfMass{},OriginalProbe{};
private:
    std::map<std::string,Probe*> probeIndex;
    std::map<std::string,Probe*>::iterator probeIndexIter;
    std::string Location;
    int planetcount,MaxPlanetNameLength,PlanetStep;
    double PlanetH;
    Planet *Planets;
    std::vector <Probe> probes;
};

#endif // CALCULATOR_H
