#ifndef PEER_H
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
signals:
    void fileReceived(QString fileName, QByteArray data);

  // ✅ Signal pour informer l'interface qu'un fichier a été reçu

private slots:
    void onNewConnection();
    void onMessageReceived(QString message);
    void onBinaryMessageReceived(QByteArray data);
    void onPeerDisconnected();

private:
    qint64 totalBytesReceived = 0;  // ✅ Compteur du nombre total d'octets reçus

    QWebSocketServer *server;
    QList<QWebSocket *> clients;
    QWebSocket *currentClient;
    QFile file;
    QString receivedFileName;  // ✅ Stocke le nom du fichier reçu
};

#endif // PEER_H
