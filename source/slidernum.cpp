#include "slidernum.h"
#include "mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QTimer>
#include <QSlider>
#include <QLabel>
#include <iostream>
#include <cmath>

slidernum::slidernum(double *iv,double it,QString ina)
    : value(iv),
      times(it),
      name(ina)
{
    slider = new QSlider(Qt::Horizontal);
    slider->setRange(-1000, 1000);
    slider->setSingleStep(1);
    slider->setPageStep(50);
    slider->setTickInterval(50);
    slider->setTickPosition(QSlider::TicksRight);
    slider->setValue(0);
    QString num0;
    num0.setNum(*value);
    lineEdit = new QLineEdit(num0);
    add=0;
    label=new QLabel;
    label->setText(name);
    QString num;
    num.setNum(times);
    lineEdit2 = new QLineEdit(num);
    label2=new QLabel;
    label2->setText("倍数");
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(label);
    layout->addWidget(lineEdit);
    layout->addWidget(label2);
    layout->addWidget(lineEdit2);
    QWidget *w = new QWidget;
    w->setLayout(layout);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(slider);
    mainLayout->addWidget(w);
    connect(slider, &QSlider::valueChanged, this, &slidernum::addChange);
    connect(slider, &QSlider::sliderPressed, this, &slidernum::timerCreate);
    connect(slider, &QSlider::sliderReleased, this, &slidernum::timerDisable);
    connect(this, &slidernum::valueChanged, lineEdit, &QLineEdit::setText);
    connect(lineEdit, &QLineEdit::editingFinished,this, &slidernum::textChangeValue);
    connect(lineEdit2, &QLineEdit::editingFinished,this, &slidernum::changeTimes);
    setLayout(mainLayout);
    setWindowTitle(tr("Hello GL"));
}

void slidernum::timerCreate(){
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(changeValue()));
    timer->start(20);
}

void slidernum::timerDisable() {
    if(timer)
        delete timer;
    slider->setValue(0);
    add=0;
}

void slidernum::addChange(double iadd) {
    add=iadd;
}

void slidernum::changeValue() {
    (*value)+=(add>0 ? times*pow(10,add/500) : -times*pow(10,-add/500));
    QString num;
    num.setNum((*value));
    emit valueChanged(num);
}

void slidernum::textChangeValue() {
    (*value)=lineEdit->text().toDouble();
}

void slidernum::changeTimes() {
    times=lineEdit->text().toDouble();
}
