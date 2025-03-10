#ifndef PEER_H  // Vérifie que le nom est bien UNIQUE
#define PEER_H

#include <QObject>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QFile>

class Peer : public QObject {
    Q_OBJECT
public:
    explicit Peer(quint16 port, QObject *parent = nullptr);

    void connectToPeer(const QString &ip, quint16 port);
    void sendFile(const QString &filePath);
    void requestFile(const QString &fileName);

private slots:
    void onNewConnection();
    void onMessageReceived(QString message);
    void onBinaryMessageReceived(QByteArray data);
    void onPeerDisconnected();

private:
    QWebSocketServer *server;
    QList<QWebSocket *> clients;
    QWebSocket *currentClient;
    QFile file;
};

#endif // PEER_H
