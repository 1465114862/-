#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "calculator.h"
#include "singlemaneuwinget.h"

class QPushButton;
class SingleWinget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
public slots:
    void save();
    void load();
    void setLocation();
    void closeEvent(QCloseEvent *event) override;
    void step2();
    void errorWidget(QString e);
signals:
    void enableAll(bool);
    void doStep2();
    void doErrorWidget(QString e);
private:
    QPushButton *L,*S;
    void step1();
    std::string location;
    QLineEdit *lineLoca;
    Calculator *calculate;
    SingleWinget *widget;
};

#endif // MAINWINDOW_H
