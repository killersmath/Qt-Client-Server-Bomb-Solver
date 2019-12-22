#ifndef BOMBTIMER_H
#define BOMBTIMER_H

#include <QGraphicsObject>
#include <QRectF>

class BombTimer : public QGraphicsObject
{
    Q_OBJECT
public:
    BombTimer(QGraphicsItem *parent = 0);
    void setupTimer(const int secs);

    bool isRunning();

protected:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
    void timerEvent(QTimerEvent *event);

signals:
    void timeout();

public slots:
    void stopTimer();

private:
    int _remainingTime;
    int _updateTimerID;
};
#endif // BOMBTIMER_H
