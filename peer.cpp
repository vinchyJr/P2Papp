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
        receivedFileName = message.mid(10);  // 🔹 Stocke le vrai nom du fichier reçu
        qDebug() << "📌 Nom du fichier reçu :" << receivedFileName;
    }
}

// ✅ Gère la réception d'un fichier avec demande de confirmation
void Peer::onBinaryMessageReceived(QByteArray data) {
    static QFile receivedFile;
    static bool receivingFile = false;

    if (!receivingFile) {
        receivingFile = true;

        // 🔹 Demande de confirmation à l'utilisateur
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(nullptr, "📥 Réception de fichier",
                                      "Un fichier est en cours de réception. Voulez-vous l'accepter ?",
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No) {
            qDebug() << "❌ L'utilisateur a refusé le fichier.";
            receivingFile = false;
            return;
        }

        // 🔹 Demande où enregistrer le fichier
        QString savePath = QFileDialog::getSaveFileName(nullptr, "Enregistrer le fichier", receivedFileName);
        if (savePath.isEmpty()) {
            qDebug() << "❌ Aucun emplacement sélectionné. Annulation.";
            receivingFile = false;
            return;
        }

        receivedFile.setFileName(savePath);
        if (!receivedFile.open(QIODevice::WriteOnly)) {
            qDebug() << "❌ Impossible d'ouvrir le fichier pour écriture.";
            receivingFile = false;
            return;
        }

        qDebug() << "📥 Fichier accepté et en cours de réception : " << savePath;
    }

    // 🔹 Écrit les données dans le fichier
    receivedFile.write(data);
    qDebug() << "📥 Chunk reçu (" << data.size() << " octets)";

    // 🔹 Ferme le fichier après la fin du transfert
    if (data.size() < 4096) {
        receivedFile.close();
        receivingFile = false;
        qDebug() << "✅ Fichier reçu et enregistré : " << receivedFile.fileName();

        // ✅ Informe l'interface graphique qu'un fichier a été reçu
        emit fileReceived(receivedFile.fileName());
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
