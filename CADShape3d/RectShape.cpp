#include "RectShape.h"

RectShape::RectShape(QVector2D pos, float w, float h)
    : width(w), height(h) {
    position = pos;
}

void RectShape::draw2D(GeometryEngine* geo, QOpenGLShaderProgram* p) {
    geo->drawRect(width, height, p, isSelected);
}

void RectShape::draw3D(GeometryEngine* geo, QOpenGLShaderProgram* p) {
    float depth = qMin(width, height) * 0.5f;
    geo->drawCuboid(width, height, depth, p, isSelected);
}

HandleType RectShape::hitTest(QVector2D mouse, float thr) const {
    float hx = width * 0.5f, hy = height * 0.5f;
    QVector2D lc = mouse - position;

    QVector2D corners[4] = { {-hx,hy}, {hx,hy}, {hx,-hy}, {-hx,-hy} };
    for (int i = 0; i < 4; ++i) {
        if ((lc - corners[i]).length() < thr)
            return (HandleType)i;
    }

    QVector2D edges[4] = { {0,hy}, {hx,0}, {0,-hy}, {-hx,0} };
    for (int i = 0; i < 4; ++i) {
        if ((lc - edges[i]).length() < thr)
            return (HandleType)(HANDLE_EDGE_TOP + i);
    }

    if (qAbs(lc.x()) <= hx && qAbs(lc.y()) <= hy)
        return HANDLE_BODY;

    return HANDLE_NONE;
}

DragContext RectShape::buildDragContext(HandleType h, QVector2D pw) const {
    DragContext ctx;
    ctx.handle = h;
    ctx.pressWorld = pw;
    ctx.pressCenter = position;
    ctx.origW = width;
    ctx.origH = height;

    float hx = width * 0.5f, hy = height * 0.5f;
    switch (h) {
    case HANDLE_CORNER_TL: ctx.anchorX = position.x() + hx; ctx.anchorY = position.y() - hy; break;
    case HANDLE_CORNER_TR: ctx.anchorX = position.x() - hx; ctx.anchorY = position.y() - hy; break;
    case HANDLE_CORNER_BR: ctx.anchorX = position.x() - hx; ctx.anchorY = position.y() + hy; break;
    case HANDLE_CORNER_BL: ctx.anchorX = position.x() + hx; ctx.anchorY = position.y() + hy; break;
    case HANDLE_EDGE_TOP:    ctx.anchorX = position.x();    ctx.anchorY = position.y() - hy; break;
    case HANDLE_EDGE_BOTTOM: ctx.anchorX = position.x();    ctx.anchorY = position.y() + hy; break;
    case HANDLE_EDGE_LEFT:   ctx.anchorX = position.x() + hx; ctx.anchorY = position.y();   break;
    case HANDLE_EDGE_RIGHT:  ctx.anchorX = position.x() - hx; ctx.anchorY = position.y();   break;
    default: break;
    }
    return ctx;
}

void RectShape::applyDrag(const DragContext& ctx, QVector2D cur) {
    if (ctx.handle == HANDLE_BODY) {
        position = cur - (ctx.pressWorld - ctx.pressCenter);
        return;
    }

    float dx = qAbs(cur.x() - ctx.anchorX);
    float dy = qAbs(cur.y() - ctx.anchorY);

    switch (ctx.handle) {
    case HANDLE_CORNER_TL: case HANDLE_CORNER_TR:
    case HANDLE_CORNER_BR: case HANDLE_CORNER_BL:
        width = qMax(0.04f, dx);
        height = qMax(0.04f, dy);
        position = QVector2D((ctx.anchorX + cur.x()) * 0.5f, (ctx.anchorY + cur.y()) * 0.5f);
        break;
    case HANDLE_EDGE_TOP: case HANDLE_EDGE_BOTTOM:
        height = qMax(0.04f, dy);
        position = QVector2D(ctx.pressCenter.x(), (ctx.anchorY + cur.y()) * 0.5f);
        break;
    case HANDLE_EDGE_LEFT: case HANDLE_EDGE_RIGHT:
        width = qMax(0.04f, dx);
        position = QVector2D((ctx.anchorX + cur.x()) * 0.5f, ctx.pressCenter.y());
        break;
    default: break;
    }
}

void RectShape::clampToBounds(float halfW, float halfH) {
    width = qMin(width, halfW * 2.0f);
    height = qMin(height, halfH * 2.0f);
    float hx = width * 0.5f, hy = height * 0.5f;
    position.setX(qBound(-halfW + hx, position.x(), halfW - hx));
    position.setY(qBound(-halfH + hy, position.y(), halfH - hy));
}

QString RectShape::typeName() const {
    return "Rectangle";
}