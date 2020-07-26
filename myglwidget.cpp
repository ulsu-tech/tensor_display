// myglwidget.cpp

#include <QtWidgets>
#include <QtOpenGL>

#include "myglwidget.h"

#include <iostream>

MyGLWidget::MyGLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    xRot = 0;
    xRot = -60;
    yRot = 0;
    zRot = 0;
    zRot = -120;
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
        updateGL();
    }
}

void MyGLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void MyGLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

void MyGLWidget::initializeGL()
{
    qglClearColor(Qt::darkGreen);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    static GLfloat lightPosition[4] = { 10, 10, 10, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

void MyGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -20.0);
    glRotatef(xRot, 1.0, 0.0, 0.0);
    glRotatef(yRot, 0.0, 1.0, 0.0);
    glRotatef(zRot, 0.0, 0.0, 1.0);
    draw();
}

void MyGLWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES_1
    glOrthof(-2, +2, -2, +2, 1.0, 15.0);
#else
    glOrtho(-5, +5, -5, +5, 1.0, 35.0);
#endif
    glMatrixMode(GL_MODELVIEW);
}

void MyGLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    return;
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + 8 * dy);
        setYRotation(yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot + 8 * dy);
        setZRotation(zRot + 8 * dx);
    }

    lastPos = event->pos();
}

void drawLineFromZeroTo( double x, double y, double z)
{

    glBegin(GL_LINES);
        glVertex3f(0,0,0);
        glVertex3f(x,y,z);
    glEnd();
}

void drawOrth(double x, double y, double z)
{
    double norm = sqrt( x*x + y*y + z*z);
    if (norm == 0) return;
    double gamma = M_PI/2.;
    if (x != 0) {
       gamma = - atan( y/x);
    }
    //3-f vector is ( - z/norm * cos(gamma); - z / norm * sin(gamma); x/norm * sin(gamma) - y/norm * cos(gamma) )
    double w = 0.01;
    double h = 0.01;
    glBegin(GL_TRIANGLE_STRIP);
      glColor4s( 1., 0., 0., 1.);
        glVertex3f(0 + sin(gamma)*w - h * ( -z/norm * cos(gamma)) ,  //1
            0 + cos(gamma)*w - h * ( -z/norm * sin(gamma)),
            0 + 0*w - h/norm*(x * cos(gamma) - y * sin(gamma)) );
        glVertex3f(x + sin(gamma)*w - h * ( -z/norm * cos(gamma)) ,  //2
            y + cos(gamma)*w - h * ( -z/norm * sin(gamma)),
            z + 0*w - h/norm*(x * cos(gamma) - y * sin(gamma)) );
        glVertex3f(0 + sin(gamma)*w + h * ( -z/norm * cos(gamma)) ,  //3
            0 + cos(gamma)*w + h * ( -z/norm * sin(gamma)), 
            0 + 0*w + h/norm*(x * cos(gamma) - y * sin(gamma)) );
        glVertex3f(x + sin(gamma)*w + h * ( -z/norm * cos(gamma)) ,  //4
            y + cos(gamma)*w + h * ( -z/norm * sin(gamma)),
            z + 0*w + h/norm*(x * cos(gamma) - y * sin(gamma)) );
        glVertex3f(0 - sin(gamma)*w + h * ( -z/norm * cos(gamma)) ,  //5
            0 - cos(gamma)*w + h * ( -z/norm * sin(gamma)),
            0 - 0*w + h/norm*(x * cos(gamma) - y * sin(gamma)) );
        glVertex3f(x - sin(gamma)*w + h * ( -z/norm * cos(gamma)) ,  //6
            y - cos(gamma)*w + h * ( -z/norm * sin(gamma)),
            z - 0*w + h/norm*(x * cos(gamma) - y * sin(gamma)) );
        glVertex3f(0 - sin(gamma)*w - h * ( -z/norm * cos(gamma)) ,  //7
            0 - cos(gamma)*w - h * ( -z/norm * sin(gamma)),
            0 - 0*w - h/norm*(x * cos(gamma) - y * sin(gamma)) );
        glVertex3f(x - sin(gamma)*w - h * ( -z/norm * cos(gamma)) ,  //8
            y - cos(gamma)*w - h * ( -z/norm * sin(gamma)),
            z - 0*w - h/norm*(x * cos(gamma) - y * sin(gamma)) );
        glVertex3f(0 + sin(gamma)*w - h * ( -z/norm * cos(gamma)) ,  //1
            0 + cos(gamma)*w - h * ( -z/norm * sin(gamma)),
            0 + 0*w - h/norm*(x * cos(gamma) - y * sin(gamma)) );
        glVertex3f(x + sin(gamma)*w - h * ( -z/norm * cos(gamma)) ,  //2
            y + cos(gamma)*w - h * ( -z/norm * sin(gamma)),
            z + 0*w - h/norm*(x * cos(gamma) - y * sin(gamma)) );
    glEnd();
}

void MyGLWidget::draw()
{
    qglColor(Qt::darkRed);

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

    drawOrth(5, 0, 0);
    drawOrth(0, 5, 0);
    drawOrth(0, 0, 5);

    for(auto &repCoord : reportedCoords)
    {
        drawLineFromZeroTo(
            scaleToSphere(repCoord.x),
            scaleToSphere(repCoord.y),
            scaleToSphere(repCoord.z));
    }
}

void MyGLWidget::redraw()
{
  ///std::cout<<"redraw called"<<std::endl;
    updateGL();
}

void MyGLWidget::appendNewVector(int x, int y, int z)
{
//    std::cout<<"on appendNewVector received x="<<x<<" y="<<y<<"  z="<<z<<std::endl;
    reportedCoords.insert(reportedCoords.begin(), {x,y,z});
    
//    std::cout<<"after we have received x="<<reportedCoords.front().x<<
//            " y="<<reportedCoords.front().y<<
//            "  z="<<reportedCoords.front().z<<std::endl;
    reportedCoords.resize(50);
}

void MyGLWidget::switchPyramid(bool nv)
{
    ShowPyramids = nv;
  std::cout<<"ShowPyramids set to "<<ShowPyramids<<std::endl;
}
