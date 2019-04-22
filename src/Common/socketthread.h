#ifndef THREAD_H
#define THREAD_H

#include <QTcpSocket>
#include <QThread>

class SocketThread : public QThread
{
    Q_OBJECT
public:
    explicit SocketThread(QTcpSocket *socket, QObject *parent = 0);
    ~SocketThread();

signals:
    void sendData(const QByteArray& data);
    void receiveData(const QByteArray& data);

    void socketError(QTcpSocket::SocketError error);

    void disconnectFromHost();

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
