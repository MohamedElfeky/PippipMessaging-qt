#include "mainwindow.h"
#include <QApplication>
#include <QCoreApplication>
#include <QString>

int main(int argc, char *argv[]) {

    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("seComm");
    QCoreApplication::setOrganizationDomain("pippip.io");
    QCoreApplication::setApplicationName("Pippip Messaging");

    MainWindow w;
    w.show();
    w.startFortuna();

    return a.exec();
}
