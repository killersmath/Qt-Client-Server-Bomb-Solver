#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

#include <QMainWindow>
#include <QJsonObject>
#include <QTcpSocket>

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void onConnectButtonClicked();
    void onSocketConnected();
    void onSocketDisconnected();

    void onSendButtonClicked();
    void onSocketReadyRead();
private:
    QTcpSocket _socket;

    void readJson(const QJsonObject &jsonObject);

};

#endif // MAINWINDOW_H
