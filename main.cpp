#include "mainwindow.h"
#include <QApplication>
#include <QString>

int main(int argc, char *argv[]) {

    QApplication a(argc, argv);

    MainWindow w;
    w.startFortuna();
    w.show();

    return a.exec();
}
