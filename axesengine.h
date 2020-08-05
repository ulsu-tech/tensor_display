#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

class AxesEngine: private QOpenGLFunctions
{
public:
    AxesEngine();
    virtual ~AxesEngine();

    virtual void drawGeometry(QOpenGLShaderProgram *program);

protected:
    void initGeometry();

    QOpenGLBuffer arrayBuf;
    QOpenGLBuffer indexBuf;
};

