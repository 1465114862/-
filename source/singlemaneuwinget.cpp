#include "singlemaneuwinget.h"
#include <QVBoxLayout>
#include <QApplication>
#include <QDesktopWidget>
#include "mainwindow.h"
#include <QMessageBox>

SingleManeuWinget::SingleManeuWinget(SingleManeuWinget*& ihere,QWidget *parent)
    : here(ihere)
{
    setAttribute(Qt::WA_DeleteOnClose);
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
    here=nullptr;
}

bool SingleManeuWinget::isClosed() {return closed;}

const SingleManeuver & SingleManeuWinget::getManeu() {
    return maneu;
}

void SingleManeuWinget::setManeu(SingleManeuver in) {
    maneu.t=in.t;
    t->updateValue();
    maneu.v[0]=in.v[0];
    vx->updateValue();
    maneu.v[1]=in.v[1];
    vy->updateValue();
    maneu.v[2]=in.v[2];
    vz->updateValue();
}
//清理掉close
