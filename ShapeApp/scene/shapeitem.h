#pragma once
#include <QGraphicsPathItem>
#include <QList>

class LinkItem;

class ShapeItem : public QGraphicsPathItem
{
public:
    ShapeItem(int id, const QString &type, int w, int h);

    int id() const { return m_id; }
    QString shapeType() const { return m_type; }
    QList<LinkItem *> links() const { return m_links; }
    void addLink(LinkItem *l)    { m_links.append(l); }
    void removeLink(LinkItem *l) { m_links.removeAll(l); }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    int m_id;
    QString m_type;
    QList<LinkItem *> m_links;
};
