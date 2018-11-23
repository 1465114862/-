#ifndef CLOCKFORTEST_H
#define CLOCKFORTEST_H

//测试用计时器
#include <time.h>
class ClockForTest
{
public:
    clock_t Xstart, Xfinish;
    double duration;
    void start();//开始计时
    void finish();//停止计时
    void finishandstart();//结束并继续开始记时
};

#endif // CLOCKFORTEST_H
