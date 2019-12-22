#include "bombtimer.h"

#include <QRectF>
#include <QPainter>

BombTimer::BombTimer(QGraphicsItem *parent) :
    QGraphicsObject(parent) ,
    _remainingTime(0),
    _updateTimerID(-1)
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
    painter->save();
    QFont font = painter->font();
    font.setPixelSize(40);
    painter->setFont(font);

    const int hours = floor(_remainingTime / 3600);
    const int minutes = floor((_remainingTime / 60) % 60);
    const int seconds = _remainingTime % 60;

    // print hrs, mins, secs
    painter->drawText(QRectF(-130, -40, 260, 45),
                      Qt::AlignCenter,
                      QString("%1:%2:%3").arg(hours, 2, 10, QChar('0')).arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')));

    painter->restore();
}

void BombTimer::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)

    if(_remainingTime > 1){
        _remainingTime--;
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

    _remainingTime = secs;
    _updateTimerID = startTimer(1000);
}

void BombTimer::stopTimer()
{
    if(_updateTimerID != -1){
        killTimer(_updateTimerID);
        _updateTimerID = -1;
    }

    _remainingTime = 0;
}
