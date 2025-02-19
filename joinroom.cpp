#include "joinroom.h"
#include <QMessageBox>
#include <QFileInfo>
#include <QUrl>

JoinRoom::JoinRoom(QWidget *parent) : QWidget(parent) {
    setAcceptDrops(true);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);

    QVBoxLayout *leftLayout = new QVBoxLayout();
    searchInput = new QLineEdit(this);
    searchInput->setPlaceholderText("Rechercher une salle...");

    roomList = new QListWidget(this);
    roomList->addItem("Salle 1");
    roomList->addItem("Salle 2");
    roomList->addItem("Salle 3");

    leftLayout->addWidget(searchInput);
    leftLayout->addWidget(roomList);

    fileList = new QListWidget(this);

    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addWidget(fileList, 2);

    connect(searchInput, &QLineEdit::textChanged, this, &JoinRoom::filterRooms);
    connect(roomList, &QListWidget::itemClicked, this, &JoinRoom::joinSelectedRoom);
}

void JoinRoom::joinSelectedRoom() {
    QString selectedRoom = roomList->currentItem()->text();
    QMessageBox::information(this, "Connexion", "Vous avez rejoint la salle : " + selectedRoom);

    fileList->clear();
    if (roomFiles.contains(selectedRoom)) {
        for (const QString &file : roomFiles[selectedRoom]) {
            fileList->addItem(file);
        }
    }
}

void JoinRoom::filterRooms() {
    for (int i = 0; i < roomList->count(); i++) {
        QListWidgetItem *item = roomList->item(i);
        item->setHidden(!item->text().contains(searchInput->text(), Qt::CaseInsensitive));
    }
}

void JoinRoom::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void JoinRoom::dropEvent(QDropEvent *event) {
    QString selectedRoom = roomList->currentItem()->text();

    for (const QUrl &url : event->mimeData()->urls()) {
        QFileInfo fileInfo(url.toLocalFile());
        roomFiles[selectedRoom].append(fileInfo.fileName());
        fileList->addItem(fileInfo.fileName());
    }
    event->acceptProposedAction();
}
