#include "mainwindow.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    tabWidget = new QTabWidget(this);

    createRoomPage = new CreateRoom(this);
    joinRoomPage = new JoinRoom(this);

    tabWidget->addTab(createRoomPage, "Créer une salle");
    tabWidget->addTab(joinRoomPage, "Rejoindre une salle");

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->addWidget(tabWidget);

    setMinimumSize(700, 500);
}

MainWindow::~MainWindow() {}
