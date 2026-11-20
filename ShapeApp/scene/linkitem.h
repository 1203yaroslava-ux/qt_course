#pragma once
#include <QGraphicsLineItem>
#include "shapeitem.h"

class LinkItem : public QGraphicsLineItem
{
public:
    LinkItem(ShapeItem *a, ShapeItem *b);
    ShapeItem *first() const  { return m_a; }
    ShapeItem *second() const { return m_b; }
    void updatePosition();

private:
    ShapeItem *m_a;
    ShapeItem *m_b;
};
