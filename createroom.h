#ifndef CREATEROOM_H
#define CREATEROOM_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>

class CreateRoom : public QWidget {
    Q_OBJECT

public:
    explicit CreateRoom(QWidget *parent = nullptr);

private slots:
    void createRoom();

private:
    QLineEdit *roomNameInput;
    QPushButton *createButton;
};

#endif // CREATEROOM_H
