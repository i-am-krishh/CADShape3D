#pragma once

#include "Shape.h"
#include "GeometryEngine.h"

class CircleShape : public Shape {
public:
    float radius = 0.25f;

    CircleShape(QVector2D pos = { 0,0 }, float r = 0.25f);

    void draw2D(GeometryEngine* geo, QOpenGLShaderProgram* p) override;
    void draw3D(GeometryEngine* geo, QOpenGLShaderProgram* p) override;

    HandleType hitTest(QVector2D mouse, float thr) const override;
    DragContext buildDragContext(HandleType h, QVector2D pw) const override;
    void applyDrag(const DragContext& ctx, QVector2D cur) override;
    void clampToBounds(float halfW, float halfH) override;

    QString typeName() const override;
};