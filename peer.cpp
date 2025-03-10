#include "peer.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QFileDialog>

Peer::Peer(quint16 port, QObject *parent)
    : QObject(parent), server(new QWebSocketServer("P2P Node", QWebSocketServer::NonSecureMode, this)) {

    if (server->listen(QHostAddress::Any, port)) {
        connect(server, &QWebSocketServer::newConnection, this, &Peer::onNewConnection);
        qDebug() << "✅ Serveur P2P démarré sur le port" << port;
    } else {
        qDebug() << "❌ Erreur : Impossible de démarrer le serveur P2P !";
    }
}

// ✅ Fonction qui gère la connexion d'un pair
void Peer::onNewConnection() {
    QWebSocket *client = server->nextPendingConnection();
    if (!client) {
        qDebug() << "⚠ Erreur : tentative de connexion d'un client NULL !";
        return;
    }

    connect(client, &QWebSocket::textMessageReceived, this, &Peer::onMessageReceived);
    connect(client, &QWebSocket::binaryMessageReceived, this, &Peer::onBinaryMessageReceived);
    connect(client, &QWebSocket::disconnected, this, &Peer::onPeerDisconnected);

    clients.append(client);
    qDebug() << "🔗 Nouveau pair connecté ! Total clients:" << clients.size();
}

// ✅ Fonction qui gère les messages reçus
void Peer::onMessageReceived(QString message) {
    qDebug() << "📩 Message P2P reçu :" << message;

    if (message.startsWith("REQUEST_FILE:")) {
        QString fileName = message.split(":")[1];
        sendFile(fileName);
    }
}

// ✅ Fonction qui gère la réception d'un fichier
void Peer::onBinaryMessageReceived(QByteArray data) {
    static QFile receivedFile;

    // 📌 Vérifie si c'est le début du transfert
    if (!receivedFile.isOpen()) {
        QString fileName = "received_file.dat";  // 🔹 Remplace par un vrai nom plus tard

        // 🔹 Demande à l'utilisateur où enregistrer le fichier
        QString savePath = QFileDialog::getSaveFileName(nullptr, "Enregistrer le fichier", fileName);
        if (savePath.isEmpty()) {
            qDebug() << "❌ Aucun emplacement choisi pour enregistrer le fichier.";
            return;
        }

        receivedFile.setFileName(savePath);
        if (!receivedFile.open(QIODevice::WriteOnly)) {
            qDebug() << "❌ Erreur : Impossible d'ouvrir le fichier pour écriture.";
            return;
        }

        qDebug() << "📥 Fichier en cours de réception : " << savePath;
    }

    // 🔹 Écrit le bloc reçu dans le fichier
    receivedFile.write(data);

    qDebug() << "📥 Chunk reçu (" << data.size() << " octets)";

    // 🔹 Ferme le fichier quand le transfert est terminé (ajoute une condition plus tard)
}

// ✅ Fonction qui envoie un fichier
void Peer::sendFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "❌ Erreur : Impossible d'ouvrir le fichier" << filePath;
        return;
    }

    qDebug() << "📤 Envoi du fichier en morceaux...";

    while (!file.atEnd()) {
        QByteArray chunk = file.read(4096);  // 🔹 Envoi en blocs de 4 Ko
        for (QWebSocket *peer : clients) {
            peer->sendBinaryMessage(chunk);
        }
    }

    file.close();
    qDebug() << "✅ Fichier envoyé avec succès !";
}

// ✅ Fonction qui gère la déconnexion d'un pair
void Peer::onPeerDisconnected() {
    QWebSocket *peer = qobject_cast<QWebSocket *>(sender());
    if (peer) {
        clients.removeAll(peer);
        peer->deleteLater();
        qDebug() << "🔴 Peer déconnecté ! Clients restants:" << clients.size();
    }
}
