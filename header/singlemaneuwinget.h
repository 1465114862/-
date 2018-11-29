#ifndef SINGLEMANEUWINGET_H
#define SINGLEMANEUWINGET_H

#include <QWidget>
#include "maneuver.h"
#include "slidernum.h"


class SingleManeuWinget : public QWidget
{
    Q_OBJECT
public:
    SingleManeuWinget(QWidget *parent = 0);
    SingleManeuver getManeu();
    bool isClosed();
public slots:
    void closeEvent(QCloseEvent *event) override;
private:
    bool closed;
    SingleManeuver maneu;
    slidernum *t,*vx,*vy,*vz;
};

#endif // SINGLEMANEUWINGET_H
