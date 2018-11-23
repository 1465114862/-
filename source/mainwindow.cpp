#include "mainwindow.h"
#include "singlewinget.h"
#include <iostream>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <stdexcept>
#include <QMessageBox>
#include <QLabel>
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
        calculate=new Calculator(location,20,25000,864*2);
        calculate->getData();
    } catch(std::exception& e) {
        QMessageBox::information(0, tr("错误"), e.what());
        return;
    }
    step1();
}

void MainWindow::step1() {
    takeCentralWidget();
    QVBoxLayout *SorL = new QVBoxLayout;
    QPushButton *L,*S;
    S=new QPushButton(tr("生成行星轨道数据并保存"),this);
    connect(S,&QPushButton::clicked,this,&MainWindow::save);
    SorL->addWidget(S);
    L=new QPushButton(tr("读取已有行星轨道数据"),this);
    connect(L,&QPushButton::clicked,this,&MainWindow::load);
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
    SingleWinget *w = new SingleWinget(this,calculate,buf);
    resize(1000,700);
    if (!centralWidget())
        setCentralWidget(w);
}

void MainWindow::save() {
    try {
        calculate->savePlanet();
    } catch(std::exception& e) {
        QMessageBox::information(0, tr("错误"), e.what());
        return;
    }
    step2();
}

void MainWindow::load() {
    try {
        calculate->loadPlanet();
    } catch(std::exception& e) {
        QMessageBox::information(0, tr("错误"), e.what());
        return;
    }
    step2();
}
