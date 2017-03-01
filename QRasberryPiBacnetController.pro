#-------------------------------------------------
#
# Project created by QtCreator 2017-02-15T12:20:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


QMAKE_CXXFLAGS   += -std=c++11

TARGET = QRasberryPiBacnetController
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    VariablesDialog.cpp \
    VariablesModel.cpp \
    VariablesModelDelegate.cpp \
    PRGReader.cpp

HEADERS  += MainWindow.h \
    VariablesDialog.h \
    VariablesModel.h \
    Variable.h \
    VariablesModelDelegate.h \
    PRGReader.h

FORMS    += MainWindow.ui \
    VariablesDialog.ui
