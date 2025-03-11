#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QListWidget>
#include <QPushButton>
#include "createroom.h"
#include "joinroom.h"
#include "peer.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSendFileClicked();
    void onFileReceived(QString fileName, QByteArray data);  // ✅ Nouveau slot pour la réception des fichiers

private:
    int fileReceivedCount = 0;
    QTabWidget *tabWidget;
    CreateRoom *createRoomPage;
    JoinRoom *joinRoomPage;
    Peer *peer;

    QListWidget *fileListWidget;  // ✅ Liste des fichiers envoyés/reçus
    QPushButton *sendFileButton;  // ✅ Bouton pour envoyer des fichiers
};

#endif // MAINWINDOW_H
