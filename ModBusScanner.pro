#-------------------------------------------------
#
# Project created by QtCreator 2017-03-14T15:23:27
#
#-------------------------------------------------

QT       += core gui serialbus serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ModBusScanner
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    dialogcomport.cpp \
    dialogrecord.cpp \
    mymodel.cpp \
    mypoolingclass.cpp \
    dialogdisplayparams.cpp \
    dialogwriteitm.cpp

HEADERS  += mainwindow.h \
    dialogcomport.h \
    dialogrecord.h \
    mymodel.h \
    processingmanager.h \
    mypoolingclass.h \
    dialogdisplayparams.h \
    displaytypes.h \
    dialogwriteitm.h

FORMS    += mainwindow.ui \
    dialogcomport.ui \
    dialogrecord.ui \
    dialogdisplayparams.ui \
    dialogwriteitm.ui
