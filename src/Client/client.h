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

    bool isConnected() { return _connected; }
    int isEqual (const QString& a, const QString& b) { return !a.compare(b, Qt::CaseInsensitive); }

    explicit Client(QObject *parent = nullptr);
    ~Client();

    void connect(const QHostAddress &host, quint16 port);
    void sendAnswer(const QString &answer);

signals:
    void disconnect();

    void connected();
    void disconnected();

    void sendData(const QByteArray &data);
    void socketError(QTcpSocket::SocketError error);

    void newQuestion(const QString& question);
    void bombStatusChanged(bool defused);
    void solved();

private slots:
    void onClientConnected();
    void onClientDisconnected();

    void processData(const QByteArray &data);

private:
    SocketThread *_socketThread;
    bool _connected;

    QByteArray prepareAnswerJson(const QString &answer);
};

#endif // CLIENT_H
