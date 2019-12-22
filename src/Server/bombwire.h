#ifndef BOMBWIRE_H
#define BOMBWIRE_H

#include <QGraphicsObject>
#include <QColor>
#include <QRectF>
#include <QPainterPath>

class BombWire : public QGraphicsObject
{
    Q_OBJECT
public:
    BombWire(const QColor &color, int id = -1, QGraphicsItem *parent = 0);

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    int id() const {return _id;}

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

signals:
    void clicked(const int id);

private:
    QColor _color;
    int _id;
    bool _pressed;
};
#endif // BOMBWIRE_H
