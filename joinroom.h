#ifndef JOINROOM_H
#define JOINROOM_H

#include <QWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDropEvent>
#include <QMimeData>
#include <QMap>

class JoinRoom : public QWidget {
    Q_OBJECT

public:
    explicit JoinRoom(QWidget *parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void joinSelectedRoom();
    void filterRooms();

private:
    QListWidget *roomList;
    QLineEdit *searchInput;
    QListWidget *fileList;
    QMap<QString, QStringList> roomFiles; // Stocke les fichiers pour chaque salle
};

#endif // JOINROOM_H
