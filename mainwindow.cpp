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

    // ✅ Ajouter une liste pour afficher les fichiers transférés
    fileListWidget = new QListWidget(this);
    fileListWidget->setMinimumHeight(100);
    layout->addWidget(fileListWidget);

    // ✅ Ajouter un bouton pour envoyer un fichier
    sendFileButton = new QPushButton("📤 Envoyer un fichier", this);
    layout->addWidget(sendFileButton);

    // ✅ Connexion du bouton pour l'envoi de fichiers
    connect(sendFileButton, &QPushButton::clicked, this, &MainWindow::onSendFileClicked);

    // ✅ Connexion du signal pour l'affichage des fichiers reçus
    connect(peer, &Peer::fileReceived, this, &MainWindow::onFileReceived);

    setMinimumSize(700, 500);
}

MainWindow::~MainWindow() {}

void MainWindow::onSendFileClicked() {
    QString filePath = QFileDialog::getOpenFileName(this, "Sélectionner un fichier à envoyer");
    if (!filePath.isEmpty()) {
        qDebug() << "📂 Fichier sélectionné :" << filePath;
        peer->sendFile(filePath);
        fileListWidget->addItem("📤 Envoyé : " + filePath);  // ✅ Ajoute le fichier à la liste
    } else {
        qDebug() << "⚠ Aucun fichier sélectionné.";
    }
}

// ✅ Ajoute un fichier reçu à la liste
void MainWindow::onFileReceived(QString fileName) {
    fileListWidget->addItem("📥 Reçu : " + fileName);
}
