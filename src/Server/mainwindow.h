#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "server.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class Bomb;

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private:    
    QGraphicsScene *_welcomeScene, *_gameScene;
    QGraphicsScene *_goodEndScene, *_badEndScene;

    Bomb *_bomb;
    Server _server;

    QString _settingsFileLocation;

    void setupWidgets();
    void setupConnections();

    void loadSettings();
    void saveSettings();

private slots:
    void newGameButtonClicked();
    void connectServerButtonClicked();

    void onBombDefused();
    void onBombExplosed();
};

#endif // MAINWINDOW_H
