#include "createroom.h"
#include <QMessageBox>

CreateRoom::CreateRoom(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    roomNameInput = new QLineEdit(this);
    roomNameInput->setPlaceholderText("Entrez le nom de la salle");

    createButton = new QPushButton("Créer", this);

    layout->addWidget(roomNameInput);
    layout->addWidget(createButton);
    layout->setAlignment(Qt::AlignCenter);

    connect(createButton, &QPushButton::clicked, this, &CreateRoom::createRoom);
}

void CreateRoom::createRoom() {
    QString roomName = roomNameInput->text();
    if (!roomName.isEmpty()) {
        QMessageBox::information(this, "Salle créée", "Salle " + roomName + " créée !");
    }
}
