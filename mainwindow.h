#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
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

private:
    QTabWidget *tabWidget;
    CreateRoom *createRoomPage;
    JoinRoom *joinRoomPage;
    Peer *peer;
};

#endif // MAINWINDOW_H
