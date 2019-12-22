#ifndef BOMBBODY_H
#define BOMBBODY_H

#include <QGraphicsObject>
#include <QRectF>

class BombBody : public QGraphicsObject
{
    Q_OBJECT
public:
    BombBody(QGraphicsItem *parent = 0);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
};

#endif // BOMBBODY_H
