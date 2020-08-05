// myglwidget.h

#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QOpenGLWidget>
#include <vector>
#include "axesengine.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>

struct tensorVec {
    int x, y, z;
};

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit MyGLWidget(QWidget *parent = 0);
    ~MyGLWidget();
    void findRotation(double &angleZ, double &angleY,const tensorVec &repCoord);
signals:

public slots:

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    inline double scaleToSphere( int v, double rad = 3.) {
        return rad / 2048. * v;
    }
    void initShaders();

public slots:
    // slots for xyz-rotation slider
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
    void redraw();
    void appendNewVector(int x, int y, int z);
    void switchPyramid(bool);

signals:
    // signaling rotation from mouse movement
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);

private:
    void draw();

    int xRot;
    int yRot;
    int zRot;

    QPoint lastPos;
    std::vector<tensorVec> reportedCoords;
    bool ShowPyramids;
    AxesEngine *axes = nullptr;
    QOpenGLShaderProgram program;
    QMatrix4x4 projection;
    QQuaternion rotation;
};

#endif // MYGLWIDGET_H

