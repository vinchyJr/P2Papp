#ifndef P2PSERVER_H
#define P2PSERVER_H

#include <QObject>
#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>
#include <QStringList>

class P2PServer : public QObject {
    Q_OBJECT

public:
    explicit P2PServer(quint16 port = 6699, QObject *parent = nullptr);
    ~P2PServer();

    void addFile(const QString &fileName);
    void sendFileList();

private slots:
    void onNewConnection();
    void onMessageReceived(QString message);
    void onSocketDisconnected();

private:
    QWebSocketServer *server;
    QList<QWebSocket *> clients;
    QStringList fileList;
};

#endif // P2PSERVER_H
