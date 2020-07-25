// window.cpp

#include <QtWidgets>
#include "window.h"
#include "ui_window.h"

#include "myglwidget.h"
#include <math.h>
#include <iostream>

Window::Window(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Window)
{
    ui->setupUi(this);

    connect(ui->myGLWidget, SIGNAL(xRotationChanged(int)), ui->rotXSlider, SLOT(setValue(int)));
    connect(ui->myGLWidget, SIGNAL(yRotationChanged(int)), ui->rotYSlider, SLOT(setValue(int)));
    connect(ui->myGLWidget, SIGNAL(zRotationChanged(int)), ui->rotZSlider, SLOT(setValue(int)));

    periodic_timer = new QTimer(this);
    connect(periodic_timer, SIGNAL(timeout()), this, SLOT(updateThings()));
    periodic_timer->start(50);
    counter = 0;
    connect(this, SIGNAL(newData(int,int, int)),
            ui->myGLWidget, SLOT(appendNewVector(int, int, int)));
    connect(this, SIGNAL(forceRedraw()), ui->myGLWidget, SLOT(redraw()));
}

Window::~Window()
{
    periodic_timer->stop();
    delete periodic_timer;
    delete ui;
}

void Window::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        close();
    else
        QWidget::keyPressEvent(e);
}

void shiftZero( int &x, int &y, int &z)
{
    x -= 2048;
    y -= 2048;
    z -= 2048;
}

void Window::updateThings()
{
    //TODO implement also reading of serial data. for now only will
    // push data to GL widget
    double l = cos(4. * counter / 180. * M_PI) * 2048;
    int x = sin(3.*counter / 180. * M_PI) * l + 2048;
    int y = sin(counter / 180. * M_PI) * l + 2048;
    int z = sin(2.* counter / 180. * M_PI) * l + 2048;
    shiftZero(x,y,z);
    std::cout<<"emitting x="<<x<<"  y="<<y<<"  z="<<z<<std::endl;
    emit newData(x, y, z);
    emit forceRedraw();   
    counter++;
}
