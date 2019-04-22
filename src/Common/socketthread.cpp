#include "socketthread.h"

#include <QDebug>
#include <QHostAddress>
#include <QDataStream>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>

SocketThread::SocketThread(QTcpSocket *socket, QObject *parent) : QThread(parent), _socket(socket)
{
    connect(_socket, &QTcpSocket::disconnected, this, &SocketThread::onDisconnected);
    connect(this, &SocketThread::disconnectFromHost, this, &SocketThread::onDisconnectFromHost);

    connect(_socket, &QTcpSocket::readyRead, this, &SocketThread::onReadyRead);
    connect(this, &SocketThread::sendData, this, &SocketThread::onSendData);

    connect(_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error), this, &SocketThread::socketError);

#ifdef CLIENT_SIDE
    connect(_socket, &QTcpSocket::stateChanged, this, &SocketThread::socketStateChanged);
#else
    qDebug() << QString("%1:%2").arg(_socket->peerAddress().toString()).arg(_socket->peerPort()) << " Connected";
#endif
}

SocketThread::~SocketThread()
{
    if(_socket->isOpen())
        _socket->close();

    delete _socket;
}

void SocketThread::onDisconnectFromHost()
{
    _socket->disconnectFromHost();
}

void SocketThread::onDisconnected()
{
    qDebug() << QString("%1:%2").arg(_socket->peerAddress().toString()).arg(_socket->peerPort()) << "Disconnected";
#ifdef CLIENT_SIDE
    emit disconnected();
#endif
    exit(); // exit from event loop
}

void SocketThread::onReadyRead()
{
    QByteArray data;

    QDataStream socketStream(_socket);
    socketStream.setVersion(QDataStream::Qt_5_11);

    for (;;) {
        socketStream.startTransaction();
        socketStream >> data;
        if (socketStream.commitTransaction()) {
            QJsonParseError parseError;
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseError);
            if (parseError.error == QJsonParseError::NoError) {
                if (jsonDoc.isObject())
                    emit receiveData(data);
            }
        } else {
            break;
        }
    }
}

void SocketThread::onSendData(const QByteArray &data)
{
    QDataStream clientStream(_socket);
    clientStream.setVersion(QDataStream::Qt_5_11);
    clientStream << data;
}

#ifdef CLIENT_SIDE
void SocketThread::socketStateChanged(QAbstractSocket::SocketState state)
{
    if(state == QTcpSocket::ConnectedState){
        qDebug() << QString("%1:%2").arg(_socket->peerAddress().toString()).arg(_socket->peerPort()) << " Connected";
        disconnect(_socket, &QTcpSocket::stateChanged, this, &SocketThread::socketStateChanged);
        emit connected();
    }
    else if(state == QAbstractSocket::UnconnectedState){
        qDebug() << "Recusado";
        emit disconnected();
        exit();
    }
}
#endif
