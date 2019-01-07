#ifndef SPLINE_H
#define SPLINE_H

#include "mathused.h"
#include <fstream>
//三次条样插值
class Spline
{
public:
    ~Spline();
    int DataLength,history;
    double *h{nullptr},*M{nullptr},*A{nullptr},*B{nullptr},*x{nullptr};
    void GetSourceData(const int & SourceDataLength,const double *SourceDataX,const double *SourceDataY);//从数据得到函数
    double value(const double & in);//得到函数值
    void save(std::fstream &fs);//保存数据到文件中
    void load(std::fstream &fs);//从文件中读取数据
};

#endif // SPLINE_H
