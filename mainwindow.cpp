#include "mainwindow.h"
#include "peer.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
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
        fileListWidget->addItem("📤 Envoyé : " + filePath);  // ✅ Ajoute le fichier envoyé à la liste
    } else {
        qDebug() << "⚠ Aucun fichier sélectionné.";
    }
}

// ✅ Gère la réception des fichiers avec confirmation utilisateur
void MainWindow::onFileReceived(QString fileName, QByteArray data) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "📥 Réception de fichier",
                                  "Un fichier nommé '" + fileName + "' a été reçu.\nVoulez-vous l'enregistrer ?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // 📂 Demander où enregistrer le fichier
        QString savePath = QFileDialog::getSaveFileName(this, "Enregistrer le fichier", fileName);
        if (!savePath.isEmpty()) {
            QFile file(savePath);
            if (file.open(QIODevice::WriteOnly)) {
                file.write(data);
                file.close();

                // ✅ Incrémente le compteur
                fileReceivedCount++;

                // ✅ Ajoute le fichier reçu et affiche le compteur
                fileListWidget->addItem(QString("📥 Reçu : %1 (%2 fichiers reçus)").arg(savePath).arg(fileReceivedCount));

                QMessageBox::information(this, "✅ Succès",
                                         QString("Le fichier a été enregistré avec succès !\nTotal fichiers reçus : %1").arg(fileReceivedCount));
            } else {
                QMessageBox::critical(this, "❌ Erreur", "Impossible d'enregistrer le fichier !");
            }
        }
    } else {
        qDebug() << "❌ L'utilisateur a refusé de sauvegarder le fichier.";
    }
}

