#include "singlewinget.h"
#include "plotwidget.h"
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
SingleWinget::SingleWinget(MainWindow *mw,Calculator *icalculate,std::vector<std::string> itoPlot)
  : mainWindow(mw),
    calculate(icalculate),
    toPlot(itoPlot)
{
    P=new double[3];
    Q=new double[3];
    for(int i=0;i<3;i++){
        P[i]=calculate->getProbe(toPlot.at(0))->InitialVelocity[i];
        Q[i]=calculate->getProbe(toPlot.at(0))->InitialPosition[i];
    }
    PAndQ[0]=new slidernum(Q,1,"X");
    PAndQ[1]=new slidernum(Q+1,1,"Y");
    PAndQ[2]=new slidernum(Q+2,1,"Z");
    PAndQ[3]=new slidernum(P,1,"Vx");
    PAndQ[4]=new slidernum(P+1,1,"Vy");
    PAndQ[5]=new slidernum(P+2,1,"Vz");
    QVBoxLayout *PQlayout = new QVBoxLayout;
    for(int i=0;i<6;i++){
        PQlayout->addWidget(PAndQ[i]);
    }
    PQlayout->addStretch();
    plotwidget = new plotWidget(icalculate,itoPlot);
    QHBoxLayout *PQPlayout = new QHBoxLayout;
    PQPlayout->addWidget(plotwidget);
    PQPlayout->addLayout(PQlayout);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(PQPlayout);
    newManeu=new QPushButton(tr("增加变轨节点"),this);
    connect(newManeu,&QPushButton::clicked,this,&SingleWinget::addManueWidget);
    mainLayout->addWidget(newManeu);
    plot=new QPushButton(tr("重新绘制"),this);
    connect(plot,&QPushButton::clicked,this,&SingleWinget::replot);
    mainLayout->addWidget(plot);
    mainLayout->addStretch();
    mainLayout->addWidget(plotwidget);
    setLayout(mainLayout);
    setWindowTitle(tr("Orbit"));
}

void SingleWinget::addManueWidget() {
    SingleManeuWinget *newm;
    manue.push_back(newm);
    manueIterator=manue.end();
    manueIterator--;
    *manueIterator=new SingleManeuWinget();
}

void SingleWinget::replot() {
    calculate->getProbe(toPlot.at(0))->GetPhaseSpaceInitialPosition(Q,P);
    clearUpManue();
    Maneuver maneus;
    for(manueIterator=manue.begin();manueIterator!=manue.end();manueIterator++){
         maneus.add((*manueIterator)->getManeu());
    }
    calculate->getProbe(toPlot.at(0))->maneuver=maneus;
    plotwidget->rePlot();
}

void SingleWinget::clearUpManue() {
    for(manueIterator=manue.begin();manueIterator!=manue.end();manueIterator++){
        if((*manueIterator)->isClosed()){
            delete (*manueIterator);
            manue.erase(manueIterator);
        }
    }
}
