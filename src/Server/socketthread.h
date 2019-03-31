#ifndef THREAD_H
#define THREAD_H

#include <QTcpSocket>
#include <QThread>

class SocketThread : public QThread
{
    Q_OBJECT
public:
    explicit SocketThread(QObject *parent = 0);
    ~SocketThread();

    bool setSocketDescriptor(qintptr socketDescriptor);

    QString peerHost() const;
    quint16 peerPort() const;

protected:
    void run() override;

signals:
    void sendData(const QByteArray& data);
    void receiveData(const QByteArray& data);
    void disconnectFromHost();

private slots:
    void onDisconnectFromHost();
    void onDisconnected();
    void onReadyRead();
    void onSendData(const QByteArray& data);

private:
    QTcpSocket *_socket;
};

#endif // THREAD_H
