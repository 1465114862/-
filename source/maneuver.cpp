#include "maneuver.h"
#include <iostream>
SingleManeuver::SingleManeuver() {
    t=0;
    v[0]=0;
    v[1]=0;
    v[2]=0;
}
SingleManeuver SingleManeuver::operator =(SingleManeuver& str) {
    t=str.t;
    v[0]=str.v[0];
    v[1]=str.v[1];
    v[2]=str.v[2];
}
void Maneuver::add(SingleManeuver singleManeuver) {
    maneuver.push_back(singleManeuver);
    length++;
    maneuverIterator=maneuver.end();
    maneuverIterator--;
    now=length;
    resetT(length);
}
void Maneuver::iteratorP() {
    maneuverIterator++;
    now++;
}
void Maneuver::iteratorM() {
    maneuverIterator--;
    now--;
}
void Maneuver::iteratorReach(int which) {
    while(now!=which){
        if(now<which)
            iteratorP();
        else
            iteratorM();
    }
}
void Maneuver::resetT(int which) {
    iteratorReach(which);
    SingleManeuver cache1=*maneuverIterator,cache2;
    while(now!=length) {
        iteratorP();
        if(cache1.t>(*maneuverIterator).t) {
            cache2=*maneuverIterator;
            *maneuverIterator=cache1;
            iteratorM();
            *maneuverIterator=cache2;
            cache1=cache2;
            iteratorP();
        }
        else{
            iteratorM();
            break;
        }
    }
    cache1=*maneuverIterator;
    while(now!=1) {
        iteratorM();
        if(cache1.t<(*maneuverIterator).t) {
            cache2=*maneuverIterator;
            *maneuverIterator=cache1;
            iteratorP();
            *maneuverIterator=cache2;
            cache1=cache2;
            iteratorM();
        }
        else{
            iteratorP();
            break;
        }
    }
}
void Maneuver::reset(SingleManeuver singleManeuver) {
    *maneuverIterator=singleManeuver;
    resetT(now);
}
void Maneuver::cut(double tmin,double tmax) {
    maneuver.remove_if([&](const SingleManeuver & single)->bool{
        return (single.t<=tmin)||(single.t>=tmax);
                       });
    maneuverIterator=maneuver.begin();
    now=1;
    length=maneuver.size();
}

Maneuver& Maneuver::operator =(Maneuver& str){
    length=str.length;
    now=1;
    maneuver.clear();
    for(std::list <SingleManeuver> ::iterator maneuverIterator2=str.maneuver.begin();maneuverIterator2!=str.maneuver.end();maneuverIterator2++)
        maneuver.push_back(*maneuverIterator2);
    maneuverIterator=maneuver.begin();
    iteratorReach(str.now);
    return *this;
}
