#ifndef SINGLEWINGET_H
#define SINGLEWINGET_H

#include <QWidget>
#include "calculator.h"
#include <list>
#include "slidernum.h"
#include "psowidget.h"
#include <QPushButton>

class plotWidget;
class MainWindow;
class SingleManeuWinget;
class QComboBox;

class SingleWinget : public QWidget
{
    Q_OBJECT

public:
    SingleWinget(MainWindow *mw,Calculator *icalculate,std::vector<std::string> itoPlot);
    std::function<double(std::vector<double>)> thefunction;
    std::function<void()> reverseCut;
signals:
public slots:
    void replot();
    void addManueWidget();
    void closeEvent(QCloseEvent *event) override;
    void P1Changed(const QString &name);
    void P2Changed(const QString &name);
    void P3Changed(const QString &name);
    void OriginChanged(int index);
    void RotateChanged(int index);
    void MethodChanged(int index);
    void save();
    void saveResult();
    void load();
    void testsolt();
    void cut();
    void reallyReplot();
    void pointToResultManue(std::vector<double> point);
    void resetViewP2();
    void resetViewP1();
private:
    vector<double> tempPoint;
    PSOwidget *psowidget{nullptr};
    double startT,resultStartT;
    double oldP[3],oldQ[3],newQ[3],newP[3];
    int testcount;
    bool rest;
    QLabel *stepLabel,*toleranceLabel,*tmaxLabel,*tminLabel;
    std::string p1,p2,p3;
    referenceOrigin origin;
    referenceRotate rotate;
    void clearUpManue();
    QPushButton *plot,*newManeu,*saveButton,*loadButton,*resetView,*test,*P1reset,*P2reset;
    slidernum *PAndQ[6];
    Calculator *calculate;
    std::vector<std::string> toPlot;
    Maneuver resultManeu;
    std::list<SingleManeuWinget*> manue;
    std::list<SingleManeuWinget*>::iterator manueIterator;
    double *P,*Q;
    plotWidget *plotwidget;
    MainWindow *mainWindow;
    QLineEdit *step,*tmin,*tmax,*tolerance,*loadFileName,*saveFileName;
    QComboBox *MethodComboBox,*P1ComboBox,*P2ComboBox,*P3ComboBox,*OriginComboBox,*RotateComboBox;
};

#endif // SINGLEWINGET_H
