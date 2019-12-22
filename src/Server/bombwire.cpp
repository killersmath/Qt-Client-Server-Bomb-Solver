#include "bombwire.h"

//#include <QCursor>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

BombWire::BombWire(const QColor &color, int id, QGraphicsItem *parent) :
    QGraphicsObject(parent),
    _color(color),
    _id(id),
    _pressed(false)
{
    setFlag(ItemClipsToShape, true);

    //setCursor(QCursor(Qt::PointingHandCursor));
}

QRectF BombWire::boundingRect() const
{
    qreal adjust = 0.5;
    return QRectF(-28 - adjust, 6 - adjust,
                  54 + adjust, 20 + adjust);
}

QPainterPath BombWire::shape() const
{
    QPainterPath path(QPointF(-32, -7));

    path.cubicTo(-32, -7 ,0, 65, 32, -7);
    path.moveTo(17, -8);
    path.cubicTo(17, -8, 0, 35, -17, -8);
    path.moveTo(-30, -7);

    return path;
}

void BombWire::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    qreal wireWidth = 10;
    qreal wireBorderWidth = 2;

    // Draw Border
    painter->save();
    QPainterPath borderPath(QPointF(-20,0));
    borderPath.cubicTo(-20,0,0, 40,20,0);
    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen(QBrush(Qt::black), wireWidth + wireBorderWidth));
    painter->drawPath(borderPath);
    painter->restore();

    // Draw Wire
    QPainterPath wirePath(QPointF(-20,0));
    wirePath.cubicTo(-20,0,0,40,20,0);
    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen(QBrush(_color), wireWidth));
    painter->drawPath(wirePath);
}

void BombWire::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        _pressed = true;
}

void BombWire::mouseReleaseEvent(QGraphicsSceneMouseEvent */*event*/)
{
    if(_pressed)
    {
        _pressed = false;
        emit clicked(_id);
    }
}
