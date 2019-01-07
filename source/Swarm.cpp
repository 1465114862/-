//
// Created by Justin Jia on 12/16/18.
//

#include <iostream>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <stdexcept>
#include <float.h>
#include <windows.h>

#include "Swarm.h"

#define randf (rand()%10000/10000.0) //宏定义生成随机数


using std::vector;
using std::cout;
using std::pow;
using std::log;

/*
 * 局部变量
 * i : 维数，坐标序号
 * j : 粒子序号
 * k : 分组序号
 * t : 迭代次数
 */
void RunnableFromLambda::setLambda(std::function<void()> &&f) {
    func=f;
}
void RunnableFromLambda::run() {
    func();
}

Particle::Particle(int DIM){
    x = new vector<double> (DIM);
    v = new vector<double> (DIM);
    Pbestx = new vector<double> (DIM);
    Lbestx = new vector<double> (DIM);
}

void PSO::DefineParticle(){
    for(int j=0; j < NUM; j++){
        swarm.push_back(Particle(DIM));
    }
    Gbestx = new vector <double> (DIM);

    //New:
    mean_1 = new vector <double> (DIM);
    mean_2 = new vector <double> (DIM);
    s_1 = new vector <double> (DIM);
    s_2 = new vector <double> (DIM);
    s_R = new vector <double> (DIM);
    s_L = new vector <double> (DIM);

}

void PSO::Initialize(){
    overClo.start();
    DefineParticle();

    vmax = new vector<double> (DIM);
    const double k = 0.2;//定义粒子速度上限参数k
    for(int i=0; i < DIM; i++){
        vmax -> at(i) = k * (xmax.at(i) - xmin.at(i));
    }//赋值最大速度

    Gbestf = -DBL_MAX;

    for(int j=0; j < NUM; j++){
        Particle *pp = &swarm.at(j);//定义粒子指针Particle Pointer: pp
        for(int i=0; i < DIM; i++){
            pp -> x -> at(i) = randf * (xmax[i] - xmin[i]) + xmin[i];
            pp -> Pbestx -> at(i) = randf * (xmax[i] - xmin[i]) + xmin[i];
            pp -> v -> at(i) =(2 * randf -1) * vmax -> at(i);
        }
        pp -> Pbestf = -DBL_MAX;
    }//随机初始化粒子位置，速度，历史最优，平均值，平方平均值
    wmax=0.9;
    wmin=0.4;
    w=wmax;
    c1=2;
    c2=2;
    c3=0;

    t=1;

    w = wmax - t*(wmax - wmin)/T;
    c2 = 2.0*(T - t)/T;
    c3 = 2.0*t/T;

    connect(this,&PSO::doCalculateAllf,this,&PSO::calculateAllf);
    connect(this,&PSO::syncOver,this,&PSO::syncThread);
    connect(this,&PSO::doEvolve,this,&PSO::Evolve);

    runnables.clear();
    for(int j=0; j<NUM;j++) {
        runnables.emplace_back();
        runnables.at(j).setLambda([&,j]()->void{swarm[j].f = f (*(swarm[j]. x));emit syncOver();});
        runnables.at(j).setAutoDelete(false);
    }

    emit doCalculateAllf();
}

void PSO::syncThread() {
    mutex.lock();
    finishedThread++;
    if(finishedThread==NUM){
        emit doEvolve();
    }
    mutex.unlock();
    //emit toLog("1");clo.finish()
}

void PSO::calculateAllf() {
    finishedThread=0;
    for(int j=0; j<NUM;j++) {
        ThreadPool.start(&runnables.at(j));
        Sleep(1);
    }
}

void PSO::Evolve(){

    /*double wmax {0.9};
    double wmin {0.4};
    double w {wmax};
    double c1 { 2 }; //历史最优权重
    double c2 { 2 }; //局部最优权重
    double c3 { 0 }; //全局最优权重*/


        //更新粒子历史最优:
        for(int j=0; j < NUM; j++){
            Particle *pp = &swarm[j];
            if(pp -> f > pp -> Pbestf){
                pp -> Pbestf = pp -> f;
                for(int i=0; i < DIM; i++){
                    pp -> Pbestx -> at(i) = pp -> x -> at(i);
                }
            }
        }

        //更新各组局部最优
        //const int l = 10;
        const int GroupNUM = NUM / l;
        for(int k=0; k < GroupNUM; k++){
            vector<double> Ltempx(DIM);
            double Ltempf;

            for(int j=k*l; j < (k+1)*l; j++){
                Particle *pp = &swarm.at(j);
                if(j == k*l){
                    for(int i=0; i < DIM; i++){
                        Ltempx[i] = pp -> x -> at(i);
                        Ltempf = pp -> Pbestf;
                    }
                }
                else{
                    if(pp -> Pbestf > Ltempf){
                        for(int i=0; i < DIM; i++){
                            Ltempx[i] = pp -> x -> at(i);
                            Ltempf = pp -> Pbestf;
                        }
                    }
                }
            }//先搜索一组里的最优，保存在Ltemp中
            for(int j=k*l; j < (k+1)*l; j++){//再更新局部最优
                Particle *pp = &swarm[j];
                for(int i=0; i < DIM; i++){
                    pp -> Lbestx -> at(i) = Ltempx[i];
                    pp -> Lbestf = Ltempf;
                }
            }
        }

        //更新全局最优:
        for(int j=0; j < NUM; j++){
            Particle *pp = &swarm[j];
            if(pp -> f > Gbestf){
                Gbestf = pp -> f;
                for(int i=0; i < DIM; i++){
                    Gbestx -> at(i) = pp -> x -> at(i);
                }
            }
        }
        //输出测试：

        //cout <<1<< "\n";
        QString temp{""},numTemp;
        numTemp.setNum(t);
        temp=temp+"第 "+numTemp+" 代，最优值 ： ";
        numTemp.setNum(Gbestf,'g',15);
        temp=temp+numTemp+"\t"+"所用时间： ";
        //numTemp.setNum(tim,'g',15);
        //temp=temp+numTemp;
        emit toLog(temp);

        //New,计算平均值，方差
        for(int i = 0; i < DIM; i++){
            mean_1 -> at(i) = 0;
            mean_2 -> at(i) = 0;
            for(int j = 0; j < NUM; j++){
                Particle *pp = &swarm[j];
                mean_1 -> at(i) += pp -> x -> at(i);
                mean_2 -> at(i) += pow(pp -> x -> at(i), 2);
            }
            mean_1 -> at(i) = mean_1 -> at(i) / NUM;
            mean_2 -> at(i) = mean_2 -> at(i) / NUM;
            s_2 -> at(i) = mean_2 -> at(i) - pow(mean_1 -> at(i), 2);
            if(s_2 -> at(i) < 0) { throw std::runtime_error { "Square variance < 0" }; }
            s_1 -> at(i) = std::max( sqrt(s_2 -> at(i)), smin[i]/2.0 );
        }

        //New，更新参数 a, 边界
        if(t == T) { a = amax;}
        else { a = amin + (amax - amin)*t/T;}

        vector<double> Lxmax (DIM);
        vector<double> Lxmin (DIM);
        //搜索 Lbestx 中的最大值和最小值，其中应包括 Gbestx：
        for(int i=0; i<DIM; i++){
            Lxmax[i] = swarm[0].x -> at(i);
            Lxmin[i] = swarm[0].x -> at(i);
        }
        for(int i=0; i < DIM; i++){
            for(int k=0; k < GroupNUM; k++){
                const int j { k*l + 1};
                Particle *pp = &swarm[j];
                if( pp -> Lbestx -> at(i) > Lxmax[i]) { Lxmax[i] = pp -> Lbestx -> at(i); }
                if( pp -> Lbestx -> at(i) < Lxmin[i]) { Lxmin[i] = pp -> Lbestx -> at(i); }
            }
        }
        for(int i=0; i<DIM; i++){
            s_R -> at(i) = std::min(xmax[i] - mean_1->at(i),
                    std::max((Lxmax[i] - mean_1->at(i)), sqrt(-2 * pow(s_1->at(i), 2) * log(a))));
            s_L -> at(i) = std::min(mean_1->at(i) - xmin[i],
                    std::max((mean_1->at(i) - Lxmin[i]), sqrt(-2 * pow(s_1->at(i), 2) * log(a))));
            if(-2 * pow(s_1->at(i), 2) * std::log(a) < 0) {
                throw std::runtime_error{"Square root error"};
            }
        }


        //更新粒子速度，位置：
        for(int j=0; j < NUM; j++){
            Particle *pp = &swarm[j];
            for(int i=0; i < DIM; i++){
                pp -> v -> at(i) = w*(pp -> v -> at(i))
                        + randf * c1*(pp -> Pbestx -> at(i) - pp -> x -> at(i))
                        + randf *(c2*(pp -> Lbestx -> at(i) - pp -> x -> at(i))
                        + c3*(Gbestx -> at(i) - pp -> x -> at(i)));
                //速度更新方程
                if(pp -> v -> at(i) > vmax -> at(i)){
                    pp -> v -> at(i) = vmax -> at(i);
                }
                if(pp -> v -> at(i) < -(vmax -> at(i)) ){
                    pp -> v -> at(i) = -(vmax -> at(i));
                }//限制粒子最大速度，超出则取最大速度
                pp -> x -> at(i) = pp -> x -> at(i) + pp -> v -> at(i);
                //New limitation of position
                if(pp -> x -> at(i) > mean_1 -> at(i) + s_R -> at(i))
                    pp -> x -> at(i) = mean_1 -> at(i) + s_R -> at(i);
                if(pp -> x -> at(i) < mean_1 -> at(i) - s_L -> at(i))
                    pp -> x -> at(i) = mean_1 -> at(i) - s_L -> at(i);
                //限制粒子位置，超出则限制在边界上
            }
        }

        t++;

        if(t>T){
            temp="总用时： ";
            numTemp.setNum(overClo.finish());
            temp=temp+numTemp;
            emit toLog(temp);
            emit done();return;}

        w = wmax - t*(wmax - wmin)/T;
        c2 = 2.0*(T - t)/T;
        c3 = 2.0*t/T;

        emit doCalculateAllf();
    //}
}

PSO::PSO(std::function<double(std::vector<double>)> TestFunction, vector<double> lower, vector<double> upper,
    int length, int generation, int particlePerGroup, int groupNumber , int iMaxThreadNum)
    : DIM(length) ,
      T(generation) ,
      NUM(particlePerGroup*groupNumber) ,
      l(groupNumber) ,
      MaxThreadNum(iMaxThreadNum)
    {
        f = TestFunction;
        ThreadPool.setMaxThreadCount(MaxThreadNum);//
        for(int i=0; i<length; i++){
            xmin.push_back(lower[i]);
            xmax.push_back(upper[i]);
            smin.push_back(e*(upper[i] - lower[i]));
        }
        std::srand ((unsigned int) time(NULL));
    }

void PSO::runPSO()
{
    try{
        Initialize();
    }catch (std::exception &ee){
        std::cerr << ee.what() <<"\n";
    }
}

double PSO::getMax()
{
    return Gbestf;
}

vector<double> PSO::getPosition()
{
    return (*Gbestx);
}

PSO::~PSO()
{
    delete Gbestx;
    delete vmax;
    delete mean_1;
    delete mean_2;
    delete s_1;
    delete s_2;
    delete s_L;
    delete s_R;
    for(int j=0; j < NUM; j++){
        delete swarm[j].x;
        delete swarm[j].v;
        delete swarm[j].Pbestx;
        delete swarm[j].Lbestx;
    }
}
