#include "CADShape3d.h"
#include "SquareShape.h"
#include "RectShape.h"
#include "CircleShape.h"

// INIT 

CADShape3d::CADShape3d(QWidget* parent) : QOpenGLWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
}

CADShape3d::~CADShape3d() {
    makeCurrent();
    delete m_geo;
    doneCurrent();
}

void CADShape3d::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.07f, 0.07f, 0.11f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.6f);
    initShaders();
    m_geo = new GeometryEngine;
}

// SHADERS 

void CADShape3d::initShaders() {
    const char* vs =
        "#version 330 core\n"
        "layout(location=0) in vec3 aPos;\n"
        "layout(location=1) in vec3 aNorm;\n"
        "uniform mat4 uMVP;\n"
        "out vec3 vNorm;\n"
        "void main(){ gl_Position=uMVP*vec4(aPos,1.0); vNorm=aNorm; }\n";

    const char* fs =
        "#version 330 core\n"
        "in vec3 vNorm;\n"
        "out vec4 fragColor;\n"
        "uniform vec3 uColor;\n"
        "uniform float uAmbient;\n"
        "void main(){\n"
        "  vec3 L=normalize(vec3(0.6,0.9,1.0));\n"
        "  float d=max(dot(normalize(vNorm),L),0.0);\n"
        "  fragColor=vec4(uColor*(uAmbient+(1.0-uAmbient)*d),1.0);\n"
        "}\n";

    m_prog.addShaderFromSourceCode(QOpenGLShader::Vertex, vs);
    m_prog.addShaderFromSourceCode(QOpenGLShader::Fragment, fs);
    m_prog.link();
}

void CADShape3d::resizeGL(int w, int h) {
    float dpr = static_cast<float>(devicePixelRatio());

    m_viewW = static_cast<float>(w) / dpr;
    m_viewH = static_cast<float>(h) / dpr;

    float aspect = m_viewW / m_viewH;
    m_halfW = aspect;
    m_halfH = 1.0f;

    m_proj2D.setToIdentity();
    m_proj2D.ortho(-m_halfW, m_halfW, -m_halfH, m_halfH, -10.f, 10.f);

    m_proj3D.setToIdentity();
    m_proj3D.perspective(45.f, aspect, 0.1f, 100.f);

    glViewport(0, 0, w, h);
}

// DRAW 

void CADShape3d::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_prog.bind();

    if (m_viewMode == VIEW_2D) {
        glDisable(GL_DEPTH_TEST);

        for (int i = 0; i < (int)m_shapes.size(); i++) {
            auto& s = m_shapes[i];
            bool sel = (i == m_selectedIdx);

            QMatrix4x4 model;
            model.translate(s->position.x(), s->position.y(), 0);
            m_prog.setUniformValue("uMVP", m_proj2D * model);
            m_prog.setUniformValue("uAmbient", 1.0f);

            s->isSelected = false;
            m_prog.setUniformValue("uColor", sel ? 1.0f : 0.3f,
                sel ? 0.8f : 0.7f,
                sel ? 0.2f : 1.0f);
            s->draw2D(m_geo, &m_prog);

if (sel) {
    s->isSelected = false; 
    m_prog.setUniformValue("uColor", 1.0f, 1.0f, 1.0f);
    s->draw2D(m_geo, &m_prog);
}
        }
        for (int i = 0; i < (int)m_shapes.size(); i++)
            m_shapes[i]->isSelected = (i == m_selectedIdx);
    }
    else {
        glEnable(GL_DEPTH_TEST);

        QMatrix4x4 view;
        view.translate(0, 0, -4);
        view.rotate(m_orbitX, 1, 0, 0);
        view.rotate(m_orbitY, 0, 1, 0);

        for (int i = 0; i < (int)m_shapes.size(); i++) {
            auto& s = m_shapes[i];
            bool sel = (i == m_selectedIdx);

            QMatrix4x4 model;
            model.translate(s->position.x(), s->position.y(), 0);
            m_prog.setUniformValue("uMVP", m_proj3D * view * model);
            m_prog.setUniformValue("uAmbient", 0.22f);
            m_prog.setUniformValue("uColor", sel ? 1.0f : 0.2f,
                sel ? 0.8f : 0.6f,
                sel ? 0.2f : 0.9f);
            s->draw3D(m_geo, &m_prog);
        }
    }
}

// COORDINATE CONVERSION 

QVector2D CADShape3d::pixelToWorld(const QPoint& p) const {
    float w = static_cast<float>(width());
    float h = static_cast<float>(height());

    float centerX = w / 2.0f;
    float centerY = h / 2.0f;

    float dx = static_cast<float>(p.x()) - centerX;
    float dy = centerY - static_cast<float>(p.y());

    float worldX = (dx / w) * (m_halfW * 2.0f);
    float worldY = (dy / h) * (m_halfH * 2.0f);

    return QVector2D(worldX, worldY);
}

float CADShape3d::pixelThreshold() const {
    return (12.0f / m_viewW) * (m_halfW * 2.0f);
}

// Event handling

void CADShape3d::mousePressEvent(QMouseEvent* e) {
    setFocus();

    if (m_viewMode == VIEW_3D) {
        if (e->button() == Qt::RightButton) {
            m_rmbDragging = true;
            m_lastRMB = e->pos();
        }
        return;
    }

    if (e->button() != Qt::LeftButton) return;

    QVector2D mouse = pixelToWorld(e->pos());
    float thr = pixelThreshold();

    if (e->type() == QEvent::MouseButtonDblClick) {
        if (m_addMode == ADD_SQUARE)
            m_shapes.push_back(std::make_unique<SquareShape>(mouse, 0.28f));
        else if (m_addMode == ADD_RECTANGLE)
            m_shapes.push_back(std::make_unique<RectShape>(mouse, 0.4f, 0.25f));
        else
            m_shapes.push_back(std::make_unique<CircleShape>(mouse, 0.18f));

        m_selectedIdx = (int)m_shapes.size() - 1;
        m_shapes[m_selectedIdx]->isSelected = true;
        m_dragging = false;
        update();
        return;
    }

    for (auto& s : m_shapes) s->isSelected = false;
    m_selectedIdx = -1;
    m_dragging = false;

    for (int i = (int)m_shapes.size() - 1; i >= 0; i--) {
        HandleType h = m_shapes[i]->hitTest(mouse, thr);
        if (h != HANDLE_NONE) {
            m_selectedIdx = i;
            m_shapes[i]->isSelected = true;
            m_drag = m_shapes[i]->buildDragContext(h, mouse);
            m_dragging = true;
            break;
        }
    }
    update();
}

void CADShape3d::mouseMoveEvent(QMouseEvent* e) {
    if (m_viewMode == VIEW_3D) {
        if (m_rmbDragging) {
            QPoint d = e->pos() - m_lastRMB;
            m_orbitX = qBound(-89.f, m_orbitX + d.y() * 0.4f, 89.f);
            m_orbitY += d.x() * 0.4f;
            m_lastRMB = e->pos();
            update();
        }
        return;
    }

    if (!m_dragging || !(e->buttons() & Qt::LeftButton)) return;
    if (m_selectedIdx < 0) return;

    QVector2D world = pixelToWorld(e->pos());
    m_shapes[m_selectedIdx]->applyDrag(m_drag, world);
    m_shapes[m_selectedIdx]->clampToBounds(m_halfW, m_halfH);
    update();
}

void CADShape3d::mouseReleaseEvent(QMouseEvent* e) {
    if (e->button() == Qt::RightButton) m_rmbDragging = false;
    if (e->button() == Qt::LeftButton)  m_dragging = false;
    update();
}

void CADShape3d::keyPressEvent(QKeyEvent* e) {
    switch (e->key()) {
    case Qt::Key_1: m_addMode = ADD_SQUARE;    break;
    case Qt::Key_2: m_addMode = ADD_RECTANGLE; break;
    case Qt::Key_3: m_addMode = ADD_CIRCLE;    break;
    case Qt::Key_Delete:
    case Qt::Key_Backspace: deleteSelected(); return;
    case Qt::Key_Escape:
        for (auto& s : m_shapes) s->isSelected = false;
        m_selectedIdx = -1;
        m_dragging = false;
        break;
    }
    update();
}

void CADShape3d::deleteSelected() {
    if (m_selectedIdx >= 0 && m_selectedIdx < (int)m_shapes.size()) {
        m_shapes.erase(m_shapes.begin() + m_selectedIdx);
        m_selectedIdx = -1;
        m_dragging = false;
    }
    update();
}