#include "singlewinget.h"
#include "plotwidget.h"
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QComboBox>
#include <QLabel>
SingleWinget::SingleWinget(MainWindow *mw,Calculator *icalculate,std::vector<std::string> itoPlot)
  : mainWindow(mw),
    calculate(icalculate),
    toPlot(itoPlot)
{
    p1="Earth";
    p2="Sun";
    origin=P1;
    rotate=P1toP2;
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
    QVBoxLayout *Combolayout = new QVBoxLayout;
    QComboBox *P1ComboBox = new QComboBox;
    QComboBox *P2ComboBox = new QComboBox;
    QComboBox *OriginComboBox = new QComboBox;
    QComboBox *RotateComboBox = new QComboBox;
    QLabel *p1lable=new QLabel(tr("第一参照物"));
    QLabel *p2lable=new QLabel(tr("第二参照物"));
    QLabel *Originlable=new QLabel(tr("原点选择"));
    QLabel *Rotatelable=new QLabel(tr("旋转系选择"));
    Combolayout->addWidget(p1lable);
    Combolayout->addWidget(P1ComboBox);
    Combolayout->addWidget(p2lable);
    Combolayout->addWidget(P2ComboBox);
    Combolayout->addWidget(Originlable);
    Combolayout->addWidget(OriginComboBox);
    Combolayout->addWidget(Rotatelable);
    Combolayout->addWidget(RotateComboBox);
    for(int i=0;i<calculate->getPlanetcount();i++) {
        P1ComboBox->addItem(QString::fromStdString(calculate->indexToPlanets(i)));
        P2ComboBox->addItem(QString::fromStdString(calculate->indexToPlanets(i)));
    }
    OriginComboBox->addItem(tr("第一参照物"));
    OriginComboBox->addItem(tr("第二参照物"));
    OriginComboBox->addItem(tr("两者质心"));
    RotateComboBox->addItem(tr("随两参照物连线旋转"));
    RotateComboBox->addItem(tr("不旋转"));
    P1ComboBox->setCurrentIndex(calculate->planetToIndex(p1));
    P2ComboBox->setCurrentIndex(calculate->planetToIndex(p2));
    OriginComboBox->setCurrentIndex(0);
    RotateComboBox->setCurrentIndex(0);
    plotwidget = new plotWidget(icalculate,itoPlot,p1,p2,origin,rotate);
    connect(P1ComboBox, SIGNAL(activated(const QString &)),
            this, SLOT(P1Changed(const QString &)));
    connect(P2ComboBox, SIGNAL(activated(const QString &)),
            this, SLOT(P2Changed(const QString &)));
    connect(OriginComboBox, SIGNAL(activated(int)),
            this, SLOT(OriginChanged(int)));
    connect(RotateComboBox, SIGNAL(activated(int)),
            this, SLOT(RotateChanged(int)));
    QHBoxLayout *PQPlayout = new QHBoxLayout;
    PQPlayout->addWidget(plotwidget);
    PQPlayout->addLayout(Combolayout);
    PQPlayout->addLayout(PQlayout);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(PQPlayout);
    newManeu=new QPushButton(tr("增加变轨节点"),this);
    connect(newManeu,&QPushButton::clicked,this,&SingleWinget::addManueWidget);
    mainLayout->addWidget(newManeu);
    plot=new QPushButton(tr("重新绘制"),this);
    connect(plot,&QPushButton::clicked,this,&SingleWinget::replot);
    connect(plotwidget,&plotWidget::rePlotResultReady,plot,&QPushButton::setEnabled);
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
    plot->setEnabled(false);
    plotwidget->rePlot();
}

void SingleWinget::closeEvent(QCloseEvent *event) {
    for(manueIterator=manue.begin();manueIterator!=manue.end();manueIterator++){
        (*manueIterator)->close();
        delete (*manueIterator);
    }
}

void SingleWinget::clearUpManue() {
    for(manueIterator=manue.begin();manueIterator!=manue.end();manueIterator++){
        if((*manueIterator)->isClosed()){
            delete (*manueIterator);
            manue.erase(manueIterator);
        }
    }
}

void SingleWinget::P1Changed(const QString &name){
    p1=name.toStdString();
    plotwidget->reference(p1,p2,origin,rotate);
}
void SingleWinget::P2Changed(const QString &name) {
    p2=name.toStdString();
    plotwidget->reference(p1,p2,origin,rotate);
}
void SingleWinget::OriginChanged(int index) {
    switch (index) {
    case 0:
        origin=P1;
        break;
    case 1:
        origin=P2;
        break;
    case 2:
        origin=CenterOfMass;
        break;
    }
    plotwidget->reference(p1,p2,origin,rotate);
}
void SingleWinget::RotateChanged(int index) {
    switch (index) {
    case 0:
        rotate=P1toP2;
        break;
    case 1:
        rotate=NoneRotate;
        break;
    }
    plotwidget->reference(p1,p2,origin,rotate);
}
