#include "singlewinget.h"
#include "plotwidget.h"
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QThread>
#include <iostream>
#include <QMutex>
SingleWinget::SingleWinget(MainWindow *mw,Calculator *icalculate,std::vector<std::string> itoPlot)
  : mainWindow(mw),
    calculate(icalculate),
    toPlot(itoPlot) ,
    rest(false) ,
    testcount(0) ,
    startT(0)
{
    thefunction=[&](std::vector<double> v)->double{
        testcount++;
        double count=testcount;
        SingleManeuver tempSM;
        Maneuver tempM;
        tempSM.t=v.at(0);
        tempSM.v[0]=v.at(1);
        tempSM.v[1]=v.at(2);
        tempSM.v[2]=v.at(3);
        tempM.add(tempSM);
        tempSM.t=v.at(4);
        tempSM.v[0]=v.at(5);
        tempSM.v[1]=v.at(6);
        tempSM.v[2]=v.at(7);
        tempM.add(tempSM);
        QString numtest;
        numtest.setNum(count);
        std::string name{numtest.toStdString()};
        calculate->newProbe(name);
        calculate->getProbe(name)->SetPhaseSpaceEndPosition(oldQ,oldP);
        calculate->getProbe(name)->maneuver=tempM;
        double output{(calculate->ReverseGamma3SolveProbe(*(calculate->getProbe(name)),psowidget->getstep(),psowidget->getepsilon(),psowidget->getStartT(),psowidget->getEndT())).first};
        calculate->deleteProbe(name);
        return -output;
    };
    reverseCut=[&]()->void{
            testcount++;
            QString numtest;
            numtest.setNum(testcount);
            std::string name{numtest.toStdString()};
            Maneuver maneus;
            for(manueIterator=manue.begin();manueIterator!=manue.end();manueIterator++){
                 maneus.add((*manueIterator)->getManeu());
            }
            calculate->newProbe(name);
            calculate->getProbe(name)->SetPhaseSpaceEndPosition(Q,P);
            calculate->getProbe(name)->maneuver=maneus;
            calculate->ReverseGamma3SolveProbe(*(calculate->getProbe(name)),step->text().toInt(),tolerance->text().toDouble(),tmin->text().toDouble(),startT,Q,P);
            startT=tmin->text().toDouble();
            for(int i=0;i<6;i++)
                PAndQ[i]->updateValue();
            calculate->deleteProbe(name);
    };
    p1="Earth";
    p2="Sun";
    p3=calculate->getOriginProbe()->name;
    origin=P1;
    rotate=P1toP2;
    P=new double[3];
    Q=new double[3];
    for(int i=0;i<3;i++){
        P[i]=calculate->getProbe(toPlot.at(0))->InitialVelocity[i];
        Q[i]=calculate->getProbe(toPlot.at(0))->InitialPosition[i];
    }
    PAndQ[0]=new slidernum(Q,1,"X");
    PAndQ[1]=new slidernum(Q+1,1,"Y");
    PAndQ[2]=new slidernum(Q+2,1,"Z");
    PAndQ[3]=new slidernum(P,1,"Vx");
    PAndQ[4]=new slidernum(P+1,1,"Vy");
    PAndQ[5]=new slidernum(P+2,1,"Vz");
    QVBoxLayout *PQlayout = new QVBoxLayout;
    for(int i=0;i<6;i++){
        PQlayout->addWidget(PAndQ[i]);
    }
    PQlayout->addStretch();
    QVBoxLayout *Combolayout = new QVBoxLayout;
    P1reset=new QPushButton(tr("聚焦于第一参照物"),this);
    connect(P1reset,&QPushButton::clicked,this,&SingleWinget::resetViewP1);
    P2reset=new QPushButton(tr("聚焦于第二参照物"),this);
    connect(P2reset,&QPushButton::clicked,this,&SingleWinget::resetViewP2);
    P1ComboBox = new QComboBox;
    P2ComboBox = new QComboBox;
    P3ComboBox = new QComboBox;
    OriginComboBox = new QComboBox;
    RotateComboBox = new QComboBox;
    MethodComboBox = new QComboBox;
    QLabel *p1lable=new QLabel(tr("第一参照物"));
    QLabel *p2lable=new QLabel(tr("第二参照物"));
    QLabel *p3lable=new QLabel(tr("考察物体"));
    QLabel *Originlable=new QLabel(tr("原点选择"));
    QLabel *Rotatelable=new QLabel(tr("旋转系选择"));
    Combolayout->addWidget(p1lable);
    Combolayout->addWidget(P1ComboBox);
    Combolayout->addWidget(P1reset);
    Combolayout->addWidget(p2lable);
    Combolayout->addWidget(P2ComboBox);
    Combolayout->addWidget(P2reset);
    Combolayout->addWidget(p3lable);
    Combolayout->addWidget(P3ComboBox);
    Combolayout->addWidget(Originlable);
    Combolayout->addWidget(OriginComboBox);
    Combolayout->addWidget(Rotatelable);
    Combolayout->addWidget(RotateComboBox);
    for(int i=0;i<calculate->getPlanetcount();i++) {
        P1ComboBox->addItem(QString::fromStdString(calculate->indexToPlanets(i)));
        P2ComboBox->addItem(QString::fromStdString(calculate->indexToPlanets(i)));
        P3ComboBox->addItem(QString::fromStdString(calculate->indexToPlanets(i)));
    }
    P3ComboBox->addItem(QString::fromStdString(calculate->getOriginProbe()->name));
    OriginComboBox->addItem(tr("第一参照物"));
    OriginComboBox->addItem(tr("第二参照物"));
    OriginComboBox->addItem(tr("两者质心"));
    RotateComboBox->addItem(tr("随两参照物连线旋转"));
    RotateComboBox->addItem(tr("不旋转"));
    MethodComboBox->addItem(tr("变步长"));
    MethodComboBox->addItem(tr("不变步长"));
    P1ComboBox->setCurrentIndex(calculate->planetToIndex(p1));
    P2ComboBox->setCurrentIndex(calculate->planetToIndex(p2));
    P3ComboBox->setCurrentText(QString::fromStdString(calculate->getOriginProbe()->name));
    OriginComboBox->setCurrentIndex(0);
    RotateComboBox->setCurrentIndex(0);
    MethodComboBox->setCurrentIndex(0);
    QHBoxLayout *SolverLayout=new QHBoxLayout;
    QVBoxLayout *StepLayout = new QVBoxLayout;
    stepLabel=new QLabel("最大步数");
    step=new QLineEdit("15000");
    StepLayout->addWidget(stepLabel);
    StepLayout->addWidget(step);
    QVBoxLayout *ToleranceLayout = new QVBoxLayout;
    QString num2;
    num2.setNum(0.0645462/1.5,'g',15);
    toleranceLabel=new QLabel("容差参数");
    tolerance=new QLineEdit(num2);
    ToleranceLayout->addWidget(toleranceLabel);
    ToleranceLayout->addWidget(tolerance);
    QVBoxLayout *TminLayout = new QVBoxLayout;
    num2.setNum(0);
    tminLabel=new QLabel("开始时间");
    tmin=new QLineEdit(num2);
    TminLayout->addWidget(tminLabel);
    TminLayout->addWidget(tmin);
    QVBoxLayout *TmaxLayout = new QVBoxLayout;
    num2.setNum(864*40500.0,'g',15);
    tmaxLabel=new QLabel("最大时间");
    tmax=new QLineEdit(num2);
    TmaxLayout->addWidget(tmaxLabel);
    TmaxLayout->addWidget(tmax);
    QVBoxLayout *MethodLayout = new QVBoxLayout;
    MethodLayout->addWidget(new QLabel("求解器方法"));
    MethodLayout->addWidget(MethodComboBox);
    SolverLayout->addLayout(StepLayout);
    SolverLayout->addLayout(ToleranceLayout);
    SolverLayout->addLayout(TminLayout);
    SolverLayout->addLayout(TmaxLayout);
    SolverLayout->addLayout(MethodLayout);
    plotwidget = new plotWidget(icalculate,itoPlot,p1,p2,origin,rotate,startT);
    plotwidget->setSolver(step->text().toInt(),tolerance->text().toDouble(),startT,tmax->text().toDouble(),MethodComboBox->currentIndex());
    connect(P1ComboBox, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(P1Changed(const QString &)));
    connect(P2ComboBox, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(P2Changed(const QString &)));
    connect(P3ComboBox, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(P3Changed(const QString &)));
    connect(OriginComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(OriginChanged(int)));
    connect(RotateComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(RotateChanged(int)));
    connect(MethodComboBox, SIGNAL(activated(int)),
            this, SLOT(MethodChanged(int)));
    QHBoxLayout *PQPlayout = new QHBoxLayout;
    PQPlayout->addWidget(plotwidget);
    PQPlayout->addLayout(Combolayout);
    PQPlayout->addLayout(PQlayout);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(SolverLayout);
    mainLayout->addLayout(PQPlayout);
    newManeu=new QPushButton(tr("增加变轨节点"),this);
    connect(newManeu,&QPushButton::clicked,this,&SingleWinget::addManueWidget);
    mainLayout->addWidget(newManeu);
    plot=new QPushButton(tr("重新绘制"),this);
    connect(plot,&QPushButton::clicked,this,&SingleWinget::replot);
    connect(plotwidget,&plotWidget::rePlotResultReady,plot,&QPushButton::setEnabled);
    mainLayout->addWidget(plot);
    resetView=new QPushButton(tr("重置视角"),this);
    connect(resetView,&QPushButton::clicked,plotwidget,&plotWidget::resetScaleAndShiftHint);
    mainLayout->addWidget(resetView);
    QLabel *saveFileNameLabel=new QLabel("保存轨道文件名：");
    mainLayout->addWidget(saveFileNameLabel);
    saveFileName=new QLineEdit("probe");
    mainLayout->addWidget(saveFileName);
    saveButton=new QPushButton(tr("保存轨道"),this);
    connect(saveButton,&QPushButton::clicked,this,&SingleWinget::save);
    mainLayout->addWidget(saveButton);
    QLabel *loadFileNameLabel=new QLabel("读取轨道文件名：");
    mainLayout->addWidget(loadFileNameLabel);
    loadFileName=new QLineEdit("probe");
    mainLayout->addWidget(loadFileName);
    loadButton=new QPushButton(tr("读取轨道"),this);
    connect(loadButton,&QPushButton::clicked,this,&SingleWinget::load);
    mainLayout->addWidget(loadButton);
    test=new QPushButton(tr("粒子群算法"),this);
    connect(test,&QPushButton::clicked,this,&SingleWinget::testsolt);
    mainLayout->addWidget(test);
    mainLayout->addStretch();
    setLayout(mainLayout);
    setWindowTitle(tr("Orbit"));
}

void SingleWinget::addManueWidget() {
    SingleManeuWinget *newm;
    manue.push_back(newm);
    manueIterator=manue.end();
    manueIterator--;
    *manueIterator=new SingleManeuWinget(*manueIterator);
}

void SingleWinget::resetViewP2() {
    plotwidget->resetScaleAndShiftByName(P2ComboBox->currentText().toStdString());
}
void SingleWinget::resetViewP1() {
    plotwidget->resetScaleAndShiftByName(P1ComboBox->currentText().toStdString());
}

void SingleWinget::reallyReplot(){
    plotwidget->setSolver(step->text().toInt(),tolerance->text().toDouble(),startT,tmax->text().toDouble(),MethodComboBox->currentIndex());
    calculate->getProbe(toPlot.at(0))->GetPhaseSpaceInitialPosition(Q,P);
    clearUpManue();
    Maneuver maneus;
    for(manueIterator=manue.begin();manueIterator!=manue.end();manueIterator++){
         maneus.add((*manueIterator)->getManeu());
    }
    calculate->getProbe(toPlot.at(0))->maneuver=maneus;
    plotwidget->rePlot();
}
void SingleWinget::replot() {
    plot->setEnabled(false);
    if(((startT-tmin->text().toDouble())/(tmax->text().toDouble()-tmin->text().toDouble())<0.001)&&((startT-tmin->text().toDouble())/(tmax->text().toDouble()-tmin->text().toDouble())>-0.001)){
        reallyReplot();
    }
    else {
        if(startT<tmin->text().toDouble()){
            startT=tmin->text().toDouble();
            cut();
            reallyReplot();
        }
        else {
            reverseCut();
            QThread *Thread=QThread::create(reverseCut);
            connect(Thread, &QThread::finished, this, &SingleWinget::reallyReplot);
            connect(Thread, &QThread::finished, Thread, &QObject::deleteLater);
            Thread->start();
        }
    }
}

void SingleWinget::cut(){
    for(int i=0;i<3;i++) {
        Q[i]=calculate->getOriginProbe()->value(i,startT);
        PAndQ[i]->updateValue();
        P[i]=calculate->getOriginProbe()->velocity(i,startT);
        PAndQ[i+3]->updateValue();
    }
}

void SingleWinget::pointToResultManue(std::vector<double> point) {
    tempPoint=point;
    QThread *toManue=QThread::create([&]()->void{
            testcount++;
            SingleManeuver tempSM;
            Maneuver tempM;
            tempSM.t=tempPoint.at(0);
            tempSM.v[0]=tempPoint.at(1);
            tempSM.v[1]=tempPoint.at(2);
            tempSM.v[2]=tempPoint.at(3);
            tempM.add(tempSM);
            tempSM.t=tempPoint.at(4);
            tempSM.v[0]=tempPoint.at(5);
            tempSM.v[1]=tempPoint.at(6);
            tempSM.v[2]=tempPoint.at(7);
            tempM.add(tempSM);
            QString numtest,temp;
            numtest.setNum(testcount);
            std::string name{numtest.toStdString()},tem{};
            calculate->newProbe(name);
            calculate->getProbe(name)->SetPhaseSpaceEndPosition(oldQ,oldP);
            calculate->getProbe(name)->maneuver=tempM;
            tempM.add((calculate->ReverseGamma3SolveProbe(*(calculate->getProbe(name)),psowidget->getstep(),psowidget->getepsilon(),psowidget->getStartT(),psowidget->getEndT())).second);
            resultManeu=tempM;
            calculate->getProbe(name)->maneuver=tempM;
            resultStartT=calculate->getProbe(name)->StartT-3600;
            calculate->ReverseGamma3SolveProbe(*(calculate->getProbe(name)),psowidget->getstep(),psowidget->getepsilon(),resultStartT,psowidget->getEndT(),newQ,newP);
            resultStartT=calculate->getProbe(name)->StartT;
            calculate->deleteProbe(name);
            saveResult();
            psowidget->logUpdate("保存结果完毕");
        });
    connect(toManue, &QThread::finished, toManue, &QObject::deleteLater);
    toManue->start();
}

void SingleWinget::testsolt() {
    psowidget=new PSOwidget(thefunction);
    connect(psowidget, &PSOwidget::resultReady, this, &SingleWinget::pointToResultManue);
    for(int i=0;i<3;i++) {
        oldP[i]=calculate->getProbe("OriginalProbe")->velocity(i,psowidget->getEndT());
        oldQ[i]=calculate->getProbe("OriginalProbe")->value(i,psowidget->getEndT());
    }
}

void SingleWinget::closeEvent(QCloseEvent *event) {
    for(manueIterator=manue.begin();manueIterator!=manue.end();manueIterator++){
        (*manueIterator)->close();
        delete (*manueIterator);
    }
    if(psowidget){psowidget->close();}
}

void SingleWinget::clearUpManue() {
    for(manueIterator=manue.begin();(manueIterator!=manue.end());){
        if(!(*manueIterator)){//此处曾有bug，指针未置空
            manue.erase(manueIterator++);
        }
        else {
            ++manueIterator;
        }
    }
}

void SingleWinget::P1Changed(const QString &name){
    p1=name.toStdString();
    plotwidget->setReference(p1,p2,p3,origin,rotate);
    if(!rest)
        plotwidget->reference();
}
void SingleWinget::P2Changed(const QString &name) {
    p2=name.toStdString();
    plotwidget->setReference(p1,p2,p3,origin,rotate);
    if(!rest)
        plotwidget->reference();
}
void SingleWinget::P3Changed(const QString &name) {
    p3=name.toStdString();
    plotwidget->setReference(p1,p2,p3,origin,rotate);
    switch (calculate->getType(p3)) {
    case probe:
        plot->setEnabled(true);
        saveButton->setEnabled(true);
        loadButton->setEnabled(true);
        break;
    case planet:
        plot->setEnabled(false);
        saveButton->setEnabled(false);
        loadButton->setEnabled(false);
        break;
    }
    plotwidget->reference();
}
void SingleWinget::OriginChanged(int index) {
    switch (index) {
    case 0:
        origin=P1;
        switch (RotateComboBox->currentIndex()) {
        case 0:
            break;
        case 1:
            P1reset->setEnabled(true);
            P2reset->setEnabled(false);
            break;
        }
        break;
    case 1:
        origin=P2;
        switch (RotateComboBox->currentIndex()) {
        case 0:
            break;
        case 1:
            P1reset->setEnabled(false);
            P2reset->setEnabled(true);
            break;
        }
        break;
    case 2:
        origin=CenterOfMass;
        switch (RotateComboBox->currentIndex()) {
        case 0:
            break;
        case 1:
            P1reset->setEnabled(false);
            P2reset->setEnabled(false);
            break;
        }
        break;
    }
    plotwidget->setReference(p1,p2,p3,origin,rotate);
    if(!rest)
        plotwidget->reference();
}
void SingleWinget::RotateChanged(int index) {
    switch (index) {
    case 0:
        rotate=P1toP2;
        P1reset->setEnabled(true);
        P2reset->setEnabled(true);
        break;
    case 1:
        rotate=NoneRotate;
        switch (OriginComboBox->currentIndex()) {
        case 0:
            P2reset->setEnabled(false);
            break;
        case 1:
            P1reset->setEnabled(false);
            break;
        case 2:
            P1reset->setEnabled(false);
            P2reset->setEnabled(false);
            break;
        }
        break;
    }
    plotwidget->setReference(p1,p2,p3,origin,rotate);
    if(!rest)
        plotwidget->reference();
}
void SingleWinget::MethodChanged(int index) {
    QString num2;
    switch (index) {
    case 0:
        toleranceLabel->setText("容差参数");
        num2.setNum(15000);
        step->setText(num2);
        num2.setNum(0.0645462/1.5,'g',15);
        tolerance->setText(num2);
        num2.setNum(864*40500.0,'g',15);
        tmax->setText(num2);
        break;
    case 1:
        toleranceLabel->setText("步长");
        num2.setNum(15000);
        step->setText(num2);
        num2.setNum(8640.0,'g',15);
        tolerance->setText(num2);
        num2.setNum(864*40500.0,'g',15);
        tmax->setText(num2);
        break;
    }
}

void SingleWinget::saveResult() {
    try {
        std::string str=calculate->getLocation()+R"(\result.dat)";
        const char *location=str.data();
        remove(location);
        std::fstream outdata;
        outdata.open(str, std::ios::out | std::ios::binary);
        if (!outdata.is_open())
            throw std::runtime_error{"无法保存轨道数据"};
        int intcache;
        double doublecache;
        intcache=P1ComboBox->currentIndex();
        outdata.write((const char *)&intcache, sizeof(int));
        intcache=P2ComboBox->currentIndex();
        outdata.write((const char *)&intcache, sizeof(int));
        intcache=OriginComboBox->currentIndex();
        outdata.write((const char *)&intcache, sizeof(int));
        intcache=RotateComboBox->currentIndex();
        outdata.write((const char *)&intcache, sizeof(int));
        intcache=0;
        outdata.write((const char *)&intcache, sizeof(int));
        intcache=psowidget->getstep();//
        outdata.write((const char *)&intcache, sizeof(int));
        doublecache=psowidget->getepsilon();//
        outdata.write((const char *)&doublecache, sizeof(double));
        doublecache=resultStartT;//
        outdata.write((const char *)&doublecache, sizeof(double));
        doublecache=psowidget->getEndT();//
        outdata.write((const char *)&doublecache, sizeof(double));
        for(int i=0;i<3;i++) {
            outdata.write((const char *)(newQ+i), sizeof(double));
            outdata.write((const char *)(newP+i), sizeof(double));
        }
        intcache=static_cast<int>(resultManeu.length);
        outdata.write((const char *)&intcache, sizeof(int));
        std::list <SingleManeuver> ::iterator resultIterator;
        for(resultIterator=resultManeu.maneuver.begin();resultIterator!=resultManeu.maneuver.end();resultIterator++){
            outdata.write((const char *)&(resultIterator->t), sizeof(double));
            outdata.write((const char *)(resultIterator->v), sizeof(double));
            outdata.write((const char *)(resultIterator->v+1), sizeof(double));
            outdata.write((const char *)(resultIterator->v+2), sizeof(double));
             (*manueIterator)->getManeu();
        }
        outdata.close();
    } catch (std::exception& e) {
        QMessageBox::information(0, tr("错误"), e.what());
        return;
    }
}
void SingleWinget::save() {
    try {
        std::string str=calculate->getLocation()+R"(\)"+saveFileName->text().toStdString()+R"(.dat)";
        const char *location=str.data();
        remove(location);
        std::fstream outdata;
        outdata.open(str, std::ios::out | std::ios::binary);
        if (!outdata.is_open())
            throw std::runtime_error{"无法保存轨道数据"};
        int intcache;
        double doublecache;
        intcache=P1ComboBox->currentIndex();
        outdata.write((const char *)&intcache, sizeof(int));
        intcache=P2ComboBox->currentIndex();
        outdata.write((const char *)&intcache, sizeof(int));
        intcache=OriginComboBox->currentIndex();
        outdata.write((const char *)&intcache, sizeof(int));
        intcache=RotateComboBox->currentIndex();
        outdata.write((const char *)&intcache, sizeof(int));
        intcache=MethodComboBox->currentIndex();
        outdata.write((const char *)&intcache, sizeof(int));
        intcache=step->text().toInt();
        outdata.write((const char *)&intcache, sizeof(int));
        doublecache=tolerance->text().toDouble();
        outdata.write((const char *)&doublecache, sizeof(double));
        doublecache=startT;
        outdata.write((const char *)&doublecache, sizeof(double));
        doublecache=tmax->text().toDouble();
        outdata.write((const char *)&doublecache, sizeof(double));
        for(int i=0;i<3;i++) {
            outdata.write((const char *)(Q+i), sizeof(double));
            outdata.write((const char *)(P+i), sizeof(double));
        }
        intcache=static_cast<int>(manue.size());
        outdata.write((const char *)&intcache, sizeof(int));
        for(manueIterator=manue.begin();manueIterator!=manue.end();manueIterator++){
            outdata.write((const char *)&((*manueIterator)->getManeu().t), sizeof(double));
            outdata.write((const char *)((*manueIterator)->getManeu().v), sizeof(double));
            outdata.write((const char *)((*manueIterator)->getManeu().v+1), sizeof(double));
            outdata.write((const char *)((*manueIterator)->getManeu().v+2), sizeof(double));
             (*manueIterator)->getManeu();
        }
        outdata.close();
    } catch (std::exception& e) {
        QMessageBox::information(0, tr("错误"), e.what());
        return;
    }
}
void SingleWinget::load() {
    try {
        std::fstream indata;
        indata.open(calculate->getLocation()+R"(\)"+loadFileName->text().toStdString()+R"(.dat)", std::ios::in | std::ios::binary);
        if (!indata.is_open())
            throw std::runtime_error{"无法读取轨道数据"};
        rest=true;
        int intcache;
        double doublecache;
        QString num2;
        indata.read((char *)&intcache, sizeof(int));
        P1ComboBox->setCurrentIndex(intcache);
        indata.read((char *)&intcache, sizeof(int));
        P2ComboBox->setCurrentIndex(intcache);
        indata.read((char *)&intcache, sizeof(int));
        OriginComboBox->setCurrentIndex(intcache);
        indata.read((char *)&intcache, sizeof(int));
        RotateComboBox->setCurrentIndex(intcache);
        num2.setNum(15000);
        indata.read((char *)&intcache, sizeof(int));
        MethodComboBox->setCurrentIndex(intcache);
        indata.read((char *)&intcache, sizeof(int));
        num2.setNum(intcache);
        step->setText(num2);
        indata.read((char *)&doublecache, sizeof(double));
        num2.setNum(doublecache,'g',15);
        tolerance->setText(num2);
        indata.read((char *)&doublecache, sizeof(double));
        startT=doublecache;
        num2.setNum(doublecache,'g',15);
        tmin->setText(num2);
        indata.read((char *)&doublecache, sizeof(double));
        num2.setNum(doublecache,'g',15);
        tmax->setText(num2);
        for(int i=0;i<3;i++) {
            indata.read((char *)(Q+i), sizeof(double));
            PAndQ[i]->updateValue();
            indata.read((char *)(P+i), sizeof(double));
            PAndQ[i+3]->updateValue();
        }
        indata.read((char *)&intcache, sizeof(int));
        clearUpManue();
        SingleManeuver manueCache;
        for(int i=0;i<intcache;i++) {
            indata.read((char *)&(manueCache.t), sizeof(double));
            indata.read((char *)(manueCache.v), 3*sizeof(double));
            addManueWidget();
            (*manueIterator)->setManeu(manueCache);
        }
        rest=false;
        indata.close();
    } catch (std::exception& e) {
        QMessageBox::information(0, tr("错误"), e.what());
        return;
    }
}
