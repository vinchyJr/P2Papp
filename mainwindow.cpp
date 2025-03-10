#include "mainwindow.h"
#include "peer.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), peer(new Peer(7777, this)) {
    qDebug() << "📌 Interface des salles affichée";

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    tabWidget = new QTabWidget(this);

    createRoomPage = new CreateRoom(this);
    joinRoomPage = new JoinRoom(this);

    tabWidget->addTab(createRoomPage, "Créer une salle");
    tabWidget->addTab(joinRoomPage, "Rejoindre une salle");

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->addWidget(tabWidget);

    // 🔹 Ajouter un bouton pour envoyer un fichier
    QPushButton *sendFileButton = new QPushButton("📤 Envoyer un fichier", this);
    layout->addWidget(sendFileButton);

    connect(sendFileButton, &QPushButton::clicked, this, &MainWindow::onSendFileClicked);

    setMinimumSize(700, 500);
}

MainWindow::~MainWindow() {}

void MainWindow::onSendFileClicked() {
    QString filePath = QFileDialog::getOpenFileName(this, "Sélectionner un fichier à envoyer");
    if (!filePath.isEmpty()) {
        qDebug() << "📂 Fichier sélectionné :" << filePath;
        peer->sendFile(filePath);
    } else {
        qDebug() << "⚠ Aucun fichier sélectionné.";
    }
}
