#include "MainWindow/MainWindow.h"
#include <QApplication>
#include <QLabel>

int main(int argc, char* argv[]) {

    QApplication app(argc, argv);

    MainWindow *mainwindow = new MainWindow;
    mainwindow->show();

    return app.exec();
}
