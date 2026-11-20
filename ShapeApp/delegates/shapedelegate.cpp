#include "shapedelegate.h"
#include <QtWidgets>

static const int ICON = 22;   // размер одной иконки
static const int GAP  = 6;    // отступ

void ShapeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, option.palette.highlight());

    int count = index.data().toInt();
    QString type = index.sibling(index.row(), 1).data().toString();

    int n = 1;                       // 1..3 фигуры  1 иконка
    if (count > 10)     n = 3;       // больше 10    3 иконки
    else if (count > 3) n = 2;       // 4..10        2 иконки

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(Qt::black, 1.5));

    for (int i = 0; i < n; ++i) {
        QRect r(option.rect.left() + GAP + i * (ICON + GAP),
                option.rect.top() + (option.rect.height() - ICON) / 2,
                ICON, ICON);
        if (type == "ellipse") {
            painter->setBrush(QColor(150, 200, 255));
            painter->drawEllipse(r);
        } else if (type == "rectangle") {
            painter->setBrush(QColor(160, 230, 160));
            painter->drawRect(r);
        } else {
            painter->setBrush(QColor(255, 200, 120));
            QPolygonF poly;
            poly << QPointF(r.left() + r.width() / 4.0, r.top())
                 << QPointF(r.left() + r.width() * 3 / 4.0, r.top())
                 << QPointF(r.right(), r.top() + r.height() / 2.0)
                 << QPointF(r.left() + r.width() * 3 / 4.0, r.bottom())
                 << QPointF(r.left() + r.width() / 4.0, r.bottom())
                 << QPointF(r.left(), r.top() + r.height() / 2.0);
            painter->drawPolygon(poly);
        }
    }
    painter->restore();
}

QSize ShapeDelegate::sizeHint(const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    return QSize(3 * (ICON + GAP) + GAP, ICON + 8);
}
