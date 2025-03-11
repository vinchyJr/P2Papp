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

    if (message.startsWith("FILE_NAME:")) {
        receivedFileName = message.mid(10);  // 🔹 Stocke le vrai nom du fichier reçu
        qDebug() << "📌 Nom du fichier reçu :" << receivedFileName;
    }
}

// ✅ Fonction qui gère la réception d'un fichier
void Peer::onBinaryMessageReceived(QByteArray data) {
    static QFile receivedFile;

    if (!receivedFile.isOpen()) {
        // 📌 Vérifie si on a reçu un nom de fichier avant
        QString fileName = receivedFileName.isEmpty() ? "received_file.dat" : receivedFileName;

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

    // 🔹 Ferme le fichier après le dernier chunk
    if (data.size() < 4096) {
        receivedFile.close();
        qDebug() << "✅ Fichier reçu et enregistré : " << receivedFile.fileName();

        // ✅ Émettre le signal pour mettre à jour l'interface
        emit fileReceived(receivedFile.fileName());
    }
}

// ✅ Fonction qui envoie un fichier (envoie aussi le nom)
void Peer::sendFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "❌ Erreur : Impossible d'ouvrir le fichier" << filePath;
        return;
    }

    QString fileName = QFileInfo(filePath).fileName();  // 🔹 Récupère le nom du fichier
    for (QWebSocket *peer : clients) {
        peer->sendTextMessage("FILE_NAME:" + fileName);  // 🔹 Envoie le nom du fichier
    }

    qDebug() << "📤 Envoi du fichier : " << fileName;

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
