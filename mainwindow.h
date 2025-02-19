#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include "createroom.h"
#include "joinroom.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QTabWidget *tabWidget;
    CreateRoom *createRoomPage;
    JoinRoom *joinRoomPage;
};

#endif // MAINWINDOW_H
