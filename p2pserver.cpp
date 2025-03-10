#include "p2pserver.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

P2PServer::P2PServer(quint16 port, QObject *parent)
    : QObject(parent), server(new QWebSocketServer("P2P Server", QWebSocketServer::NonSecureMode, this)) {
    if (server->listen(QHostAddress::Any, port)) {
        connect(server, &QWebSocketServer::newConnection, this, &P2PServer::onNewConnection);
        qDebug() << "✅ P2P Server started on port" << port;
    } else {
        qDebug() << "❌ Failed to start P2P Server!";
    }
}

P2PServer::~P2PServer() {
    server->close();
    qDeleteAll(clients);
}

void P2PServer::onNewConnection() {
    QWebSocket *client = server->nextPendingConnection();
    connect(client, &QWebSocket::textMessageReceived, this, &P2PServer::onMessageReceived);
    connect(client, &QWebSocket::disconnected, this, &P2PServer::onSocketDisconnected);
    clients.append(client);
    qDebug() << "🔗 New peer connected! Total clients:" << clients.size();
}

void P2PServer::onMessageReceived(QString message) {
    qDebug() << "📩 Received message:" << message;

    // Si le message contient "GET_FILES", on envoie la liste des fichiers disponibles
    if (message == "GET_FILES") {
        sendFileList();
    }
}

void P2PServer::sendFileList() {
    QJsonObject json;
    json["files"] = QJsonArray::fromStringList(fileList);
    QJsonDocument doc(json);

    for (QWebSocket *client : clients) {
        client->sendTextMessage(doc.toJson());
        qDebug() << "📤 Sent file list to client:" << client;
    }
}

void P2PServer::onSocketDisconnected() {
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());
    if (client) {
        clients.removeAll(client);
        client->deleteLater();
        qDebug() << "🔴 Peer disconnected! Remaining clients:" << clients.size();
    }
}

// 🔹 Ajoute cette ligne à la fin pour éviter l'erreur "Q_OBJECT macro but no moc include"
#include "p2pserver.moc"
