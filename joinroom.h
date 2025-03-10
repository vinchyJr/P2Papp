#ifndef JOINROOM_H
#define JOINROOM_H

#include <QWidget>
#include <QWebSocket>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>

class JoinRoom : public QWidget {
    Q_OBJECT
public:
    explicit JoinRoom(QWidget *parent = nullptr);  // 🔹 Vérifie que cette ligne est bien là !
    void joinRoom();

private slots:
    void onConnected();
    void onTextMessageReceived(QString message);

private:
    QWebSocket *socket;
    QLineEdit *roomNameInput;
    QListWidget *roomList;
    QPushButton *joinButton;
};

#endif // JOINROOM_H
