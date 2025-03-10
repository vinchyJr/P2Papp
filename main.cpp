#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>
#include "p2pserver.h"
#include "mainwindow.h"  // Ajout de MainWindow

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 🔹 Créer une fenêtre temporaire pour démarrer le serveur
    QMainWindow startWindow;
    startWindow.setWindowTitle("P2P App - Démarrage");
    startWindow.resize(400, 300);

    // 🔹 Création du bouton "Démarrer le serveur"
    QWidget *centralWidget = new QWidget(&startWindow);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    QPushButton *startButton = new QPushButton("Démarrer le serveur", centralWidget);
    layout->addWidget(startButton);
    centralWidget->setLayout(layout);
    startWindow.setCentralWidget(centralWidget);

    // 🔹 Déclaration du serveur sans le démarrer immédiatement
    P2PServer *server = nullptr;

    // 🔹 Lancer le serveur et ouvrir `MainWindow` après le clic sur le bouton
    QObject::connect(startButton, &QPushButton::clicked, [&]() {
        if (!server) {
            server = new P2PServer(6699);
            qDebug() << "✅ Serveur P2P démarré sur le port 6699";

            // 🔹 Fermer la fenêtre de démarrage
            startWindow.close();

            // 🔹 Ouvrir l'interface principale (`MainWindow`)
            MainWindow *mainWindow = new MainWindow();
            mainWindow->show();
        }
    });

    startWindow.show();
    return app.exec();
}
