#include "singlemaneuwinget.h"
#include <QVBoxLayout>
#include <QApplication>
#include <QDesktopWidget>
#include "mainwindow.h"

SingleManeuWinget::SingleManeuWinget(SingleManeuWinget **itothis,QWidget *parent)
    : tothis(itothis)
{
    maneu.t=0;
    maneu.v[0]=0;
    maneu.v[1]=0;
    maneu.v[2]=0;
    t = new slidernum(&(maneu.t),1,"时间");
    vx = new slidernum(maneu.v,1,"ΔVx");
    vy = new slidernum(maneu.v+1,1,"ΔVy");
    vz = new slidernum(maneu.v+2,1,"ΔVz");
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(t);
    layout->addWidget(vx);
    layout->addWidget(vy);
    layout->addWidget(vz);
    setLayout(layout);
    setWindowTitle(tr("maneu"));
    connect(this,&QObject::destroyed,this,&SingleManeuWinget::deletePointer);
    show();
}

void SingleManeuWinget::deletePointer() {
    *tothis=NULL;
}

SingleManeuver SingleManeuWinget::getManeu() {
    return maneu;
}
