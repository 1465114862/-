#include "mainwindow.h"
#include "singlewinget.h"
#include <iostream>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <stdexcept>
#include <QMessageBox>
#include <QLabel>
#include <QThread>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QVBoxLayout *file = new QVBoxLayout;
    QLabel *tip=new QLabel("请输入行星数据所在文件夹路径");
    file->addWidget(tip);
    QPushButton *open;
    lineLoca = new QLineEdit("");
    file->addWidget(lineLoca);
    open=new QPushButton(tr("读取行星数据"),this);
    connect(open,&QPushButton::clicked,this,&MainWindow::setLocation);
    file->addWidget(open);
    QWidget *w = new QWidget;
    w->setLayout(file);
    w->resize(1000,10);
    if (!centralWidget())
        setCentralWidget(w);
}

void MainWindow::setLocation() {
    try {
        location=lineLoca->text().toStdString();
        calculate=new Calculator(location,25000,864*2);
        calculate->getData();
    } catch(std::exception& e) {
        QMessageBox::information(0, tr("错误"), e.what());
        return;
    }
    step1();
}

void MainWindow::step1() {
    takeCentralWidget();
    connect(this, &MainWindow::doStep2, this, &MainWindow::step2);
    connect(this, &MainWindow::doErrorWidget, this, &MainWindow::errorWidget);
    QVBoxLayout *SorL = new QVBoxLayout;
    S=new QPushButton(tr("生成行星轨道数据并保存"),this);
    connect(S,&QPushButton::clicked,this,&MainWindow::save);
    connect(this,&MainWindow::enableAll,S,&QPushButton::setEnabled);
    SorL->addWidget(S);
    L=new QPushButton(tr("读取已有行星轨道数据"),this);
    connect(L,&QPushButton::clicked,this,&MainWindow::load);
    connect(this,&MainWindow::enableAll,L,&QPushButton::setEnabled);
    SorL->addWidget(L);
    QWidget *w = new QWidget;
    w->setLayout(SorL);
    if (!centralWidget())
        setCentralWidget(w);
}

void MainWindow::step2() {
    takeCentralWidget();
    std::vector<std::string> buf;
    buf.push_back("OriginalProbe");
    widget = new SingleWinget(this,calculate,buf);
    resize(1000,700);
    if (!centralWidget())
        setCentralWidget(widget);
}

void MainWindow::errorWidget(QString e) {
    QMessageBox::information(0, tr("错误"), e);
}

void MainWindow::save() {
    S->setEnabled(false);
    L->setEnabled(false);
    QThread *calculateThread=QThread::create([&]()->void{
                                                 try {
                                                     calculate->savePlanet();
                                                 } catch(std::exception& e) {
                                                     emit doErrorWidget(e.what());
                                                     emit enableAll(true);
                                                     return;
                                                 }
                                                 emit doStep2();
                                             });
    connect(calculateThread, &QThread::finished, calculateThread, &QObject::deleteLater);
    calculateThread->start();
}

void MainWindow::load() {
    S->setEnabled(false);
    L->setEnabled(false);
    QThread *calculateThread=QThread::create([&]()->void{
                                                 try {
                                                     calculate->loadPlanet();
                                                 } catch(std::exception& e) {
                                                     emit doErrorWidget(e.what());
                                                     emit enableAll(true);
                                                     return;
                                                 }
                                                 emit doStep2();
                                             });
    connect(calculateThread, &QThread::finished, calculateThread, &QObject::deleteLater);
    calculateThread->start();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    takeCentralWidget();
    if(widget){widget->close();}
}
