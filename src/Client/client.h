#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QHostAddress>
#include <QTcpSocket>

#include "socketthread.h"

class Client : public QObject
{
    Q_OBJECT

public:
    enum ConnectState {Disconnected, Connecting, Connected};

    explicit Client(QObject *parent = 0);

    void connect(const QHostAddress &host, quint16 port);

    bool isConnected();

signals:
    void disconnect();

    void connected();
    void disconnected();

    void sendData(const QByteArray &data);
    void receiveData(const QByteArray &data);
    void socketError(QTcpSocket::SocketError error);

private slots:
    void onClientConnected();
    void onClientDisconnected();

private:
    SocketThread *_socketThread;
    bool _connected;
};

#endif // CLIENT_H
