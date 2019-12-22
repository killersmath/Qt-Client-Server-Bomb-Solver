#ifndef BOMB_H
#define BOMB_H

#include <QGraphicsObject>
#include <QRectF>

#include "bombwire.h"
#include "bombtimer.h"
#include "bombbody.h"

class Bomb : public QGraphicsObject
{
    Q_OBJECT

public:
    Bomb(QGraphicsItem *parent = 0);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

    void setupBomb(const int secs);
    void stopBomb();

private slots:
    void wireCutted(int id);

signals:
    void defused();
    void explosed();

private:
    BombBody *bBody;
    BombTimer *bTimer;
    BombWire *rWire, *gWire, *bWire;
    int _defuseWireID;
};


#endif // BOMB_H
