#include "peer.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

Peer::Peer(quint16 port, QObject *parent)
    : QObject(parent), server(new QWebSocketServer("P2P Node", QWebSocketServer::NonSecureMode, this)) {

    if (server->listen(QHostAddress::Any, port)) {
        connect(server, &QWebSocketServer::newConnection, this, &Peer::onNewConnection);
        qDebug() << "✅ Serveur P2P démarré sur le port" << port;
    } else {
        qDebug() << "❌ Erreur : Impossible de démarrer le serveur P2P !";
    }
}

void Peer::connectToPeer(const QString &ip, quint16 port) {
    if (currentClient) {
        qDebug() << "⚠ Déjà connecté à un pair.";
        return;
    }

    currentClient = new QWebSocket();
    connect(currentClient, &QWebSocket::connected, [=]() {
        qDebug() << "🔗 Connecté à" << ip << ":" << port;
    });
    connect(currentClient, &QWebSocket::disconnected, this, &Peer::onPeerDisconnected);
    connect(currentClient, &QWebSocket::binaryMessageReceived, this, &Peer::onBinaryMessageReceived);

    qDebug() << "🔌 Connexion en cours vers" << ip << ":" << port;
    currentClient->open(QUrl(QString("ws://%1:%2").arg(ip).arg(port)));
}

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

void Peer::onMessageReceived(QString message) {
    qDebug() << "📩 Message P2P reçu :" << message;

    if (message.startsWith("REQUEST_FILE:")) {
        QString fileName = message.split(":")[1];
        sendFile(fileName);
    }
}

void Peer::onBinaryMessageReceived(QByteArray data) {
    if (!file.isOpen()) {
        file.setFileName("received_file.dat");
        file.open(QIODevice::WriteOnly);
    }
    file.write(data);
    qDebug() << "📥 Chunk de fichier reçu (" << data.size() << " octets)";
}

void Peer::sendFile(const QString &filePath) {
    file.setFileName(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "❌ Erreur : Impossible d'ouvrir le fichier";
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
    qDebug() << "✅ Fichier envoyé !";
}

void Peer::requestFile(const QString &fileName) {
    if (clients.isEmpty()) {
        qDebug() << "⚠ Aucun pair connecté pour demander un fichier.";
        return;
    }

    for (QWebSocket *peer : clients) {
        peer->sendTextMessage("REQUEST_FILE:" + fileName);
    }
    qDebug() << "📤 Demande du fichier envoyée aux pairs !";
}

void Peer::onPeerDisconnected() {
    QWebSocket *peer = qobject_cast<QWebSocket *>(sender());
    if (peer) {
        clients.removeAll(peer);
        peer->deleteLater();
        qDebug() << "🔴 Peer déconnecté ! Clients restants:" << clients.size();
    }
}
