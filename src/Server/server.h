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
    Server(QObject *parent = 0);
    ~Server();

    void close();
    SocketListModel* listModel() const;

    void startChallenge();

protected:
    void incomingConnection(qintptr socketDescriptor);

signals:
    void challangeSolved();

    void socketError(QTcpSocket::SocketError error);

public slots:
    void onSocketThreadFinished();
    void onReveiceData(const QByteArray& data);

    void onBombDefused();
    void onBombExplosed();

private:
    SocketListModel *_listModel;
    QMap<SocketThread*,QPair<QString,int>> _questions;

};

#endif // SERVER_H
