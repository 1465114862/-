#include "intidata.h"
#include <iostream>
#include <stdexcept>
//读取行星数据(pdata.txt)
void GetPlanetData(Planet * Planets,int planetcount,std::string Location) {
    std::fstream indata;
    indata.open(Location+R"(\pdata.txt)",std::ios::in);
    if(!indata.is_open ())
        throw std::runtime_error{"打开pdata.txt失败"};
    for(int i=0;i<planetcount;i++)
        (*(Planets+i)).GetName(indata);
    for(int i=0;i<planetcount;i++)
        (*(Planets+i)).GetParameter(indata);
    indata.close();
}
//从名字得到行星编号
int NameToNumber(const std::string name,Planet *Planets,int planetcount) {
    for(int j=0;j<planetcount;j++) {
        if (name==(*(Planets+j)).name)
            return j;
    }
    return -1;
}
//读取初始数据(data.txt)
void GetInitialData(Planet *Planets,int planetcount,std::string Location) {
    int soibody;
    std::string soiname;
    std::fstream indata;
    indata.open(Location+R"(\data.txt)", std::ios::in);
    if (!indata.is_open())
        throw std::runtime_error{"打开data.txt失败"};
    for(int i=0;i<planetcount;i++)
        (*(Planets+i)).GetRawInformation(indata);
    for(int i=0;i<planetcount;i++)
        (*(Planets+i)).GetRotationAxis(indata);
    indata.get();
    indata>>soiname;
    soibody = NameToNumber(soiname, Planets,planetcount);
    for(int i=0;i<planetcount;i++)
        ((*(Planets+i)).IsSoibody)= i == soibody;
    indata.close();
}
//得到中心天体编号(游戏设定)
int GetSoibody(Planet *Planets,int planetcount) {
    for(int i=0;i<planetcount;i++) {
        if((*(Planets+i)).IsSoibody)
            return i;
    }
    return -1;
}
//初始数据处理1
void PhaseSpaceProcess_1(Planet *Planets,int planetcount) {
    int soibody=GetSoibody(Planets,planetcount);
    for(int i=0;i<planetcount;i++) {
        if(!(*(Planets+i)).IsSoibody) {
            for (int j = 0; j < 3; j++) {
                (*(Planets + i)).InitialPosition[j] = (*(Planets + i)).RawPosition[j];
                (*(Planets + i)).InitialVelocity[j] =
                        (*(Planets + soibody)).RawVelocity[j] + (*(Planets + i)).RawVelocity[j];
            }
        }
        else {
            for(int j=0;j<3;j++) {
                (*(Planets+i)).InitialPosition[j]=(*(Planets+i)).RawPosition[j];
                (*(Planets+i)).InitialVelocity[j]=(*(Planets+i)).RawVelocity[j];
            }
        }
    }
}
//得到全体的质心
void GetCenterOfMass(Planet *Planets,Probe &CenterOfMass,int planetcount) {
    double MassSum=0;
    for(int i=0;i<planetcount;i++) {
        MassSum+=(*(Planets+i)).mass;
    }
    for(int i=0;i<3;i++) {
        CenterOfMass.InitialVelocity[i]=0;
        CenterOfMass.InitialPosition[i]=0;
    }
    for(int j=0;j<planetcount;j++) {
        for(int i=0;i<3;i++) {
            CenterOfMass.InitialPosition[i]+=((*(Planets+j)).mass)*((*(Planets+j)).InitialPosition[i])/MassSum;
            CenterOfMass.InitialVelocity[i]+=((*(Planets+j)).mass)*((*(Planets+j)).InitialVelocity[i])/MassSum;
        }
    }
}
//初始数据处理2
void PhaseSpaceProcess_2(Planet *Planets,int planetcount,Probe &CenterOfMass) {
    for(int i=0;i<planetcount;i++) {
        for (int j = 0; j < 3; j++) {
            (*(Planets + i)).InitialPosition[j] -= CenterOfMass.InitialPosition[j];
            (*(Planets + i)).InitialVelocity[j] -= CenterOfMass.InitialVelocity[j];
        }
    }
}
//全部初始化
void InitializationPlanets(Planet *Planets,int planetcount,Probe &CenterOfMass,std::string Location){
    GetPlanetData(Planets,planetcount,Location);
    GetInitialData(Planets,planetcount,Location);
    PhaseSpaceProcess_1(Planets,planetcount);
    GetCenterOfMass(Planets,CenterOfMass,planetcount);
    PhaseSpaceProcess_2(Planets,planetcount,CenterOfMass);
}
