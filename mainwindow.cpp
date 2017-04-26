#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ParameterGenerator.h"
#include "NewAccountDialog.h"
#include "EntropyStream.h"
#include "UDPListener.h"
#include <QThread>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    session(0) {

    ui->setupUi(this);
    connect(ui->NewAccountAction, SIGNAL(triggered()), this, SLOT(newAccount()));
    connect(ui->LoginAction, SIGNAL(triggered()), this, SLOT(logIn()));

}

MainWindow::~MainWindow() {

    delete session;
    delete ui;

}
/*
void MainWindow::fortunaStreamError(QString error) {

    // EntropyStream doesn't emit errors at the moment.

}

void MainWindow::fortunaUDPError(QString errstr, bool fatal) {
   // TODO: Add message box and quit on fatal
}
*/
void MainWindow::logIn() {

}

void MainWindow::newAccount() {

    Pippip::ParameterGenerator *gen = new Pippip::ParameterGenerator;
    session = gen;
    NewAccountDialog dialog(gen);
    dialog.exec();

}

void MainWindow::startFortuna() {

    QThread *streamThread = new QThread;
    Pippip::EntropyStream *stream = new Pippip::EntropyStream;
    stream->moveToThread(streamThread);
    connect(stream, SIGNAL(error(QString)), this, SLOT(fortunaStreamError(QString)));
    connect(streamThread, SIGNAL(started()), stream, SLOT(threadFunction()));
    connect(stream, SIGNAL(finished()), streamThread, SLOT(quit()));
    streamThread->start();

    QThread *udpThread = new QThread;
    Pippip::UDPListener *udp = new Pippip::UDPListener(stream);
    udp->moveToThread(udpThread);
    connect(udp, SIGNAL(error(QString, bool)), this, SLOT(fortunaUDPError(QString, bool)));
    connect(udpThread, SIGNAL(started()), udp, SLOT(runListener()));
    connect(udp, SIGNAL(finished()), udpThread, SLOT(quit()));
    udpThread->start();

}
