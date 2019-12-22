#include "server.h"
#include "socketthread.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPair>
#include <QRandomGenerator>

#include <QScopedPointer>

Server::Server(QObject *parent) :
    QTcpServer(parent),
    _listModel(new SocketListModel(this))
{

}

Server::~Server()
{
    close();
    delete _listModel;
}

void Server::close()
{
    if(!isListening())
        return;

    QTcpServer::close();
    _listModel->clear();
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    QScopedPointer<QTcpSocket> socketPointer(new QTcpSocket); // auto desctruction

    // set the ID
    if(!socketPointer->setSocketDescriptor(socketDescriptor))
    {
        emit socketError(socketPointer->error());
        return;
    }

    setupSocketThread(new SocketThread(socketPointer.take()));
}

void Server::setupSocketThread(SocketThread *socketThread)
{
    connect(socketThread, &SocketThread::finished, this, &Server::onSocketThreadFinished);
    connect(socketThread, &SocketThread::finished, socketThread, &SocketThread::deleteLater);
    connect(socketThread, &SocketThread::receivedData, this, &Server::processData);

    _listModel->addSocket(qMakePair(socketThread->getHost().toString(), socketThread->getPort()), socketThread);

    socketThread->start();
}

void Server::onSocketThreadFinished()
{
    SocketThread *socketThread = dynamic_cast<SocketThread*>(sender());

    if(!socketThread)
        return;

    _listModel->removeSocket(socketThread);

}

void Server::processData(const QByteArray &data)
{
    const QJsonDocument doc = QJsonDocument::fromJson(data);
    const QJsonObject obj = doc.object();
    const QJsonValue typeJsonValue = obj["type"];

    if (typeJsonValue.isNull() || !typeJsonValue.isString())
        return;

    const QString type = typeJsonValue.toString();

    if(isEqual(type, "answer")){
        SocketThread *socketThread = dynamic_cast<SocketThread*>(sender());

        if(!socketThread) return;

        processAnswer(socketThread, obj["answer"].toString());
    }
}

void Server::processAnswer(SocketThread *sThread, const QString& answer)
{
    bool isNumber;
    int value = answer.toInt(&isNumber);

    if(!isNumber)
        return;

    const QPair<QString, int> quiz = _questions[sThread];
    const int quizAnswer = quiz.second;

    if(quizAnswer == value){
        _questions.remove(sThread);

        if(!_questions.isEmpty()){
            sThread->sendData(prepareSolvedJson());
        }
        else{
            emit quizSolved();
        }
    }
}

void Server::newGame()
{
    _questions.clear();

    for(const SocketListModel::DataInfo& data : _listModel->dataList()){
        const QPair<QString, int> quiz = generateQuiz();
        SocketThread *socketThread = data.second;

        _questions.insert(socketThread, quiz);
        socketThread->sendData(prepareQuestionJson(quiz.first));
    }
}

QPair<QString, int> Server::generateQuiz()
{
    //int a = QRandomGenerator::global()->bounded(10, 120);
    int a = 10;
    int b = QRandomGenerator::global()->bounded(10, 120);
    int answer = a*b;

    const QString question = QString("%1 * %2").arg(a).arg(b);

    return qMakePair(question, answer);
}

void Server::onBombDefused()
{
    for(const SocketListModel::DataInfo& data : _listModel->dataList()){
        data.second->sendData(prepareBombStatusJson(true));
    }
}

void Server::onBombExplosed()
{
    for(const SocketListModel::DataInfo& data : _listModel->dataList()){
        data.second->sendData(prepareBombStatusJson(false));
    }
}

QByteArray Server::prepareQuestionJson(const QString &question)
{
    QJsonDocument doc;
    QJsonObject obj;
    obj["type"] = "question";
    obj["question"] = question;
    doc.setObject(obj);
    return doc.toJson(QJsonDocument::Compact);
}

QByteArray Server::prepareSolvedJson()
{
    QJsonDocument doc;
    QJsonObject obj;
    obj["type"] = "solved";
    doc.setObject(obj);
    return doc.toJson(QJsonDocument::Compact);
}

QByteArray Server::prepareBombStatusJson(bool defused)
{
    QJsonDocument doc;
    QJsonObject obj;
    obj["type"] = "bombstatus";
    obj["defused"] = defused;
    doc.setObject(obj);
    return doc.toJson(QJsonDocument::Compact);
}
