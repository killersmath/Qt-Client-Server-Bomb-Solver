#ifndef BOMBITEM_H
#define BOMBITEM_H

#include <QColor>
#include <QGraphicsItem>
#include <QGraphicsObject>

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

class BombBody : public QGraphicsObject
{
    Q_OBJECT
public:
    BombBody(QGraphicsItem *parent = 0);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
};

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
    int _remaindSeconds;
    int _updateTimerID;
};

class Bomb : public QGraphicsObject
{
    Q_OBJECT

public:
    Bomb(QGraphicsItem *parent = 0);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

    void setupBomb(const int secs);

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

#endif // BOMBITEM_H
