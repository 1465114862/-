#ifndef SINGLEWINGET_H
#define SINGLEWINGET_H

#include <QWidget>
#include "calculator.h"
#include <list>
#include "slidernum.h"
#include <QPushButton>

class plotWidget;
class MainWindow;
class SingleManeuWinget;

class SingleWinget : public QWidget
{
    Q_OBJECT

public:
    SingleWinget(MainWindow *mw,Calculator *icalculate,std::vector<std::string> itoPlot);
signals:
public slots:
    void replot();
    void addManueWidget();
private:
    void clearUpManue();
    QPushButton *plot,*newManeu;
    slidernum *PAndQ[6];
    Calculator *calculate;
    std::vector<std::string> toPlot;
    std::list<SingleManeuWinget*> manue;
    std::list<SingleManeuWinget*>::iterator manueIterator;
    Maneuver *maneuver;
    double *P,*Q;
    plotWidget *plotwidget;
    MainWindow *mainWindow;
};

#endif // SINGLEWINGET_H
