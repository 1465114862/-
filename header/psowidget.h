#ifndef PSOWIDGET_H
#define PSOWIDGET_H

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QThread>
#include "Swarm.h"
class PSOwidget : public QWidget
{
    Q_OBJECT
public:
    explicit PSOwidget(std::function<double(std::vector<double>)> func,QWidget *parent = nullptr);
    double getEndT();
    double getStartT();
    double getepsilon();
    int getstep();
    int getGeneration();
    int getPPGnumber();
    int getGnumber();
    int getThreadnumber();
    std::vector<double> getMax();
    std::vector<double> getMin();
    QLineEdit *EndT,*StartT,*epsilon,*step,*generation,*PPGnumber,*Gnumber,*Threadnumber;
    std::vector<QLineEdit *> max,min;
signals:
    void resultReady(std::vector<double>);
public slots:
    void endCalculate();
    void startCalculate();
    void logUpdate(QString theLog);
    void clearLog();
private:
    PSO *pso{nullptr};
    std::function<double(std::vector<double>)> f;
    QTextEdit *log;
    QPushButton *start;
    QVBoxLayout* labelAndLineEdit(QLineEdit * & line,QString context,QString label);
    QHBoxLayout* maxMin(QString context,double defaultmin,double defaultmax);
};

#endif // PSOWIDGET_H
