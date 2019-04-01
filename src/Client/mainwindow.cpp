#include "mainwindow.h"

#include <QDataStream>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>

#include <QRegularExpression>
#include <QValidator>

#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), _settingsFileLocation(QApplication::applicationDirPath() + "/settings.ini")
{
    setupUi(this);

    setFixedSize(width(), height());

    setupWidgets();
    setupConnections();

    loadSettings();
}

MainWindow::~MainWindow()
{
    _client.disconnect();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    QMainWindow::closeEvent(event);
}

void MainWindow::setupWidgets()
{
    const QString numberRange = QString("[1-9][0-9]?|1[0-9][0-9]|2[0-5][0-5]");

    QRegularExpression IPrx(QString("^(%1)\\.(0|%2)\\.(0|%3)\\.(0|%4)$").arg(numberRange, numberRange, numberRange, numberRange));
    QValidator *IPvalidator = new QRegularExpressionValidator(IPrx, this);

    hostLineEdit->setValidator(IPvalidator);
}

void MainWindow::setupConnections()
{
    connect(connectPushButton, &QPushButton::clicked, this, &MainWindow::onConnectButtonClicked);
    connect(sendButton, &QPushButton::clicked, this, &MainWindow::onSendButtonClicked);

    connect(&_client, &Client::connected, this, &MainWindow::onClientConnected);
    connect(&_client, &Client::disconnected, this, &MainWindow::onClientDisconnected);
    connect(&_client, &Client::receiveData, this, &MainWindow::onClientReceiveData);
    connect(&_client, &Client::socketError, this, [this](QTcpSocket::SocketError error){
        switch(error){
        case QTcpSocket::ConnectionRefusedError:
            windowStatusBar->showMessage(QLatin1String("Connection Refused."), 2000);
            break;
        case QTcpSocket::RemoteHostClosedError:
            windowStatusBar->showMessage(QLatin1String("Remove Host has been closed."), 2000);
            break;
        default:
            break;
        }
    });

}

void MainWindow::onConnectButtonClicked()
{
    if(!_client.isConnected()){
        _client.connect(QHostAddress(hostLineEdit->text().trimmed()), portSpinBox->text().toInt());

        connectPushButton->setText("Trying...");
        connectPushButton->setEnabled(false);
    }
    else{
        _client.disconnect();
    }
}

void MainWindow::onClientConnected()
{
    connectPushButton->setText(QStringLiteral("Disconnect"));
    connectPushButton->setEnabled(true);

    hostLineEdit->setEnabled(false);
    portSpinBox->setEnabled(false);

    windowStatusBar->showMessage(QLatin1String("Connected."), 2000);
}

void MainWindow::onClientDisconnected()
{
    connectPushButton->setText(QStringLiteral("Connect"));
    connectPushButton->setEnabled(true);

    hostLineEdit->setEnabled(true);
    portSpinBox->setEnabled(true);

    contentLabel->clear();

    sendLineEdit->setEnabled(false);
    sendButton->setEnabled(false);

    windowStatusBar->showMessage(QLatin1String("Disconnected."), 2000);
}

void MainWindow::onSendButtonClicked()
{
    QJsonDocument jsonDocument;
    QJsonObject jsonObject;
    jsonObject["type"] = "answer";
    jsonObject["value"] = sendLineEdit->text();
    jsonDocument.setObject(jsonObject);

    _client.sendData(jsonDocument.toJson(QJsonDocument::Compact));
}

void MainWindow::onClientReceiveData(const QByteArray &data)
{
    QJsonObject jsonObject = QJsonDocument::fromJson(data).object();

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

void MainWindow::loadSettings()
{
    QSettings settings(_settingsFileLocation, QSettings::IniFormat);

    settings.beginGroup("server");
    const QString serverHost = settings.value("host", "127.0.0.1").toString();
    const int serverPort = settings.value("port", 1024).toInt();
    settings.endGroup();

    hostLineEdit->setText(serverHost);
    portSpinBox->setValue(serverPort);

}

void MainWindow::saveSettings()
{
    QSettings settings(_settingsFileLocation, QSettings::IniFormat);

    const QString serverHost = hostLineEdit->text();

    settings.beginGroup("server");
    settings.setValue("host", serverHost.isEmpty() ? QLatin1String("127.0.0.1") : serverHost);
    settings.setValue("port", portSpinBox->value());
    settings.endGroup();

    settings.sync();
}
