#include "mainwindow.h"
#include "bombitem.h"

#include <QDebug>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QRandomGenerator>
#include <QNetworkInterface>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QTcpSocket>
#include <QTimer>

#include <QSettings>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), _settingsFileLocation(QApplication::applicationDirPath() + "/settings.ini")
{
    setupUi(this);

    setupWidgets();
    setupConnections();

    loadSettings();
}

MainWindow::~MainWindow()
{

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    QMainWindow::closeEvent(event);
}

void MainWindow::setupWidgets()
{
    // Host Ip Validator
    const QString numberRange = QString("[1-9][0-9]?|1[0-9][0-9]|2[0-5][0-5]");
    QRegularExpression IPrx(QString("^(%1)\\.(0|%2)\\.(0|%3)\\.(0|%4)$").arg(numberRange, numberRange, numberRange, numberRange));
    QValidator *IPvalidator = new QRegularExpressionValidator(IPrx, this);
    hostLineEdit->setValidator(IPvalidator);

    // Client List
    listView->setModel(_server.listModel());

    // WELCOME SCENE
    _welcomeScene = new QGraphicsScene(this);
    _welcomeScene->setItemIndexMethod(QGraphicsScene::NoIndex);
    _welcomeScene->addText("Click on New Game Button to start the game.", QFont("Arial", 15));
    _welcomeScene->setSceneRect(_welcomeScene->itemsBoundingRect());

    // GAME SCENE
    _gameScene = new QGraphicsScene(this);
    _gameScene->setItemIndexMethod(QGraphicsScene::NoIndex);
    _bomb = new Bomb;
    _gameScene->addItem(_bomb);
    _gameScene->setSceneRect(_gameScene->itemsBoundingRect());

    //  GOOD ENDING SCENE
    _goodEndScene = new QGraphicsScene(this);
    _goodEndScene->setItemIndexMethod(QGraphicsScene::NoIndex);
    QGraphicsTextItem* GTitem = _goodEndScene->addText("The Bomb has Been Defused", QFont("Arial", 18));
    GTitem->setDefaultTextColor(QColor(0, 255, 0));
    _goodEndScene->setSceneRect(_goodEndScene->itemsBoundingRect());

    // BAD ENDING SCENE
    _badEndScene = new QGraphicsScene(this);
    _badEndScene->setItemIndexMethod(QGraphicsScene::NoIndex);
    QGraphicsTextItem* BTitem = _badEndScene->addText("KABBUMMM", QFont("Arial", 18));
    BTitem->setDefaultTextColor(QColor(255, 0, 0));
    _badEndScene->setSceneRect(_badEndScene->itemsBoundingRect());

    // GRAPHICS VIEW SETTINGS
    graphicsView->setBackgroundBrush(QBrush(Qt::gray));
    graphicsView->setRenderHint(QPainter::Antialiasing);
    graphicsView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    graphicsView->setScene(_welcomeScene);
}

void MainWindow::setupConnections()
{
    connect(newGameAction, &QAction::triggered, this, &MainWindow::newGameButtonClicked);

    connect(helpAction, &QAction::triggered, this, [this](){
        QMessageBox::information(this,
                                 QStringLiteral("Help Content"),
                                 QStringLiteral("The objetive is defuse the bomb by solving math problems.\n\n") +
                                 QStringLiteral("To play the game, you need to follow the below instructions:\n\n") +
                                 QStringLiteral("1) Insert the IP and Port server Info and start the Server Mode.\n") +
                                 QStringLiteral("2) Wait until one or more clients connect to the server.\n") +
                                 QStringLiteral("3) Click on New Game Button at Top Menu Bar.\n\n") +
                                 QStringLiteral("A Math Expression will be send to everyone connected to the server.\n\n") +
                                 QStringLiteral("If everyone solve the math expression, the bomb will be the defused, otherwhise, it will be explose.")
                                 );
    });

    connect(aboutAction, &QAction::triggered, this, [this](){
        QMessageBox::about(this,
                           QStringLiteral("About the Program"),
                           QStringLiteral("Maxsuel Filomeno & XXX\n") +
                           QStringLiteral("Distributed Systems. March, 2019\n") +
                           QStringLiteral("Universidade do Estado de Mato Grosso, UNEMAT, Brazil\n")
                           );
    });

    connect(connectServerButton, &QPushButton::clicked, this, &MainWindow::connectServerButtonClicked);

    connect(_bomb, &Bomb::defused, this, &MainWindow::onBombDefused);
    connect(_bomb, &Bomb::defused, &_server, &Server::onBombDefused);

    connect(_bomb, &Bomb::explosed, this, &MainWindow::onBombExplosed);
    connect(_bomb, &Bomb::explosed, &_server, &Server::onBombExplosed);

    connect(&_server, &Server::challangeSolved, _bomb, &Bomb::defused);

    connect(_server.listModel(), &SocketListModel::socketAdded, this, [=](const QString &host, quint16 port){
        const QString message = QString("%1:%2 has been connected.").arg(host).arg(port);
        windowStatusBar->showMessage(message, 2000);
    });

    connect(_server.listModel(), &SocketListModel::socketRemoved, this, [this](const QString &host, quint16 port){
        const QString message = QString("%1:%2 has been disconnected.").arg(host).arg(port);
        windowStatusBar->showMessage(message, 2000);
    });
}

void MainWindow::loadSettings()
{
    QSettings settings(_settingsFileLocation, QSettings::IniFormat);

    settings.beginGroup("window");

    if(settings.contains("geometry"))
        restoreGeometry(settings.value("geometry").toByteArray());

    if(settings.contains("state"))
        restoreState(settings.value("state").toByteArray());

    settings.endGroup();

    settings.beginGroup("server");
    QString serverHost = settings.value("host", "").toString();
    const int serverPort = settings.value("port", 1024).toInt();
    settings.endGroup();

    settings.beginGroup("bomb");
    const int bombTimer = settings.value("timer", 15).toInt();
    settings.endGroup();

    if(serverHost.isEmpty()){
        QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
        // Find the first no local address
        for (int i = 0; i < ipAddressesList.size(); ++i) {
            if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
                    ipAddressesList.at(i).toIPv4Address()) {
                serverHost = ipAddressesList.at(i).toString();
                break;
            }
        }
        // If din't found, use Localhost address
        if (serverHost.isEmpty())
            serverHost = QHostAddress(QHostAddress::LocalHost).toString();
    }

    hostLineEdit->setText(serverHost);
    portSpinBox->setValue(serverPort);
    bombTimerSpinBox->setValue(bombTimer);
}

void MainWindow::saveSettings()
{
    QSettings settings(_settingsFileLocation, QSettings::IniFormat);

    const QString serverHost = hostLineEdit->text();

    settings.beginGroup("window");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("state", saveState());
    settings.endGroup();

    settings.beginGroup("server");
    settings.setValue("host", serverHost.isEmpty() ? QLatin1String("127.0.0.1") : serverHost);
    settings.setValue("port", portSpinBox->value());
    settings.endGroup();

    settings.beginGroup("bomb");
    settings.setValue("timer", bombTimerSpinBox->value());
    settings.endGroup();

    settings.sync();
}

void MainWindow::newGameButtonClicked()
{
    newGameAction->setEnabled(false);

    _server.sendQuestions();
    graphicsView->setScene(_gameScene);
    _bomb->setupBomb(bombTimerSpinBox->cleanText().toInt());
}

void MainWindow::connectServerButtonClicked()
{
    if(!_server.isListening()){
        if(_server.listen(QHostAddress(hostLineEdit->text().trimmed()), portSpinBox->text().toInt())){
            connectServerButton->setText(QStringLiteral("Stop Server"));
            newGameAction->setEnabled(true);
            hostLineEdit->setEnabled(false);
            portSpinBox->setEnabled(false);
            windowStatusBar->showMessage(QStringLiteral("The server is listening in (%1:%2)").arg(_server.serverAddress().toString()).arg(_server.serverPort()), 2000);
        }
    }
    else{
        _server.close();
        graphicsView->setScene(_welcomeScene);
        _bomb->stopBomb();
        connectServerButton->setText(QStringLiteral("Start Server"));
        newGameAction->setEnabled(false);
        hostLineEdit->setEnabled(true);
        portSpinBox->setEnabled(true);
        windowStatusBar->showMessage(QStringLiteral("The server is not listening anymore."), 2000);
    }
}

void MainWindow::onBombDefused()
{
    graphicsView->setScene(_goodEndScene);
    newGameAction->setEnabled(true);
}

void MainWindow::onBombExplosed()
{
    graphicsView->setScene(_badEndScene);
    newGameAction->setEnabled(true);
}
