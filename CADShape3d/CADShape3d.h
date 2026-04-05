#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QKeyEvent>
#include <vector>
#include <memory>

#include "Shape.h"
#include "GeometryEngine.h"

enum AddShapeType { ADD_SQUARE, ADD_RECTANGLE, ADD_CIRCLE };
enum ViewMode { VIEW_2D, VIEW_3D };

class CADShape3d : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    CADShape3d(QWidget* parent = nullptr);
    ~CADShape3d();

    void setAddMode(AddShapeType t) { m_addMode = t; }
    void setViewMode(ViewMode mode) { m_viewMode = mode; update(); }
    void deleteSelectedPublic() { deleteSelected(); }
    void toggleRotation(bool active);

protected:
    void initializeGL()    override;
    void resizeGL(int, int) override;
    void paintGL()         override;

    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;

private:
    void      initShaders();
    void      deleteSelected();
    QVector2D pixelToWorld(const QPoint& p) const;
    float     pixelThreshold() const;

    GeometryEngine* m_geo = nullptr;
    QOpenGLShaderProgram m_prog;

    QMatrix4x4 m_proj2D;
    QMatrix4x4 m_proj3D;

    // Logical canvas size (set in resizeGL, used in pixelToWorld)
    // CRITICAL: these must be logical pixels, same space as e->pos()
    float m_viewW = 1.0f;
    float m_viewH = 1.0f;

    // World-space extents (match ortho projection exactly)
    float m_halfW = 1.0f;
    float m_halfH = 1.0f;

    std::vector<std::unique_ptr<Shape>> m_shapes;
    int m_selectedIdx = -1;

    DragContext m_drag;
    bool        m_dragging = false;

    AddShapeType m_addMode = ADD_SQUARE;
    ViewMode     m_viewMode = VIEW_2D;

    float  m_orbitX = 25.0f;
    float  m_orbitY = 30.0f;
    QPoint m_lastRMB;
    bool   m_rmbDragging = false;

};