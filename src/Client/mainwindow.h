#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

#include <QMainWindow>
#include <QJsonObject>
#include <QTcpSocket>

#include "client.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void onConnectButtonClicked();

    void onClientConnected();
    void onClientDisconnected();
    void onClientSocketError(const QTcpSocket::SocketError error);

    void onSendButtonClicked();

    void onNewQuestion(const QString& question);
    void onSolved();
    void onBombStatusChanged(bool defused);


private:
    const QString _settingsFileLocation;
    Client _client;

    void setupWidgets();
    void setupConnections();

    void loadSettings();
    void saveSettings();
};

#endif // MAINWINDOW_H
