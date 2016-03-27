#-------------------------------------------------
#
# Project created by QtCreator 2015-11-15T20:14:53
#
#-------------------------------------------------

QT       += core gui network xml xmlpatterns positioning

CONFIG +=  c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NWeather
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    radarproduct.cpp \
    radarimage.cpp \
    currentconditions.cpp \
    weatherstation.cpp \
    settingsdialog.cpp \
    pages.cpp \
    scnetworksccessmanager.cpp \
    radartreewidget.cpp \
    initsettingsdialog.cpp

HEADERS  += mainwindow.h \
    radarproduct.h \
    radarimage.h \
    currentconditions.h \
    weatherstation.h \
    reqtype.h \
    settingsdialog.h \
    pages.h \
    scnetworkaccessmanager.h \
    weatherutil.h \
    radartreewidget.h \
    initsettingsdialog.h

FORMS    += mainwindow.ui

RESOURCES += \
    weather.qrc
