#include "axesengine.h"

#include <QVector2D>
#include <QVector3D>

struct VertexData
{
    QVector3D position;
    QVector2D texCoord;
};

//! [0]
AxesEngine::AxesEngine()
    : indexBuf(QOpenGLBuffer::IndexBuffer)
{
    initializeOpenGLFunctions();

    // Generate 2 VBOs
    arrayBuf.create();
    indexBuf.create();

    // Initializes cube geometry and transfers it to VBOs
    initGeometry();
}

AxesEngine::~AxesEngine()
{
    arrayBuf.destroy();
    indexBuf.destroy();
}

void AxesEngine::initGeometry()
{
    // For cube we would need only 8 vertices but we have to
    // duplicate vertex for each face because texture coordinate
    // is different.
    VertexData vertices[] = {
        // Vertex data for face 0
        {QVector3D(.0f, -0.01f,  -0.01f), QVector2D(0.0f, 0.0f)},  // v0
        {QVector3D( 5.0f, -0.01f,  -0.01f), QVector2D(1.f, 0.0f)}, // v1
        {QVector3D(0.0f,  -0.01f,  0.01f), QVector2D(0.0f, 0.5f)},  // v2
        {QVector3D( 5.0f,  -0.01f,  0.01f), QVector2D(1.f, 0.5f)}, // v3

        // Vertex data for face 1
        {QVector3D( 0.0f, 0.01f,  0.01f), QVector2D( 0.0f, 0.5f)}, // v4
        {QVector3D( 5.0f, 0.01f, 0.01f), QVector2D(0.33f, 0.5f)}, // v5

        // Vertex data for face 2
        {QVector3D( 0.0f,  0.01f,  -0.01f), QVector2D(0.0f, 1.0f)},  // v6
        {QVector3D( 5.0f,  0.01f, -0.01f), QVector2D(0.33f, 1.0f)}, // v7

        // Vertex data for face 3
        {QVector3D(.0f, -0.01f,  -0.01f), QVector2D(0.0f, 0.0f)},  // v0
        {QVector3D( 5.0f, -0.01f,  -0.01f), QVector2D(1.0f, 0.0f)}, // v1
    };

    // Indices for drawing cube faces using triangle strips.
    // Triangle strips can be connected by duplicating indices
    // between the strips. If connecting strips have opposite
    // vertex order then last index of the first strip and first
    // index of the second strip needs to be duplicated. If
    // connecting strips have same vertex order then only last
    // index of the first strip needs to be duplicated.
    GLushort indices[] = {
         0,  1,  2,  3,       // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
         4,    5,  6,   7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
         8,   9};

//! [1]
    // Transfer vertex data to VBO 0
    arrayBuf.bind();
    arrayBuf.allocate(vertices, 10 * sizeof(VertexData));

    // Transfer index data to VBO 1
    indexBuf.bind();
    indexBuf.allocate(indices, 10 * sizeof(GLushort));
//! [1]
}

//! [2]
void AxesEngine::drawGeometry(QOpenGLShaderProgram *program)
{
    // Tell OpenGL which VBOs to use
    arrayBuf.bind();
    indexBuf.bind();

    // Offset for position
    quintptr offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));

    // Draw cube geometry using indices from VBO 1
    glDrawElements(GL_TRIANGLE_STRIP, 10, GL_UNSIGNED_SHORT, nullptr);
}

