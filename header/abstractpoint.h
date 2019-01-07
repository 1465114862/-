#ifndef ABSTRACTPOINT_H
#define ABSTRACTPOINT_H

#include <string>
#include "plot.h"

class AbstractPoint
{
public:
    AbstractPoint();
    double mass;
    std::string name;
    GlPlot3D *plot{nullptr};
    virtual double value(int dimension,double t)=0;
    virtual double velocity(int dimension,double t)=0;
    virtual ~AbstractPoint() = 0;
};

#endif // ABSTRACTPOINT_H
