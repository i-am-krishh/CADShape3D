#include "SquareShape.h"

SquareShape::SquareShape(QVector2D pos, float s) : side(s) {
    position = pos;
}

void SquareShape::draw2D(GeometryEngine* geo, QOpenGLShaderProgram* p) {
    geo->drawSquare(side, p, isSelected);
}

void SquareShape::draw3D(GeometryEngine* geo, QOpenGLShaderProgram* p) {
    geo->drawCube(side, p, isSelected);
}

HandleType SquareShape::hitTest(QVector2D mouse, float thr) const {
    float h = side * 0.5f;
    QVector2D lc = mouse - position;

    // Corners (test first - higher priority)
    QVector2D corners[4] = { {-h,h},{h,h},{h,-h},{-h,-h} };
    for (int i = 0; i < 4; ++i)
        if ((lc - corners[i]).length() < thr)
            return (HandleType)i;

    // Edge midpoints
    QVector2D edges[4] = { {0,h},{h,0},{0,-h},{-h,0} };
    for (int i = 0; i < 4; ++i)
        if ((lc - edges[i]).length() < thr)
            return (HandleType)(HANDLE_EDGE_TOP + i);

    // Body
    if (qAbs(lc.x()) <= h && qAbs(lc.y()) <= h)
        return HANDLE_BODY;

    return HANDLE_NONE;
}

DragContext SquareShape::buildDragContext(HandleType h, QVector2D pressWorld) const {
    DragContext ctx;
    ctx.handle = h;
    ctx.pressWorld = pressWorld;
    ctx.pressCenter = position;
    ctx.origW = side;

    float hs = side * 0.5f;
    switch (h) {
    case HANDLE_CORNER_TL: ctx.anchorX = position.x() + hs; ctx.anchorY = position.y() - hs; break;
    case HANDLE_CORNER_TR: ctx.anchorX = position.x() - hs; ctx.anchorY = position.y() - hs; break;
    case HANDLE_CORNER_BR: ctx.anchorX = position.x() - hs; ctx.anchorY = position.y() + hs; break;
    case HANDLE_CORNER_BL: ctx.anchorX = position.x() + hs; ctx.anchorY = position.y() + hs; break;
    case HANDLE_EDGE_TOP:    ctx.anchorX = position.x();     ctx.anchorY = position.y() - hs; break;
    case HANDLE_EDGE_BOTTOM: ctx.anchorX = position.x();     ctx.anchorY = position.y() + hs; break;
    case HANDLE_EDGE_LEFT:   ctx.anchorX = position.x() + hs;  ctx.anchorY = position.y();    break;
    case HANDLE_EDGE_RIGHT:  ctx.anchorX = position.x() - hs;  ctx.anchorY = position.y();    break;
    default: break;
    }
    return ctx;
}

void SquareShape::applyDrag(const DragContext& ctx, QVector2D cur) {
    if (ctx.handle == HANDLE_BODY) {
        QVector2D offset = ctx.pressWorld - ctx.pressCenter;
        position = cur - offset;
        return;
    }

    float dx = qAbs(cur.x() - ctx.anchorX);
    float dy = qAbs(cur.y() - ctx.anchorY);
    float newSide = qMax(0.05f, qMax(dx, dy));
    side = newSide;

    float midX = (ctx.anchorX + cur.x()) * 0.5f;
    float midY = (ctx.anchorY + cur.y()) * 0.5f;

    switch (ctx.handle) {
    case HANDLE_EDGE_TOP:
    case HANDLE_EDGE_BOTTOM:
        side = qMax(0.05f, dy);
        midX = ctx.pressCenter.x();
        break;
    case HANDLE_EDGE_LEFT:
    case HANDLE_EDGE_RIGHT:
        side = qMax(0.05f, dx);
        midY = ctx.pressCenter.y();
        break;
    default: break;
    }

    position = QVector2D(midX, midY);
}

void SquareShape::clampToBounds(float halfW, float halfH) {
    float h = side * 0.5f;
    side = qMin(side, qMin(halfW, halfH) * 2.0f);
    h = side * 0.5f;
    position.setX(qBound(-halfW + h, position.x(), halfW - h));
    position.setY(qBound(-halfH + h, position.y(), halfH - h));
}

QString SquareShape::typeName() const {
    return "Square";
}