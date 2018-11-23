#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "calculator.h"
#include "singlemaneuwinget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
public slots:
    void save();
    void load();
    void setLocation();
private:
    void step1();
    void step2();
    std::string location;
    QLineEdit *lineLoca;
    Calculator *calculate;
};

#endif // MAINWINDOW_H
