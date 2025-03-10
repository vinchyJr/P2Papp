#include "peer.h"
#include <QApplication>
#include <QDebug>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    qDebug() << "🚀 Application P2P démarrée !";

    // Démarrer un peer sur un port spécifique (exemple : 7777)
    Peer myPeer(7777);

    // Afficher la fenêtre principale
    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
