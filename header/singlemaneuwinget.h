#ifndef SINGLEMANEUWINGET_H
#define SINGLEMANEUWINGET_H

#include <QWidget>
#include "maneuver.h"
#include "slidernum.h"


class SingleManeuWinget : public QWidget
{
    Q_OBJECT
public:
    SingleManeuWinget(SingleManeuWinget*& ihere ,QWidget *parent = 0);
    const SingleManeuver& getManeu();
    bool isClosed();
    void setManeu(SingleManeuver in);
public slots:
    void closeEvent(QCloseEvent *event) override;
private:
    SingleManeuWinget*& here;
    bool closed;
    SingleManeuver maneu;
    slidernum *t,*vx,*vy,*vz;
};

#endif // SINGLEMANEUWINGET_H
