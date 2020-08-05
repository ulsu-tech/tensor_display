#-------------------------------------------------
#
# Project created by QtCreator 2013-09-25T09:11:42
#
#-------------------------------------------------

QT       += core gui opengl
QT += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OpenGLExample
TEMPLATE = app


SOURCES += main.cpp\
        window.cpp \
        axesengine.cpp \
    myglwidget.cpp

HEADERS  += window.h \
        axesengine.h \
    myglwidget.h

FORMS    += window.ui

RESOURCES += shaders.qrc
