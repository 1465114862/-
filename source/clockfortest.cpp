#include "clockfortest.h"
#include <iostream>

void ClockForTest::start(){
    Xstart=clock();
}
void ClockForTest::finish() {
    Xfinish = clock();
    duration = (double)(Xfinish - Xstart) / CLOCKS_PER_SEC;
    std::cout << duration << std::endl;
}
void ClockForTest::finishandstart() {
    finish();
    start();
}
