#include "joinroom.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDebug>

JoinRoom::JoinRoom(QWidget *parent) : QWidget(parent), socket(new QWebSocket()) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    roomNameInput = new QLineEdit(this);
    roomNameInput->setPlaceholderText("Entrez l'IP de l'hôte (ex: 192.168.1.10)");

    roomList = new QListWidget(this);
    joinButton = new QPushButton("Rejoindre", this);

    layout->addWidget(roomNameInput);
    layout->addWidget(roomList);
    layout->addWidget(joinButton);
    layout->setAlignment(Qt::AlignCenter);

    connect(joinButton, &QPushButton::clicked, this, &JoinRoom::joinRoom);
    connect(socket, &QWebSocket::connected, this, &JoinRoom::onConnected);
    connect(socket, &QWebSocket::textMessageReceived, this, &JoinRoom::onTextMessageReceived);
}

void JoinRoom::joinRoom() {
    QString hostIP = roomNameInput->text().trimmed();
    if (hostIP.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer l'IP de l'hôte !");
        return;
    }

    QString wsUrl = QString("ws://%1:7777").arg(hostIP);
    qDebug() << "🔗 Connexion directe à la salle P2P:" << wsUrl;
    socket->open(QUrl(wsUrl));
}

void JoinRoom::onConnected() {
    qDebug() << "🔗 Connexion WebSocket réussie !";
}

void JoinRoom::onTextMessageReceived(QString message) {
    qDebug() << "📩 Message reçu :" << message;
}
