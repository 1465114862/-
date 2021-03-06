#-------------------------------------------------
#
# Project created by QtCreator 2018-11-23T20:58:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = orbit_github
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    calculator.cpp \
    clockfortest.cpp \
    intidata.cpp \
    ipsk4.cpp \
    main.cpp \
    mainwindow.cpp \
    maneuver.cpp \
    mathused.cpp \
    planet.cpp \
    plot.cpp \
    plotwidget.cpp \
    probe.cpp \
    reference.cpp \
    singlemaneuwinget.cpp \
    singlewinget.cpp \
    slidernum.cpp \
    spline.cpp \
    abstractpoint.cpp \
    splineiterator.cpp \
    Swarm.cpp \
    psowidget.cpp

HEADERS += \
    calculator.h \
    clockfortest.h \
    intidata.h \
    ipsk4.h \
    mainwindow.h \
    maneuver.h \
    mathused.h \
    planet.h \
    plot.h \
    plotwidget.h \
    probe.h \
    reference.h \
    singlemaneuwinget.h \
    singlewinget.h \
    slidernum.h \
    spline.h \
    abstractpoint.h \
    splineiterator.h \
    Swarm.h \
    psowidget.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
