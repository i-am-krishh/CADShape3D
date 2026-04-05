#pragma once
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <vector>

class GeometryEngine : protected QOpenGLFunctions {
public:
    GeometryEngine();
    ~GeometryEngine();

    // 2D
    void drawSquare(float side, QOpenGLShaderProgram* p, bool selected = false);
    void drawRect(float w, float h, QOpenGLShaderProgram* p, bool selected = false);
    void drawCircle(float radius, QOpenGLShaderProgram* p, bool selected = false);

    // 3D
    void drawCube(float side, QOpenGLShaderProgram* p, bool selected = false);
    void drawCuboid(float w, float h, float d, QOpenGLShaderProgram* p, bool selected = false);
    void drawSphere(float radius, QOpenGLShaderProgram* p, bool selected = false);

private:
    struct VD { float x, y, z, nx, ny, nz; };
    static VD V(float x, float y, float z, float nx, float ny, float nz)
    {
        return { x,y,z,nx,ny,nz };
    }

    void upload(const void* data, int bytes);
    void bindAttribs(QOpenGLShaderProgram* p);
    void drawHandles(const std::vector<QVector2D>& pts, float dotSize,
        QOpenGLShaderProgram* p);

    QOpenGLBuffer m_vbo;
    QOpenGLBuffer m_ibo;
    QOpenGLVertexArrayObject m_vao;
};