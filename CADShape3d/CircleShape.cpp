#include "CircleShape.h"

CircleShape::CircleShape(QVector2D pos, float r) : radius(r) {
    position = pos;
}

void CircleShape::draw2D(GeometryEngine* geo, QOpenGLShaderProgram* p) {
    geo->drawCircle(radius, p, isSelected);
}

void CircleShape::draw3D(GeometryEngine* geo, QOpenGLShaderProgram* p) {
    geo->drawSphere(radius, p, isSelected);
}

HandleType CircleShape::hitTest(QVector2D mouse, float thr) const {
    QVector2D lc = mouse - position;
    float dist = lc.length();

    // Perimeter band: detects if the mouse is near the edge of the circle
    if (qAbs(dist - radius) < thr)
        return HANDLE_PERIMETER;

    // Body: detects if the mouse is inside the circle
    if (dist < radius - thr)
        return HANDLE_BODY;

    return HANDLE_NONE;
}

DragContext CircleShape::buildDragContext(HandleType h, QVector2D pw) const {
    DragContext ctx;
    ctx.handle = h;
    ctx.pressWorld = pw;
    ctx.pressCenter = position;
    ctx.origW = radius;
    return ctx;
}

void CircleShape::applyDrag(const DragContext& ctx, QVector2D cur) {
    if (ctx.handle == HANDLE_BODY) {
        // Moves the circle based on mouse displacement
        position = cur - (ctx.pressWorld - ctx.pressCenter);
        return;
    }
    // Radius = distance from center to current mouse position
    radius = qMax(0.04f, (cur - position).length());
}

void CircleShape::clampToBounds(float halfW, float halfH) {
    float maxR = qMin(halfW, halfH);
    radius = qMin(radius, maxR);
    position.setX(qBound(-halfW + radius, position.x(), halfW - radius));
    position.setY(qBound(-halfH + radius, position.y(), halfH - radius));
}

QString CircleShape::typeName() const {
    return "Circle";
}