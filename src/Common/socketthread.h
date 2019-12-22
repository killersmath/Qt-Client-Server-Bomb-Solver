#ifndef THREAD_H
#define THREAD_H

#include <QTcpSocket>
#include <QThread>

#include <QHostAddress>

class SocketThread : public QThread
{
    Q_OBJECT
public:
    explicit SocketThread(QTcpSocket *socket, QObject *parent = 0);
    ~SocketThread();

    QHostAddress getHost();
    quint16 getPort();

    void connectToHost(const QHostAddress& host, quint16 port);

signals:
    void sendData(const QByteArray& data);
    void receivedData(const QByteArray& data);
    void disconnectFromHost();
    void socketError(QTcpSocket::SocketError error);

#ifdef CLIENT_SIDE
    void connected();
    void disconnected();
#endif

private slots:
    void onDisconnectFromHost();
    void onDisconnected();
    void onReadyRead();
    void onSendData(const QByteArray& data);

#ifdef CLIENT_SIDE
    void socketStateChanged(QTcpSocket::SocketState state);
#endif
private:
    QTcpSocket *_socket;
};

#endif // THREAD_H
