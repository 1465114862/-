//
// Created by Justin Jia on 12/16/18.
//

#ifndef PSO_JSM1_2_SWARM_H
#define PSO_JSM1_2_SWARM_H

#include <iostream>
#include <cmath>
#include <vector>
#include <functional>
#include <QObject>
#include <QThread>
#include <clockfortest.h>
#include <QRunnable>
#include <QThreadPool>
#include <QMutex>

using std::vector;
using std::exp;
using std::pow;

const double PI {acos(-1)};

class Particle{
public:
    Particle(int DIM);
    vector<double> *x;
    vector<double> *v;
    double f;
    vector<double> *Pbestx;
    double Pbestf;
    vector<double> *Lbestx;
    double Lbestf;
};

class RunnableFromLambda :public QRunnable {
public:
    void setLambda(std::function<void()> &&f);
    void run() override;
private:
    std::function<void()> func{};
};

class PSO : public QObject {
    Q_OBJECT
signals:
    void syncOver();
    void done();
    void toLog(QString);
    void doEvolve();
    void doCalculateAllf();
public slots:
    void syncThread();
    void Evolve();
    void calculateAllf();
public:
    PSO(std::function<double(std::vector<double>)> TestFunction, vector<double> lower, vector<double> upper,
            int length, int generation, int particlePerGroup, int groupNumber, int iMaxThreadNum);
    void runPSO();
    double getMax();
    vector<double> getPosition();
    ~PSO();//
private:
    std::function<double(std::vector<double>)> f;
    vector<double> xmin;
    vector<double> xmax;
    const int DIM;
    const int T;
    const int NUM;

    const int l;
    const int MaxThreadNum;
    QMutex mutex;
    QThreadPool ThreadPool;

    double wmax;
    double wmin;
    double w;
    double c1;
    double c2;
    double c3;

    vector<Particle> swarm;
    vector<double> *Gbestx;
    double Gbestf;
    vector<double> *vmax;

    //New，about updating boundary while evolving:
    const double e { 0.1 }, amin { 0.001 }, amax { exp(-1.0/(2*pow(e,2))) };
    double a { amin };
    vector<double> *mean_1, *mean_2;//粒子平均位置，平方平均位置
    vector<double> *s_1, *s_2, *s_R, *s_L, smin;//方差，左右边界宽度

    int t;

    int finishedThread;
    vector<RunnableFromLambda> runnables;

    ClockForTest clo;
    ClockForTest overClo;

    double tim;

    void DefineParticle();
    void Initialize();
};

#endif //PSO_JSM1_2_SWARM_H
