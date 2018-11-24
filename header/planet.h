#ifndef PLANET_H
#define PLANET_H

#include "spline.h"

//txt文档读取单词
void GetWord(std::fstream &fs,char *word,int MaxLength);
//得到行星数量(pcount.txt)
int GetPlanetCount(std::string Location);
//行星类
class Planet
{
public:
    int MaxPlanetNameLength,DataLength;
    bool IsSoibody,IsSphericalSymmetry;
    char *name;
    double mass,j2,J2,JJ2,radius,RotationAxis[3],RawPosition[3],RawVelocity[3],InitialPosition[3],InitialVelocity[3],*Position,*Velocity,acceleration[3],dp[3],dq[3],PCache[3],QCache[3];
    Spline SplinePosition[3],SplineVelocity[3];
    void GetMaxPlanetNameLength(int length);//初始化名字最长长度
    void GetName(std::fstream &fs);//读取名字
    void GetParameter(std::fstream &fs);//读取行星参数
    void GetRawInformation(std::fstream &fs);//读取原初始数据(游戏中得到)
    void GetRotationAxis(std::fstream &fs);//读取自转轴数据(游戏中得到)
    void InitializationPhaseSpace(int step,int position);//求解前初始化
    void ClearAcceleration();//清除加速度缓存
    void Cleardpdq();//清除坐标和动量空间增量缓存
    void ClearPositon();//解出方程后清除数据点存储
    friend void gravity(Planet &Planets_1,Planet &Planets_2);//得到两星球间力
    void GetSpline(double *XSource);//由数据点插值
    void save(std::fstream &fs);//存储星球解数据
    void load(std::fstream &fs);//读取星球解数据
    double value(int dimension,double t);
    double velocity(int dimension,double t);
};
//保存所有行星数据
void SaveAll(Planet *Planets,int planetcount,std::string Location);
//读取所有行星数据
void LoadAll(Planet *Planets,int planetcount,std::string Location);

#endif // PLANET_H
