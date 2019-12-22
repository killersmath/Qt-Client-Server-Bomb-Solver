#include "bombbody.h"

#include <QRectF>
#include <QPainter>

BombBody::BombBody(QGraphicsItem *parent) : QGraphicsObject(parent)
{
}

QRectF BombBody::boundingRect() const
{
    qreal adjust = 0.5;
    return QRectF(-150 - adjust, -60 - adjust,
                  300 + adjust, 120 + adjust);
}

void BombBody::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // full body
    painter->save();
    painter->setBrush(Qt::white);
    painter->setPen(QPen(Qt::black, 1));
    painter->drawRect(-150,-60, 300, 120);
    painter->restore();
}
