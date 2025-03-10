#ifndef CREATEROOM_H
#define CREATEROOM_H

#include <QWidget>
#include <QWebSocket>

class QLineEdit;
class QPushButton;

class CreateRoom : public QWidget {
    Q_OBJECT
public:
    explicit CreateRoom(QWidget *parent = nullptr);

private slots:
    void createRoom();
    void onConnected();
    void onTextMessageReceived(QString message);

private:
    QLineEdit *roomNameInput;
    QPushButton *createButton;
    QWebSocket *socket;
};

#endif // CREATEROOM_H
