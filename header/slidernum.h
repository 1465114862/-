#ifndef SLIDERNUM_H
#define SLIDERNUM_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QTimer;
class QSlider;
class QLineEdit;
class QLabel;
QT_END_NAMESPACE

class MainWindow;

class slidernum : public QWidget
{
    Q_OBJECT

public:
    slidernum(double *iv,double it,QString ina);
public slots:
    void changeValue();
    void changeTimes();
    void addChange(double iadd);
    void timerCreate();
    void timerDisable();
    void textChangeValue();
    void updateValue();
signals:
    void valueChanged(QString num);
private:
    double *value,add{0},times{10};
    QTimer *timer;
    QSlider *slider;
    QLineEdit *lineEdit,*lineEdit2;
    QLabel *label,*label2;
    QString name;
};

#endif // SLIDERNUM_H
