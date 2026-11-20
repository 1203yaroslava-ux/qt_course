#include "shapeitem.h"
#include "linkitem.h"
#include <QtWidgets>

ShapeItem::ShapeItem(int id, const QString &type, int w, int h)
    : m_id(id), m_type(type)
{
    QPainterPath path;
    QColor color;
    if (type == "ellipse") {
        path.addEllipse(0, 0, w, h);
        color = QColor(150, 200, 255);
    } else if (type == "rectangle") {
        path.addRect(0, 0, w, h);
        color = QColor(160, 230, 160);
    } else {                                  // "polygon": шестиугольник
        QPolygonF poly;
        poly << QPointF(w / 4.0, 0)     << QPointF(w * 3 / 4.0, 0)
             << QPointF(w, h / 2.0)     << QPointF(w * 3 / 4.0, h)
             << QPointF(w / 4.0, h)     << QPointF(0, h / 2.0);
        path.addPolygon(poly);
        path.closeSubpath();
        color = QColor(255, 200, 120);
    }
    setPath(path);
    setBrush(QBrush(color));
    setPen(QPen(Qt::black, 2));
    setZValue(1);                             // фигуры рисуются поверх линий
    setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
}

QVariant ShapeItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionHasChanged) {   // фигуру сдвинули
        for (LinkItem *l : m_links)
            l->updatePosition();              // линии тянутся за ней
    }
    return QGraphicsPathItem::itemChange(change, value);
}
