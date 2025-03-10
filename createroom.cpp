#include "createroom.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <QWebSocket>

CreateRoom::CreateRoom(QWidget *parent) : QWidget(parent), socket(new QWebSocket()) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    roomNameInput = new QLineEdit(this);
    roomNameInput->setPlaceholderText("Entrez le nom de la salle");

    createButton = new QPushButton("Créer", this);

    layout->addWidget(roomNameInput);
    layout->addWidget(createButton);
    layout->setAlignment(Qt::AlignCenter);

    connect(createButton, &QPushButton::clicked, this, &CreateRoom::createRoom);
    connect(socket, &QWebSocket::connected, this, &CreateRoom::onConnected);
    connect(socket, &QWebSocket::textMessageReceived, this, &CreateRoom::onTextMessageReceived);
}

void CreateRoom::createRoom() {
    QString roomName = roomNameInput->text().trimmed();
    if (roomName.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Le nom de la salle ne peut pas être vide !");
        return;
    }

    qDebug() << "🔗 Connexion au serveur WebSocket...";
    socket->open(QUrl("ws://192.168.1.100:6699"));  // 🔹 Remplace par l'IP du serveur
}

void CreateRoom::onConnected() {
    QString roomName = roomNameInput->text().trimmed();
    if (!roomName.isEmpty()) {
        qDebug() << "📤 Envoi de la requête de création de salle...";
        socket->sendTextMessage("CREATE_ROOM:" + roomName);
    }
}

void CreateRoom::onTextMessageReceived(QString message) {
    qDebug() << "📩 Message reçu du serveur:" << message;
    if (message.startsWith("ROOM_CREATED:")) {
        QMessageBox::information(this, "Salle créée", "Salle créée avec succès !");
    } else {
        QMessageBox::critical(this, "Erreur", "Impossible de créer la salle.");
    }
}
