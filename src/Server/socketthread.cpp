#include "socketthread.h"

#include <QDebug>
#include <QHostAddress>
#include <QDataStream>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>

SocketThread::SocketThread(QObject *parent) : QThread(parent), _socket(new QTcpSocket(this))
{

}

SocketThread::~SocketThread()
{
    delete _socket;
}

bool SocketThread::setSocketDescriptor(qintptr socketDescriptor)
{
    return _socket->setSocketDescriptor(socketDescriptor);
}

QString SocketThread::peerHost() const
{
    return _socket->peerAddress().toString();
}

quint16 SocketThread::peerPort() const
{
    return _socket->peerPort();
}

void SocketThread::run()
{
    connect(this, &SocketThread::disconnectFromHost, this, &SocketThread::onDisconnectFromHost);
    connect(_socket, &QTcpSocket::disconnected, this, &SocketThread::onDisconnected);

    connect(_socket, &QTcpSocket::readyRead, this, &SocketThread::onReadyRead);
    connect(this, &SocketThread::sendData, this, &SocketThread::onSendData);

    qDebug() << QString("%1:%2").arg(peerHost()).arg(peerPort()) << " Connected";

    exec();
}

void SocketThread::onDisconnectFromHost()
{
    _socket->disconnectFromHost();
}

void SocketThread::onDisconnected()
{
    qDebug() << QString("%1:%2").arg(peerHost()).arg(peerPort()) << "Disconnected";
    exit(0);
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

