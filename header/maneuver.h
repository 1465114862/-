#ifndef MANEUVER_H
#define MANEUVER_H

#include <list>

class SingleManeuver {
public:
    double t,v[3];
    SingleManeuver();
    SingleManeuver operator =(SingleManeuver& str);
};
class Maneuver {
public:
    Maneuver(){length=0;now=1;}
    int length,now;
    std::list <SingleManeuver> maneuver;
    std::list <SingleManeuver> ::iterator maneuverIterator;
    void add(SingleManeuver singleManeuver);
    void resetT(int which);
    void reset(SingleManeuver singleManeuver);
    void iteratorP();
    void iteratorM();
    void iteratorReach(int which);
    void cut(double tmin,double tmax);
    Maneuver& operator =(Maneuver& str);
};

#endif // MANEUVER_H
