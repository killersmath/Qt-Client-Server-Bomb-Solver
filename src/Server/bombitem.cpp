#include "bombitem.h"

#include <QCursor>
#include <QPainter>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QRandomGenerator>

////////////////////////
////  Class Bomb Wire
////////////////////////

BombWire::BombWire(const QColor &color, int id, QGraphicsItem *parent) :
    QGraphicsObject(parent),
    _color(color),
    _id(id),
    _pressed(false)
{
    setFlag(ItemClipsToShape, true);

    setCursor(QCursor(Qt::PointingHandCursor));
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

//////////////////
///   Class: Bomb Body
/////////////////

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

BombTimer::BombTimer(QGraphicsItem *parent) : QGraphicsObject(parent) , _remaindSeconds(0), _updateTimerID(-1)
{
}

QRectF BombTimer::boundingRect() const
{
    qreal adjust = 0.5;
    return QRectF(-150 - adjust, -60 - adjust,
                  300 + adjust, 120 + adjust);
}

void BombTimer::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // Timer Body
    painter->save();
    painter->setBrush(QBrush(QColor(230, 230, 230)));
    painter->setPen(QPen(Qt::black, 1));
    painter->drawRoundedRect(-130, -40, 260, 45, 3, 3);
    painter->restore();

    // clock number

    const int minutes = _remaindSeconds / 60;
    const int seconds = _remaindSeconds % 60;

    painter->save();
    QFont font = painter->font();
    font.setPixelSize(40);
    painter->setFont(font);
    painter->drawText(QRectF(-130, -40, 260, 45), Qt::AlignCenter, QString("00:%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')));
    painter->restore();
}

void BombTimer::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)

    if(_remaindSeconds > 1){
        _remaindSeconds--;
        update();
    }
    else{
        if(_updateTimerID != -1){
            killTimer(_updateTimerID);
            _updateTimerID = -1;
        }

        emit timeout();
    }
}

bool BombTimer::isRunning()
{
    return _updateTimerID != -1;
}

void BombTimer::setupTimer(const int secs)
{
    if(_updateTimerID != -1)
        killTimer(_updateTimerID);

    _remaindSeconds = secs;
    _updateTimerID = startTimer(1000);
}

void BombTimer::stopTimer()
{
    if(_updateTimerID != -1){
        killTimer(_updateTimerID);
        _updateTimerID = -1;
    }

    _remaindSeconds = 0;
}

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

    connect(rWire, &BombWire::clicked, this, &Bomb::wireCutted);
    connect(gWire, &BombWire::clicked, this, &Bomb::wireCutted);
    connect(bWire, &BombWire::clicked, this, &Bomb::wireCutted);

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
    if(bTimer->isRunning())
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
