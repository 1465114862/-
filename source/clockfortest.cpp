#include "clockfortest.h"
#include <iostream>

void ClockForTest::start(){
    Xstart=clock();
}
double ClockForTest::finish() {
    Xfinish = clock();
    duration = (double)(Xfinish - Xstart) / CLOCKS_PER_SEC;
    return duration;
}
double ClockForTest::finishandstart() {
    double r{finish()};
    start();
    return r;
}
