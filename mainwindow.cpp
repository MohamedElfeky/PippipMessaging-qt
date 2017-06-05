#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ParameterGenerator.h"
#include "Vault.h"
#include "NewAccountDialog.h"
#include "NicknamesDialog.h"
#include "LoginDialog.h"
#include "NicknameManager.h"
#include "Authenticator.h"
#include "EntropyStream.h"
#include "UDPListener.h"
#include <CryptoKitty-C/keys/RSAPrivateKey.h>
#include <CryptoKitty-C/keys/RSAPublicKey.h>
#include <QThread>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    session(0) {

    ui->setupUi(this);
    connect(ui->NewAccountAction, SIGNAL(triggered()), this, SLOT(newAccount()));
    connect(ui->LoginAction, SIGNAL(triggered()), this, SLOT(logIn()));
    connect(ui->LogoutAction, SIGNAL(triggered()), this, SLOT(logOut()));
    connect(ui->NicknamesAction, SIGNAL(triggered()), this, SLOT(manageNicknames()));

    statusLabel = new QLabel(this);
    statusBar()->addWidget(statusLabel);

    //session = new Pippip::SessionState; // Debugging only!
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

    Pippip::Vault *gen = new Pippip::Vault;
    session = gen;
    LoginDialog *dialog = new LoginDialog(gen, this);
    dialog->exec();
    if (session->sessionState == Pippip::SessionState::authenticated) {
        ui->LoginAction->setEnabled(false);
        ui->LogoutAction->setEnabled(true);
        ui->NicknamesAction->setEnabled(true);
    }

}

void MainWindow::logOut() {

    Pippip::Authenticator *auth = new Pippip::Authenticator(session, this);
    connect(auth, SIGNAL(loggedOut()), this, SLOT(loggedOut()));
    auth->logOut();
    //auth->deleteLater();

}

void MainWindow::loggedOut() {

    delete session->serverPublicKey;
    delete session->userPrivateKey;
    delete session->userPublicKey;
    delete session;
    ui->LoginAction->setEnabled(true);
    ui->LogoutAction->setEnabled(false);

}

void MainWindow::manageNicknames() {

    NicknamesDialog *dialog = new NicknamesDialog(session, this);
    Pippip::NicknameManager *manager = new Pippip::NicknameManager(this, session);
    dialog->setManager(manager);
    dialog->exec();
    bool hasNicknames = manager->getNicknames().size() > 0;
    ui->CancelAction->setEnabled(hasNicknames);
    ui->DeleteAction->setEnabled(hasNicknames);
    ui->RequestAction->setEnabled(hasNicknames);

}

void MainWindow::newAccount() {

    Pippip::ParameterGenerator *gen = new Pippip::ParameterGenerator;
    session = gen;
    NewAccountDialog dialog(gen);
    dialog.exec();
    if (session->sessionState == Pippip::SessionState::authenticated) {
        ui->LoginAction->setEnabled(false);
        ui->NicknamesAction->setEnabled(true);
    }

}

void MainWindow::startFortuna() {

    QThread *streamThread = new QThread;
    Pippip::EntropyStream *stream = new Pippip::EntropyStream;
    stream->moveToThread(streamThread);
    //connect(stream, SIGNAL(error(QString)), this, SLOT(fortunaStreamError(QString)));
    connect(streamThread, SIGNAL(started()), stream, SLOT(threadFunction()));
    connect(stream, SIGNAL(finished()), streamThread, SLOT(quit()));
    streamThread->start();

    QThread *udpThread = new QThread;
    Pippip::UDPListener *udp = new Pippip::UDPListener(stream);
    udp->moveToThread(udpThread);
    //connect(udp, SIGNAL(error(QString, bool)), this, SLOT(fortunaUDPError(QString, bool)));
    connect(udpThread, SIGNAL(started()), udp, SLOT(runListener()));
    connect(udp, SIGNAL(finished()), udpThread, SLOT(quit()));
    udpThread->start();

}

void MainWindow::updateStatus(QString status) {

    statusLabel->setText(status);
    statusBar()->show();

}
