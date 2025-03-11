#include "peer.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

Peer::Peer(quint16 port, QObject *parent)
    : QObject(parent), server(new QWebSocketServer("P2P Node", QWebSocketServer::NonSecureMode, this)) {

    if (server->listen(QHostAddress::Any, port)) {
        connect(server, &QWebSocketServer::newConnection, this, &Peer::onNewConnection);
        qDebug() << "✅ Serveur P2P démarré sur le port" << port;
    } else {
        qDebug() << "❌ Erreur : Impossible de démarrer le serveur P2P !";
        qDebug() << "🔍 Détails de l'erreur : " << server->errorString();
    }

}

// ✅ Connexion à un pair via WebSocket
void Peer::connectToPeer(const QString &ip, quint16 port) {
    if (currentClient) {
        qDebug() << "⚠ Déjà connecté à un pair.";
        return;
    }

    currentClient = new QWebSocket();
    connect(currentClient, &QWebSocket::connected, [=]() {
        qDebug() << "🔗 Connecté à " << ip << ":" << port;
    });
    connect(currentClient, &QWebSocket::disconnected, this, &Peer::onPeerDisconnected);
    connect(currentClient, &QWebSocket::binaryMessageReceived, this, &Peer::onBinaryMessageReceived);

    qDebug() << "🔌 Connexion en cours vers " << ip << ":" << port;
    currentClient->open(QUrl(QString("ws://%1:%2").arg(ip).arg(port)));
}

// ✅ Gère l'arrivée d'un nouveau client
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

// ✅ Gère la réception d'un message
void Peer::onMessageReceived(QString message) {
    qDebug() << "📩 Message P2P reçu :" << message;

    if (message.startsWith("FILE_NAME:")) {
        receivedFileName = message.mid(10).trimmed();  // 🔹 Stocke le vrai nom du fichier reçu et enlève les espaces
        qDebug() << "📌 Nom du fichier reçu enregistré :" << receivedFileName;
    }
}

// ✅ Gère la réception d'un fichier avec demande de confirmation
void Peer::onBinaryMessageReceived(QByteArray data) {
    totalBytesReceived += data.size();  // ✅ Incrémente le compteur d'octets reçus
    qDebug() << "📥 Chunk reçu (" << data.size() << " octets). Total reçu :" << totalBytesReceived << " octets";

    if (!file.isOpen()) {
        // ✅ Vérifie si un nom de fichier a été reçu
        QString fileName = receivedFileName.isEmpty() ? "received_file.dat" : receivedFileName;

        // ✅ Demande à l'utilisateur où enregistrer le fichier
        QString savePath = QFileDialog::getSaveFileName(nullptr, "📥 Enregistrer le fichier", fileName);
        if (savePath.isEmpty()) {
            qDebug() << "❌ Aucun emplacement choisi, annulation.";
            return;
        }

        file.setFileName(savePath);
        if (!file.open(QIODevice::WriteOnly)) {
            qDebug() << "❌ Erreur : Impossible d'ouvrir le fichier pour écriture.";
            return;
        }

        qDebug() << "📥 Début du transfert du fichier : " << savePath;
    }

    file.write(data);
    qDebug() << "📥 Chunk reçu (" << data.size() << " octets) | Total reçu :" << totalBytesReceived << " octets";

    // ✅ Fermeture du fichier après la réception complète
    if (file.size() > 0) {
        qDebug() << "✅ Fichier reçu avec succès ! Enregistré sous :" << file.fileName();
        file.close();

        // ✅ Affichage d'un message de confirmation à l'utilisateur
        QMessageBox::information(nullptr, "✅ Réception terminée",
                                 QString("Le fichier '%1' a été reçu avec succès !\nTaille : %2 octets")
                                     .arg(file.fileName())
                                     .arg(totalBytesReceived));
    }
}


// ✅ Envoi d'un fichier à un pair
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
        qDebug() << chunk;
    }

    file.close();
    qDebug() << "✅ Fichier envoyé avec succès !";
}

// ✅ Gère la déconnexion d'un pair
void Peer::onPeerDisconnected() {
    QWebSocket *peer = qobject_cast<QWebSocket *>(sender());
    if (peer) {
        clients.removeAll(peer);
        peer->deleteLater();
        qDebug() << "🔴 Peer déconnecté ! Clients restants:" << clients.size();
    }
}
