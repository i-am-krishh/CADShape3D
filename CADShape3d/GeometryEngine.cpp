#include "GeometryEngine.h"
#include <QVector2D>
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

GeometryEngine::GeometryEngine() {
    initializeOpenGLFunctions();
    m_vao.create();
    m_vbo.create(); m_vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    m_ibo.create(); m_ibo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
}

GeometryEngine::~GeometryEngine() {
    m_vao.destroy();
    m_vbo.destroy();
    m_ibo.destroy();
}

void GeometryEngine::upload(const void* data, int bytes) {
    m_vbo.bind();
    m_vbo.allocate(data, bytes);
}

void GeometryEngine::bindAttribs(QOpenGLShaderProgram* p) {
    m_vao.bind();
    m_vbo.bind();
    p->enableAttributeArray(0);
    p->setAttributeBuffer(0, GL_FLOAT, 0, 3, sizeof(VD));
    p->enableAttributeArray(1);
    p->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 3, sizeof(VD));
}

void GeometryEngine::drawHandles(const std::vector<QVector2D>& pts,
    float ds, QOpenGLShaderProgram* p) {
    std::vector<VD> v;
    v.reserve(pts.size() * 6);
    for (auto& c : pts) {
        float x = c.x(), y = c.y(), z = 0.002f;
        v.push_back(V(x - ds, y - ds, z, 0, 0, 1)); v.push_back(V(x + ds, y - ds, z, 0, 0, 1));
        v.push_back(V(x + ds, y + ds, z, 0, 0, 1)); v.push_back(V(x + ds, y + ds, z, 0, 0, 1));
        v.push_back(V(x - ds, y + ds, z, 0, 0, 1)); v.push_back(V(x - ds, y - ds, z, 0, 0, 1));
    }
    upload(v.data(), (int)(v.size() * sizeof(VD)));
    bindAttribs(p);
    glDrawArrays(GL_TRIANGLES, 0, (int)v.size());
    m_vao.release();
}

//  2D Shapes

void GeometryEngine::drawSquare(float side, QOpenGLShaderProgram* p, bool sel) {
    float s = side * 0.5f;
    VD v[4] = { V(-s,-s,0,0,0,1), V(s,-s,0,0,0,1),
                V(s, s,0,0,0,1), V(-s,s,0,0,0,1) };
    upload(v, sizeof(v));
    bindAttribs(p);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    m_vao.release();

    if (sel) {
        float ds = qBound(0.008f, s * 0.12f, 0.030f);
        std::vector<QVector2D> handles = {
            {-s, s},{s, s},{s,-s},{-s,-s},   // corners
            { 0, s},{s, 0},{0,-s},{-s, 0}    // edge midpoints
        };
        drawHandles(handles, ds, p);
    }
}

void GeometryEngine::drawRect(float w, float h, QOpenGLShaderProgram* p, bool sel) {
    float x = w * 0.5f, y = h * 0.5f;
    VD v[4] = { V(-x,-y,0,0,0,1), V(x,-y,0,0,0,1),
                V(x, y,0,0,0,1), V(-x,y,0,0,0,1) };
    upload(v, sizeof(v));
    bindAttribs(p);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    m_vao.release();

    if (sel) {
        float ds = qBound(0.008f, qMin(x, y) * 0.12f, 0.030f);
        std::vector<QVector2D> handles = {
            {-x, y},{x, y},{x,-y},{-x,-y},
            { 0, y},{x, 0},{0,-y},{-x, 0}
        };
        drawHandles(handles, ds, p);
    }
}

void GeometryEngine::drawCircle(float r, QOpenGLShaderProgram* p, bool sel) {
    const int segs = 64;
    std::vector<VD> v;
    v.reserve(segs);
    for (int i = 0; i < segs; ++i) {
        float a = 2.0f * (float)M_PI * i / segs;
        v.push_back(V(r * cosf(a), r * sinf(a), 0, 0, 0, 1));
    }
    upload(v.data(), (int)(v.size() * sizeof(VD)));
    bindAttribs(p);
    glDrawArrays(GL_LINE_LOOP, 0, segs);
    m_vao.release();

    if (sel) {
        float ds = qBound(0.008f, r * 0.12f, 0.030f);
        std::vector<QVector2D> handles;
        for (int i = 0; i < 8; ++i) {
            float a = 2.0f * (float)M_PI * i / 8;
            handles.push_back({ r * cosf(a), r * sinf(a) });
        }
        drawHandles(handles, ds, p);
    }
}

// 3D Shapes

void GeometryEngine::drawCube(float side, QOpenGLShaderProgram* p, bool) {
    float s = side * 0.5f;
    VD v[] = {
        V(-s,-s, s,0,0,1), V(s,-s, s,0,0,1), V(s, s, s,0,0,1),
        V(s, s, s,0,0,1),  V(-s,s, s,0,0,1), V(-s,-s, s,0,0,1),
        V(-s,-s,-s,0,0,-1),V(-s, s,-s,0,0,-1),V(s, s,-s,0,0,-1),
        V(s, s,-s,0,0,-1), V(s,-s,-s,0,0,-1), V(-s,-s,-s,0,0,-1),
        V(-s,-s,-s,-1,0,0),V(-s,-s, s,-1,0,0),V(-s, s, s,-1,0,0),
        V(-s, s, s,-1,0,0),V(-s, s,-s,-1,0,0),V(-s,-s,-s,-1,0,0),
        V(s,-s,-s,1,0,0),  V(s, s,-s,1,0,0),  V(s, s, s,1,0,0),
        V(s, s, s,1,0,0),  V(s,-s, s,1,0,0),  V(s,-s,-s,1,0,0),
        V(-s, s,-s,0,1,0), V(-s, s, s,0,1,0), V(s, s, s,0,1,0),
        V(s, s, s,0,1,0),  V(s, s,-s,0,1,0),  V(-s, s,-s,0,1,0),
        V(-s,-s,-s,0,-1,0),V(s,-s,-s,0,-1,0), V(s,-s, s,0,-1,0),
        V(s,-s, s,0,-1,0), V(-s,-s, s,0,-1,0),V(-s,-s,-s,0,-1,0)
    };
    upload(v, sizeof(v));
    bindAttribs(p);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    m_vao.release();
}

void GeometryEngine::drawCuboid(float w, float h, float d, QOpenGLShaderProgram* p, bool) {
    float x = w * 0.5f, y = h * 0.5f, z = d * 0.5f;
    VD v[] = {
        V(-x,-y, z,0,0,1), V(x,-y, z,0,0,1), V(x, y, z,0,0,1),
        V(x, y, z,0,0,1),  V(-x,y, z,0,0,1), V(-x,-y, z,0,0,1),
        V(-x,-y,-z,0,0,-1),V(-x, y,-z,0,0,-1),V(x, y,-z,0,0,-1),
        V(x, y,-z,0,0,-1), V(x,-y,-z,0,0,-1), V(-x,-y,-z,0,0,-1),
        V(-x,-y,-z,-1,0,0),V(-x,-y, z,-1,0,0),V(-x, y, z,-1,0,0),
        V(-x, y, z,-1,0,0),V(-x, y,-z,-1,0,0),V(-x,-y,-z,-1,0,0),
        V(x,-y,-z,1,0,0),  V(x, y,-z,1,0,0),  V(x, y, z,1,0,0),
        V(x, y, z,1,0,0),  V(x,-y, z,1,0,0),  V(x,-y,-z,1,0,0),
        V(-x, y,-z,0,1,0), V(-x, y, z,0,1,0), V(x, y, z,0,1,0),
        V(x, y, z,0,1,0),  V(x, y,-z,0,1,0),  V(-x, y,-z,0,1,0),
        V(-x,-y,-z,0,-1,0),V(x,-y,-z,0,-1,0), V(x,-y, z,0,-1,0),
        V(x,-y, z,0,-1,0), V(-x,-y, z,0,-1,0),V(-x,-y,-z,0,-1,0)
    };
    upload(v, sizeof(v));
    bindAttribs(p);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    m_vao.release();
}

void GeometryEngine::drawSphere(float r, QOpenGLShaderProgram* p, bool)
{
    const int sec = 36;
    const int stk = 18;

    std::vector<VD> verts;

    for (int i = 0; i < stk; ++i) {
        float phi1 = M_PI * (float(i) / stk - 0.5f);
        float phi2 = M_PI * (float(i + 1) / stk - 0.5f);

        float y1 = r * sinf(phi1);
        float y2 = r * sinf(phi2);

        float r1 = r * cosf(phi1);
        float r2 = r * cosf(phi2);

        for (int j = 0; j <= sec; ++j) {
            float theta = j * 2.0f * M_PI / sec;

            float x1 = r1 * cosf(theta);
            float z1 = r1 * sinf(theta);

            float x2 = r2 * cosf(theta);
            float z2 = r2 * sinf(theta);

            // triangle strip
            verts.push_back(V(x1, y1, z1, x1 / r, y1 / r, z1 / r));
            verts.push_back(V(x2, y2, z2, x2 / r, y2 / r, z2 / r));
        }
    }

    if (verts.empty()) return;

    m_vao.bind();

    m_vbo.bind();
    m_vbo.allocate(verts.data(), int(verts.size() * sizeof(VD)));

    p->enableAttributeArray(0);
    p->setAttributeBuffer(0, GL_FLOAT, 0, 3, sizeof(VD));

    p->enableAttributeArray(1);
    p->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 3, sizeof(VD));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(verts.size()));

    m_vbo.release();
    m_vao.release();
}