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

public slots:
    void onConnectButtonClicked();

    void onClientConnected();
    void onClientDisconnected();

    void onSendButtonClicked();

    void onClientReceiveData(const QByteArray &data);

private:
    const QString _settingsFileLocation;
    Client _client;

    void setupWidgets();
    void setupConnections();

    void loadSettings();
    void saveSettings();
};

#endif // MAINWINDOW_H
