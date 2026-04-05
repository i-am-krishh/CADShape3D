#pragma once
#include <QVector2D>
#include <QOpenGLShaderProgram>
#include <QString>
#include <QtMath>

class GeometryEngine;

enum HandleType {
    HANDLE_NONE = -1,
    HANDLE_CORNER_TL = 0,
    HANDLE_CORNER_TR = 1,
    HANDLE_CORNER_BR = 2,
    HANDLE_CORNER_BL = 3,
    HANDLE_EDGE_TOP = 4,
    HANDLE_EDGE_RIGHT = 5,
    HANDLE_EDGE_BOTTOM = 6,
    HANDLE_EDGE_LEFT = 7,
    HANDLE_PERIMETER = 8,
    HANDLE_BODY = 9
};

// Everything needed to describe a drag, captured once at mouse-press.
struct DragContext {
    HandleType handle = HANDLE_NONE;
    QVector2D  pressWorld;       // world pos at press
    QVector2D  pressCenter;      // shape center at press
    float      anchorX = 0;   // fixed opposite corner/edge (world)
    float      anchorY = 0;
    float      origW = 0;   // original width / side / radius
    float      origH = 0;   // original height (rect only)
};

class Shape {
public:
    QVector2D position;
    bool isSelected = false;

    virtual void draw2D(GeometryEngine*, QOpenGLShaderProgram*) = 0;
    virtual void draw3D(GeometryEngine*, QOpenGLShaderProgram*) = 0;

    virtual HandleType hitTest(QVector2D mouse, float threshold) const = 0;
    virtual DragContext buildDragContext(HandleType h, QVector2D pressWorld) const = 0;
    virtual void applyDrag(const DragContext& ctx, QVector2D cur) = 0;
    virtual void clampToBounds(float halfW, float halfH) = 0;

    virtual QString typeName() const = 0;
    virtual ~Shape() {}
};