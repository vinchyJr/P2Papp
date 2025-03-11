#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QListWidget>  // ✅ Ajout pour afficher les fichiers transférés
#include <QPushButton>  // ✅ Ajout pour le bouton d'envoi de fichier
#include "createroom.h"
#include "joinroom.h"
#include "peer.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSendFileClicked();  // 📤 Bouton pour envoyer un fichier
    void onFileReceived(QString fileName);  // 📥 Ajout d'un fichier reçu à la liste

private:
    QTabWidget *tabWidget;
    CreateRoom *createRoomPage;
    JoinRoom *joinRoomPage;
    Peer *peer;
    QListWidget *fileListWidget;  // ✅ Liste des fichiers envoyés/reçus
    QPushButton *sendFileButton;  // ✅ Bouton d'envoi de fichier
};

#endif // MAINWINDOW_H
