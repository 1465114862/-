#ifndef PLOT_H
#define PLOT_H

#include <stdexcept>
#include <QtGui/QOpenGLShaderProgram>
#include "mathused.h"
//画图时原点与范围转换
void GLCoordinatesTransform(const double *a,const double *b,const double *c,GLfloat *vertices,int maxwide,int length);
class double4{
public:
    double v[4];
};
//绘制三维参数图
class GlPlot3D
{
public:
    ~GlPlot3D();
    void plot();//得到所有绘图数据点
    GlPlot3D(std::function<double(double)> &ifx,std::function<double(double)> &ify,std::function<double(double)> &ifz,int imaxwide,int iminstep,double iminCosTheta,std::vector<double> iTGroup);
    GLfloat *normedT{nullptr},*Vbo{nullptr};//OpenGL绘图所需的顶点
    double *vertices{nullptr};
    int Step();//顶点数
    std::list<double4> data;
    std::list<double4>::iterator dataIterator;
    std::vector<double> getScaleAndOrigin();
    void shift(std::vector<double> in);
private:
    std::vector<double> tGroup;
    int step,maxwide,minstep;
    double minCosTheta;
    std::function<double(double)> *fx,*fy,*fz;
    void setVertices(double *a,double *b,double *c);
    bool vCosTheta3(const double * r1,const double *r2);
    GLfloat glFindMinOrMax(bool Max,const GLfloat *a,int length,int step=1);
};

#endif // PLOT_H
