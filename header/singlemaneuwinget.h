#ifndef SINGLEMANEUWINGET_H
#define SINGLEMANEUWINGET_H

#include <QWidget>
#include "maneuver.h"
#include "slidernum.h"


class SingleManeuWinget : public QWidget
{
    Q_OBJECT
public:
    SingleManeuWinget(SingleManeuWinget **itothis,QWidget *parent = 0);
    SingleManeuver getManeu();
public slots:
    void deletePointer();
private:
    SingleManeuWinget **tothis;
    SingleManeuver maneu;
    slidernum *t,*vx,*vy,*vz;
};

#endif // SINGLEMANEUWINGET_H
