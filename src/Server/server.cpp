#include "server.h"
#include "socketthread.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPair>
#include <QRandomGenerator>

Server::Server(QObject *parent) : QTcpServer(parent), _listModel(new SocketListModel(this))
{

}

Server::~Server()
{
    close();
    _listModel->deleteLater();
}

void Server::close()
{
    if(isListening()){
        QTcpServer::close();
        _listModel->clear();
    }
}

SocketListModel *Server::listModel() const
{
    return _listModel;
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    SocketThread *socketThread = new SocketThread;

    if(!socketThread->setSocketDescriptor(socketDescriptor)){
        qDebug() << "Socket Descriptor Error";
        socketThread->deleteLater();
        return;
    }

    connect(socketThread, &SocketThread::finished, this, &Server::onSocketThreadFinished);
    connect(socketThread, &SocketThread::finished, socketThread, &SocketThread::deleteLater);
    connect(socketThread, &SocketThread::receiveData, this, &Server::onReveiceData);

    _listModel->addSocket(qMakePair(socketThread->peerHost(), socketThread->peerPort()),socketThread);

    socketThread->start();
}

void Server::onSocketThreadFinished()
{
    SocketThread *socketThread = dynamic_cast<SocketThread*>(sender());

    if(socketThread){
        _listModel->removeSocket(socketThread);
    }
}

void Server::onReveiceData(const QByteArray &data)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(data);
    QJsonObject jsonObject = jsonDocument.object();

    const QJsonValue typeVal = jsonObject.value(QLatin1String("type"));
    if (typeVal.isNull() || !typeVal.isString())
        return;

    if(typeVal.toString().compare(QLatin1String("answer"), Qt::CaseInsensitive) == 0){
        bool ok;
        int number = jsonObject["value"].toString().toInt(&ok);

        if(ok){
            SocketThread *socketThread = dynamic_cast<SocketThread*>(sender());
            if(socketThread){
                if(_questions[socketThread].second == number){
                    _questions.remove(socketThread);

                    if(!_questions.isEmpty()){
                        QJsonDocument jsonDocument;
                        QJsonObject jsonObject;
                        jsonObject["type"] = "solved";
                        jsonDocument.setObject(jsonObject);
                        socketThread->sendData(jsonDocument.toJson(QJsonDocument::Compact));
                    }
                    else{
                        emit challangeSolved();
                    }
                }
            }
        }
    }
}

void Server::onBombDefused()
{
    qDebug() << "Called";

    SocketListModel::DataList dataList = _listModel->getData();

    for(SocketListModel::DataInfo data : dataList){
        QJsonDocument doc;
        QJsonObject obj;
        obj["type"] = "bombstatus";
        obj["defused"] = true;
        doc.setObject(obj);
        data.second->sendData(doc.toJson(QJsonDocument::Compact));
    }
}

void Server::onBombExplosed()
{
    SocketListModel::DataList dataList = _listModel->getData();

    for(SocketListModel::DataInfo data : dataList){
        QJsonDocument doc;
        QJsonObject obj;
        obj["type"] = "bombstatus";
        obj["defused"] = false;
        doc.setObject(obj);
        data.second->sendData(doc.toJson(QJsonDocument::Compact));
    }
}

void Server::startChallenge()
{
    SocketListModel::DataList dataList = _listModel->getData();

    _questions.clear();

    for(SocketListModel::DataInfo data : dataList){
        //int a = QRandomGenerator::global()->bounded(10, 120);
        int a = 10;
        int b = QRandomGenerator::global()->bounded(10, 120);
        int answer = a*b;

        const QString question = QString("%1 * %2").arg(a).arg(b);

        _questions.insert(data.second, qMakePair(question, answer));

        QJsonDocument doc;
        QJsonObject obj;
        obj["type"] = "question";
        obj["text"] = question;
        doc.setObject(obj);
        data.second->sendData(doc.toJson(QJsonDocument::Compact));
    }
}
