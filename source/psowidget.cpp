#include "psowidget.h"
#include <QLabel>
#include <iostream>
#include <windows.h>

PSOwidget::PSOwidget(std::function<double(std::vector<double>)> func,QWidget *parent)
  : f(func)
{
    QHBoxLayout* layout=new QHBoxLayout;
    QString num;
    num.setNum(20000);
    layout->addLayout(labelAndLineEdit(step,num,"最大步数"));
    num.setNum(0.0645462/0.75,'g',15);
    layout->addLayout(labelAndLineEdit(epsilon,num,"容差参数"));
    num.setNum(18000000);
    layout->addLayout(labelAndLineEdit(EndT,num,"逆向积分开始时间"));
    num.setNum(86400);
    layout->addLayout(labelAndLineEdit(StartT,num,"最小时间"));
    num.setNum(50);
    layout->addLayout(labelAndLineEdit(generation,num,"迭代代数"));
    num.setNum(10);
    layout->addLayout(labelAndLineEdit(PPGnumber,num,"每组粒子个数"));
    num.setNum(5);
    layout->addLayout(labelAndLineEdit(Gnumber,num,"组数"));
    num.setNum(QThread::idealThreadCount());
    layout->addLayout(labelAndLineEdit(Threadnumber,num,"最大线程数"));
    QVBoxLayout *mainlayout = new QVBoxLayout;
    mainlayout->addLayout(layout);
    mainlayout->addWidget(new QLabel("变轨节点1"));
    mainlayout->addLayout(maxMin("时间",16500000,18000000));
    mainlayout->addLayout(maxMin("ΔVx",-150,150));
    mainlayout->addLayout(maxMin("ΔVy",-150,150));
    mainlayout->addLayout(maxMin("ΔVz",-150,150));
    mainlayout->addWidget(new QLabel("变轨节点2"));
    mainlayout->addLayout(maxMin("时间",0,16500000-86400*50));
    mainlayout->addLayout(maxMin("ΔVx",-40,40));
    mainlayout->addLayout(maxMin("ΔVy",-40,40));
    mainlayout->addLayout(maxMin("ΔVz",-40,40));
    start=new QPushButton(tr("开始计算"),this);
    connect(start,&QPushButton::clicked,this,&PSOwidget::startCalculate);
    mainlayout->addWidget(start);
    log=new QTextEdit;
    log->setReadOnly(true);
    mainlayout->addWidget(log);
    setLayout(mainlayout);
    setWindowTitle(tr("粒子群算法"));
    show();
}
QVBoxLayout* PSOwidget::labelAndLineEdit(QLineEdit * & line,QString context,QString label) {
    QVBoxLayout* layout=new QVBoxLayout;
    line=new QLineEdit(context);
    QLabel* la=new QLabel(label);
    layout->addWidget(la);
    layout->addWidget(line);
    return layout;
}
QHBoxLayout* PSOwidget::maxMin(QString context,double defaultmin,double defaultmax) {
    max.emplace_back();
    min.emplace_back();
    QHBoxLayout* layout=new QHBoxLayout;
    QString num;
    num.setNum(defaultmax,'g',15);
    layout->addLayout(labelAndLineEdit(max.back(),num,context+"最大值"));
    num.setNum(defaultmin,'g',15);
    layout->addLayout(labelAndLineEdit(min.back(),num,context+"最小值"));
    return layout;
}
double PSOwidget::getEndT(){
    return EndT->text().toDouble();
}
double PSOwidget::getStartT(){
    return StartT->text().toDouble();
}
double PSOwidget::getepsilon(){
    return epsilon->text().toDouble();
}
int PSOwidget::getstep(){
    return step->text().toInt();
}
std::vector<double> PSOwidget::getMax() {
    std::vector<double> out;
    for(int i=0;i<max.size();i++) {
        out.push_back(max.at(i)->text().toDouble());
    }
    return out;
}
void PSOwidget::logUpdate(QString theLog) {
    log->append(theLog);
}
void PSOwidget::clearLog() {
    log->clear();
}
int PSOwidget::getGeneration(){
    return generation->text().toInt();
}
int PSOwidget::getPPGnumber(){
    return PPGnumber->text().toInt();
}
int PSOwidget::getGnumber(){
    return Gnumber->text().toInt();
}
int PSOwidget::getThreadnumber(){
    return Threadnumber->text().toInt();
}
void PSOwidget::startCalculate() {
    start->setEnabled(false);
    if(pso){delete pso;pso=nullptr;}
    pso=new PSO(f,getMin(),getMax(),8,getGeneration(),getPPGnumber(),getGnumber(),getThreadnumber());
    connect(pso, &PSO::done,this, &PSOwidget::endCalculate);
    connect(pso, &PSO::toLog,this, &PSOwidget::logUpdate);
    pso->runPSO();
}
void PSOwidget::endCalculate() {
    start->setEnabled(true);
    emit resultReady(pso->getPosition());
}
std::vector<double> PSOwidget::getMin(){
    std::vector<double> out;
    for(int i=0;i<min.size();i++) {
        out.push_back(min.at(i)->text().toDouble());
    }
    return out;
}
