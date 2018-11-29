#include "singlemaneuwinget.h"
#include <QVBoxLayout>
#include <QApplication>
#include <QDesktopWidget>
#include "mainwindow.h"
#include <QMessageBox>

SingleManeuWinget::SingleManeuWinget(QWidget *parent)
{
    closed=false;
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
    show();
}

void SingleManeuWinget::closeEvent(QCloseEvent *event) {
    closed=true;
}

bool SingleManeuWinget::isClosed() {return closed;}

SingleManeuver SingleManeuWinget::getManeu() {
    return maneu;
}
