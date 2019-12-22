#ifndef SERVER_H
#define SERVER_H

#include "socketlistmodel.h"

#include <QAbstractListModel>
#include <QList>
#include <QMap>
#include <QPair>
#include <QTcpServer>
#include <QTcpSocket>

class SocketThread;

class Server : public QTcpServer
{
    Q_OBJECT

public:
    SocketListModel* listModel() const { return _listModel; }

    Server(QObject *parent = 0);
    ~Server();

    void close();
    void newGame();

protected:
    void incomingConnection(qintptr socketDescriptor);

signals:
    void quizSolved();
    void socketError(QTcpSocket::SocketError error);

public slots:
    void onSocketThreadFinished();
    void processData(const QByteArray& data);
    void onBombDefused();
    void onBombExplosed();

private:
    int isEqual (const QString& a, const QString& b) { return !a.compare(b, Qt::CaseInsensitive); }

    SocketListModel *_listModel;
    QMap<SocketThread*,QPair<QString,int>> _questions;

    void setupSocketThread(SocketThread *socketThread);
    void processAnswer(SocketThread* sThread, const QString& answer);

    QPair<QString, int> generateQuiz();

    QByteArray prepareBombStatusJson(bool defused);
    QByteArray prepareQuestionJson(const QString& question);
    QByteArray prepareSolvedJson();
};

#endif // SERVER_H
