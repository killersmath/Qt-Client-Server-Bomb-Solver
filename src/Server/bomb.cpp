#include "bomb.h"

#include <QRandomGenerator>
#include <QPainter>

Bomb::Bomb(QGraphicsItem *parent) : QGraphicsObject(parent)
{
    setFlag(ItemHasNoContents);

    bBody = new BombBody(this);
    bTimer = new BombTimer(bBody);
    rWire = new BombWire(QColor(255, 0, 0), 0, this);
    gWire = new BombWire(QColor(0, 255, 0), 1, this);
    bWire = new BombWire(QColor(0, 0, 255), 2, this);

    bBody->setZValue(zValue()+1);

    rWire->setPos(-80, 55);
    rWire->setZValue(zValue());
    gWire->setPos(0, 55);
    gWire->setZValue(zValue());
    bWire->setPos(80, 55);
    bWire->setZValue(zValue());

    //connect(rWire, &BombWire::clicked, this, &Bomb::wireCutted);
    //connect(gWire, &BombWire::clicked, this, &Bomb::wireCutted);
    //connect(bWire, &BombWire::clicked, this, &Bomb::wireCutted);

    connect(bTimer, &BombTimer::timeout, this, &Bomb::explosed);

    connect(this, &Bomb::defused, bTimer, &BombTimer::stopTimer);
    connect(this, &Bomb::explosed, bTimer, &BombTimer::stopTimer);
}

QRectF Bomb::boundingRect() const
{
    return QRectF();
}

void Bomb::paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}

void Bomb::setupBomb(const int secs)
{
    _defuseWireID = QRandomGenerator::global()->bounded(3);
    bTimer->setupTimer(secs);
}

void Bomb::stopBomb()
{
    bTimer->stopTimer();
}

void Bomb::wireCutted(int id)
{
    if(bTimer->isRunning()){
        if(_defuseWireID == id)
            emit defused();
        else
            emit explosed();
    }
}

