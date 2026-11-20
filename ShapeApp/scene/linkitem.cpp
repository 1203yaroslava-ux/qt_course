#include "linkitem.h"
#include <QPen>

LinkItem::LinkItem(ShapeItem *a, ShapeItem *b)
    : m_a(a), m_b(b)
{
    setPen(QPen(Qt::black, 2));
    setZValue(0);
    m_a->addLink(this);
    m_b->addLink(this);
    updatePosition();
}

void LinkItem::updatePosition()
{
    setLine(QLineF(m_a->sceneBoundingRect().center(),
                   m_b->sceneBoundingRect().center()));
}
