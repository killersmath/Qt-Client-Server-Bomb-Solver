#include "client.h"
#include "socketthread.h"

#include <QScopedPointer>
#include <QTcpSocket>
#include <QHostAddress>

#include <QDebug>

#include <QJsonObject>
#include <QJsonDocument>

Client::Client(QObject *parent) :
    QObject(parent),
    _socketThread(new SocketThread(new QTcpSocket)),
    _connected(false)
{
    QObject::connect(_socketThread, &SocketThread::connected, this, &Client::onClientConnected, Qt::QueuedConnection);
    QObject::connect(_socketThread, &SocketThread::disconnected, this, &Client::onClientDisconnected, Qt::QueuedConnection);
    QObject::connect(this, &Client::disconnect, _socketThread, &SocketThread::disconnectFromHost);
    QObject::connect(_socketThread, &SocketThread::socketError, this, &Client::socketError);

    QObject::connect(this, &Client::sendData, _socketThread, &SocketThread::sendData, Qt::QueuedConnection);
    QObject::connect(_socketThread, &SocketThread::receivedData, this, &Client::processData, Qt::QueuedConnection);
}

Client::~Client()
{
    if(_connected)
        _socketThread->disconnectFromHost();

    if(_socketThread->isRunning()){
        _socketThread->terminate();
        _socketThread->wait();
    }

    delete _socketThread;
}

void Client::connect(const QHostAddress &host, quint16 port)
{
    _socketThread->start();
    _socketThread->connectToHost(host, port);
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

void Client::processData(const QByteArray &data)
{
    const QJsonDocument doc = QJsonDocument::fromJson(data);
    const QJsonObject obj = doc.object();
    const QJsonValue typeJsonValue = obj["type"];

    if (typeJsonValue.isNull() || !typeJsonValue.isString())
        return;

    const QString type = typeJsonValue.toString();

    if (isEqual(type, "question")) {
        emit newQuestion(obj["question"].toString());
    }
    else if(isEqual(type, "solved")){
        emit solved();
    }
    else if(isEqual(type, "bombstatus")){
        emit bombStatusChanged(obj["defused"].toBool());
    }
}

void Client::sendAnswer(const QString &answer)
{
    emit sendData(prepareAnswerJson(answer));
}

QByteArray Client::prepareAnswerJson(const QString &answer)
{
    QJsonDocument doc;
    QJsonObject obj;
    obj["type"] = "answer";
    obj["answer"] = answer;
    doc.setObject(obj);

    return doc.toJson(QJsonDocument::Compact);
}

