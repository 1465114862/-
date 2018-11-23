#ifndef MANEUVER_H
#define MANEUVER_H

#include <list>

class SingleManeuver {
public:
    double t,v[3];
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
    Maneuver& operator =(Maneuver& str);
};

#endif // MANEUVER_H
