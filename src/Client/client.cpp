#include "client.h"
#include "socketthread.h"

#include <QScopedPointer>
#include <QTcpSocket>
#include <QHostAddress>

#include <QDebug>

Client::Client(QObject *parent) : QObject(parent), _connected(false)
{

}

bool Client::isConnected()
{
    return _connected;
}

void Client::connect(const QHostAddress &host, quint16 port)
{
    QScopedPointer<QTcpSocket> socketPointer(new QTcpSocket);

    _socketThread = new SocketThread(socketPointer.get());

    QObject::connect(_socketThread, &SocketThread::connected, this, &Client::onClientConnected, Qt::QueuedConnection);
    QObject::connect(_socketThread, &SocketThread::disconnected, this, &Client::onClientDisconnected, Qt::QueuedConnection);

    QObject::connect(this, &Client::sendData, _socketThread, &SocketThread::sendData, Qt::QueuedConnection);
    QObject::connect(_socketThread, &SocketThread::receiveData, this, &Client::receiveData, Qt::QueuedConnection);

    QObject::connect(_socketThread, &SocketThread::socketError, this, &Client::socketError);

    QObject::connect(this, &Client::disconnect, _socketThread, &SocketThread::disconnectFromHost);

    QObject::connect(_socketThread, &SocketThread::finished, _socketThread, &SocketThread::deleteLater, Qt::DirectConnection);

    _socketThread->start();

    socketPointer->connectToHost(host, port);

    socketPointer.take();
}

void Client::onClientConnected()
{
    _connected = true;
    emit connected();
}

void Client::onClientDisconnected()
{
    _connected = false;
    emit disconnected();
}
