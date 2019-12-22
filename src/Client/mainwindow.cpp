#include "mainwindow.h"

#include <QDataStream>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>

#include <QRegularExpression>
#include <QValidator>

#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _settingsFileLocation(QApplication::applicationDirPath() + "/settings.ini")
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
    // interface
    connect(connectPushButton, &QPushButton::clicked, this, &MainWindow::onConnectButtonClicked);
    connect(sendButton, &QPushButton::clicked, this, &MainWindow::onSendButtonClicked);

    // client
    connect(&_client, &Client::connected, this, &MainWindow::onClientConnected);
    connect(&_client, &Client::disconnected, this, &MainWindow::onClientDisconnected);
    connect(&_client, &Client::socketError, this, &MainWindow::onClientSocketError);

    // reply
    connect(&_client, &Client::newQuestion, this, &MainWindow::onNewQuestion);
    connect(&_client, &Client::solved, this, &MainWindow::onSolved);
    connect(&_client, &Client::bombStatusChanged, this, &MainWindow::onBombStatusChanged);

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

void MainWindow::onConnectButtonClicked()
{
    if(!_client.isConnected()){
        _client.connect(QHostAddress(hostLineEdit->text().trimmed()), portSpinBox->text().toInt());

        connectPushButton->setText(QStringLiteral("Trying..."));
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

    windowStatusBar->showMessage(QStringLiteral("Connected."), 2000);
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

    windowStatusBar->showMessage(QStringLiteral("Disconnected."), 2000);
}

void MainWindow::onClientSocketError(const QAbstractSocket::SocketError error)
{
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
}

void MainWindow::onSendButtonClicked()
{
    _client.sendAnswer(sendLineEdit->text().trimmed());
}

void MainWindow::onNewQuestion(const QString &question)
{
    contentLabel->setText(question);
    sendLineEdit->clear();
    sendButton->setEnabled(true);
    sendLineEdit->setEnabled(true);
}

void MainWindow::onSolved()
{
    sendButton->setEnabled(false);
    sendLineEdit->setEnabled(false);
}

void MainWindow::onBombStatusChanged(bool defused)
{
    contentLabel->setText(defused ? "The Bomb has been Defused" : " KABUM ");
    sendLineEdit->setEnabled(false);
    sendButton->setEnabled(false);
}
