#ifndef INTIDATA_H
#define INTIDATA_H

#include "planet.h"
#include "probe.h"

//读取行星数据(pdata.txt)
void GetPlanetData(Planet * Planets,int planetcount,std::string Location);
//从名字得到行星编号
int NameToNumber(const std::string name,Planet *Planets,int planetcount);
//读取初始数据(data.txt)
void GetInitialData(Planet *Planets,int planetcount,std::string Location);
//得到中心天体编号(游戏设定)
int GetSoibody(Planet *Planets,int planetcount);
//初始数据处理1
void PhaseSpaceProcess_1(Planet *Planets,int planetcount);
//得到全体的质心
void GetCenterOfMass(Planet *Planets,Probe &CenterOfMass,int planetcount);
//初始数据处理2
void PhaseSpaceProcess_2(Planet *Planets,int planetcount,Probe &CenterOfMass);
//全部初始化
void InitializationPlanets(Planet *Planets,int planetcount,Probe &CenterOfMass,std::string Location);

#endif // INTIDATA_H
