#include "mainwindow.h"

#include <QDataStream>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>

#include <QRegularExpression>
#include <QValidator>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);

    setFixedSize(width(), height());

    connect(connectPushButton, &QPushButton::clicked, this, &MainWindow::onConnectButtonClicked);
    connect(sendButton, &QPushButton::clicked, this, &MainWindow::onSendButtonClicked);

    connect(&_socket, &QTcpSocket::connected, this, &MainWindow::onSocketConnected);
    connect(&_socket, &QTcpSocket::disconnected, this, &MainWindow::onSocketDisconnected);
    connect(&_socket, &QTcpSocket::readyRead, this, &MainWindow::onSocketReadyRead);

    const QString numberRange = QString("[1-9][0-9]?|1[0-9][0-9]|2[0-5][0-5]");

    QRegularExpression IPrx(QString("^(%1)\\.(0|%2)\\.(0|%3)\\.(0|%4)$").arg(numberRange, numberRange, numberRange, numberRange));
    QValidator *IPvalidator = new QRegularExpressionValidator(IPrx, this);

    hostLineEdit->setValidator(IPvalidator);
}

MainWindow::~MainWindow()
{
    _socket.disconnectFromHost();
}

void MainWindow::onConnectButtonClicked()
{
    if(_socket.state() != QTcpSocket::ConnectedState){
        _socket.connectToHost(QHostAddress(hostLineEdit->text().trimmed()), portSpinBox->text().toInt());
        _socket.waitForConnected(5000);
    }
    else{
        _socket.disconnectFromHost();
    }
}

void MainWindow::onSocketConnected()
{
    connectPushButton->setText(QStringLiteral("Disconnect"));
    hostLineEdit->setEnabled(false);
    portSpinBox->setEnabled(false);
}

void MainWindow::onSocketDisconnected()
{
    connectPushButton->setText(QStringLiteral("Connect"));
    hostLineEdit->setEnabled(true);
    portSpinBox->setEnabled(true);
    contentLabel->clear();
    sendLineEdit->setEnabled(false);
    sendButton->setEnabled(false);
}

void MainWindow::onSendButtonClicked()
{
    QJsonDocument jsonDocument;
    QJsonObject jsonObject;
    jsonObject["type"] = "answer";
    jsonObject["value"] = sendLineEdit->text();
    jsonDocument.setObject(jsonObject);

    QDataStream socketStream(&_socket);
    socketStream.setVersion(QDataStream::Qt_5_11);
    socketStream << jsonDocument.toJson(QJsonDocument::Compact);
}

void MainWindow::onSocketReadyRead()
{
    QByteArray data;

    QDataStream socketStream(&_socket);
    socketStream.setVersion(QDataStream::Qt_5_11);

    for (;;) {
        socketStream.startTransaction();
        socketStream >> data;
        if (socketStream.commitTransaction()) {
            QJsonParseError parseError;
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseError);
            if (parseError.error == QJsonParseError::NoError) {
                if (jsonDoc.isObject())
                    readJson(jsonDoc.object());
            }
        } else {
            break;
        }
    }
}

void MainWindow::readJson(const QJsonObject &jsonObject)
{
    const QJsonValue typeVal = jsonObject.value(QLatin1String("type"));
    if (typeVal.isNull() || !typeVal.isString())
        return;

    if (typeVal.toString().compare(QLatin1String("question"), Qt::CaseInsensitive) == 0) {
        contentLabel->setText(jsonObject["text"].toString());
        sendLineEdit->clear();
        sendButton->setEnabled(true);
        sendLineEdit->setEnabled(true);
    }
    else if(typeVal.toString().compare(QLatin1String("solved"), Qt::CaseInsensitive) == 0){
        sendButton->setEnabled(false);
        sendLineEdit->setEnabled(false);
    }
    else if(typeVal.toString().compare(QLatin1String("bombstatus"), Qt::CaseInsensitive) == 0){
        contentLabel->setText(jsonObject["defused"].toBool() ? "The Bomb has been Defused" : " KABUM ");
        sendLineEdit->setEnabled(false);
        sendButton->setEnabled(false);
    }
}
