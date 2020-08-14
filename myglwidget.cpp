// myglwidget.cpp

#include <QtWidgets>
#include <QtOpenGL>

#include "myglwidget.h"

#include <iostream>
#include <QDebug>

MyGLWidget::MyGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    xRot = 0;
    xRot = -60;
    yRot = 0;
    zRot = 0;
    zRot = -120;

    ShowPyramids = true;
}

MyGLWidget::~MyGLWidget()
{
}

QSize MyGLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize MyGLWidget::sizeHint() const
{
    return QSize(400, 400);
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360*16;
    while (angle > 360)
        angle -= 360* 16;
}

void MyGLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        update();
    }
}

void MyGLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        update();
    }
}

void MyGLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        update();
    }
}

void MyGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0.2, 0.6, 0.1, 1);

    initShaders();

//! [2]
    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);
//! [2]

    axes = new AxesEngine;

}

void MyGLWidget::initShaders()
{
    // Compile vertex shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl"))
        close();

    // Compile fragment shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl"))
        close();

    // Link shader pipeline
    if (!program.link())
        close();

    // Bind shader pipeline for use
    if (!program.bind())
        close();
}

void MyGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    QMatrix4x4 matrix;
    matrix.translate(0.0, 0.0, -10.0);
    rotation = QQuaternion::fromEulerAngles(xRot, yRot, zRot);
    matrix.rotate(rotation);

    program.setUniformValue("mvp_matrix", projection * matrix);

    QVector4D fColor( 0.4, 0.7, 0.1, 0.9);
    program.setUniformValue("f_color", fColor);

    // Draw cube geometry
    QMatrix4x4 object_rotate;
    object_rotate.setToIdentity(); 
    //object_rotate.rotate(90., 0., 1.);
    program.setUniformValue("rotate_object", object_rotate);
    //geometries->drawGeometry(&program);
    axes->drawGeometry(&program);

    object_rotate.rotate(-90., 0., 1.);
    program.setUniformValue("rotate_object", object_rotate);
    fColor.setX(1.); fColor.setY(0.);
    program.setUniformValue("f_color", fColor);
    axes->drawGeometry(&program);
    object_rotate.rotate(90., 0., 0., 1.);
    program.setUniformValue("rotate_object", object_rotate);
    fColor.setX(0.); fColor.setZ(1.);
    program.setUniformValue("f_color", fColor);
    axes->drawGeometry(&program);

    draw();
}

void MyGLWidget::resizeGL(int w, int h)
{
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 1.0, zFar = 17.0, fov = 45.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
}

void MyGLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + 1 * dy);
        setYRotation(yRot + 1 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot + 1 * dy);
        setZRotation(zRot + 1 * dx);
    }

    lastPos = event->pos();
}

void MyGLWidget::findRotation(double &angleZ, double &angleY,const tensorVec &repCoord)
{
    if ( abs(repCoord.x) < 1e-6) {
        angleZ = 90.;
        if( repCoord.y < 0) {
                angleZ = -90.;
        }
    } else {
        angleZ = 180. / M_PI * atan(1. *repCoord.y/repCoord.x);
        if (repCoord.x < 0) {
            angleZ += 180.;
        }
    }
    if ( abs(repCoord.x) < 1e-6 && abs(repCoord.y) < 1e-6 )
    {
        if (repCoord.z < 0)
            angleY = 90.;
        else
            angleY = -90.;
    } else {
        auto l_pr = sqrt(repCoord.x* repCoord.x + repCoord.y* repCoord.y);
        angleY = -180. / M_PI * atan(1.* repCoord.z/ l_pr);
    }
}

void MyGLWidget::draw()
{
/*
  if (ShowPyramids) {
    glBegin(GL_TRIANGLE_STRIP);
        //glNormal3f(0,-1,0.707);
        glNormal3f(0, -1, 0);
        qglColor(Qt::red);
        glVertex3f(0,0,0);
        glVertex3f(1,0,0);
        glVertex3f(0,0,3);
        glNormal3f(0.577, 0.577, 0.577);
        qglColor(Qt::darkRed);
        glVertex3f(0,2,0);
        glNormal3f(-1, 0, 0);
        qglColor(Qt::blue);
        glVertex3f(0,0,0);
        glNormal3f(0, 0, -1);
        qglColor(Qt::green);
        glVertex3f(1,0,0);
    glEnd();
  }
*/

    auto m = reportedCoords.size();
    int i=0;
    for(auto &repCoord : reportedCoords)
    {
        // how to rotate there?
        //repCoord.x, .y, .z
        double v = 1. - 1. * i / m;
        QVector4D fColor( v, 0.1, v, 1. - 0.9 * i / m );
        program.setUniformValue("f_color", fColor);

        QMatrix4x4 object_rotate;
        object_rotate.setToIdentity(); 
        // scale 1./5. * L
        object_rotate.scale( 3./5. * sqrt( repCoord.x*repCoord.x + repCoord.y*repCoord.y + repCoord.z * repCoord.z )/ 2048. );
        // x/y - определяют поворот вокруг z
        double angleZ = 0, angleY = 0;
        findRotation(angleZ, angleY, repCoord);
        object_rotate.rotate(angleZ, 0., 0., 1.);
        object_rotate.rotate(angleY, 0., 1.);
        //qDebug()<<"from x="<<repCoord.x<<" y="<<repCoord.y<<"   z="<<repCoord.z<<"    calced overZ="<<angleZ<<"   and overY'="<<angleY;

        program.setUniformValue("rotate_object", object_rotate);
        axes->drawGeometry(&program);
        ++i;
    }
}

void MyGLWidget::redraw()
{
  ///std::cout<<"redraw called"<<std::endl;
    update();
}

void MyGLWidget::appendNewVector(int x, int y, int z)
{
//    std::cout<<"on appendNewVector received x="<<x<<" y="<<y<<"  z="<<z<<std::endl;
    reportedCoords.insert(reportedCoords.begin(), {x,y,z});
    
//    std::cout<<"after we have received x="<<reportedCoords.front().x<<
//            " y="<<reportedCoords.front().y<<
//            "  z="<<reportedCoords.front().z<<std::endl;
    reportedCoords.resize(5);
}

void MyGLWidget::switchPyramid(bool nv)
{
    ShowPyramids = nv;
  std::cout<<"ShowPyramids set to "<<ShowPyramids<<std::endl;
}

