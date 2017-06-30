#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ParameterGenerator.h"
#include "Vault.h"
#include "NewAccountDialog.h"
#include "NicknamesDialog.h"
#include "ContactsDialog.h"
#include "LoginDialog.h"
#include "NicknameManager.h"
#include "ContactManager.h"
#include "Authenticator.h"
#include "EntropyStream.h"
#include "UDPListener.h"
#include <CryptoKitty-C/keys/RSAPrivateKey.h>
#include <CryptoKitty-C/keys/RSAPublicKey.h>
#include <QThread>
#include <QLabel>
#include <QKeySequence>

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent),
  ui(new Ui::MainWindow),
  session(0),
  nicknameManager(0),
  contactManager(0) {

    ui->setupUi(this);
    connect(ui->NewAccountAction, SIGNAL(triggered()), this, SLOT(newAccount()));
    connect(ui->LoginAction, SIGNAL(triggered()), this, SLOT(logIn()));
    connect(ui->LogoutAction, SIGNAL(triggered()), this, SLOT(logOut()));
    connect(ui->NicknamesAction, SIGNAL(triggered()), this, SLOT(manageNicknames()));
    connect(ui->ContactsAction, SIGNAL(triggered()), this, SLOT(manageContacts()));

    statusLabel = new QLabel(this);
    statusBar()->addWidget(statusLabel);

#if defined (Q_OS_MACX)
    ui->LoginAction->setShortcut(QKeySequence(Qt::ControlModifier + Qt::Key_L));
#endif

}

MainWindow::~MainWindow() {

    delete contactManager;
    delete nicknameManager;
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

}

void MainWindow::loggedIn() {

    ui->LoginAction->setEnabled(false);
    ui->NewAccountAction->setEnabled(false);
    ui->LogoutAction->setEnabled(true);
    ui->NicknamesAction->setEnabled(true);
    ui->ContactsAction->setEnabled(true);
    nicknameManager = new Pippip::NicknameManager(this, session);
    contactManager = new Pippip::ContactManager(this, session);
    updateStatus("Authentication Complete");

}

void MainWindow::logOut() {

    Pippip::Authenticator *auth = new Pippip::Authenticator(session, this);
    connect(auth, SIGNAL(loggedOut()), this, SLOT(loggedOut()));
    auth->logOut();
    //auth->deleteLater();

}

void MainWindow::loggedOut() {

    delete session;
    session = 0;
    delete nicknameManager;
    nicknameManager = 0;
    delete contactManager;
    contactManager = 0;
    ui->LoginAction->setEnabled(true);
    ui->NewAccountAction->setEnabled(true);
    ui->LogoutAction->setEnabled(false);
    ui->NicknamesAction->setEnabled(false);
    ui->ContactsAction->setEnabled(false);
    updateStatus("Logged out");

}

void MainWindow::manageContacts() {

    ContactsDialog *dialog = new ContactsDialog(session, this);
    dialog->setContactManager(contactManager);
    dialog->setNicknameManager(nicknameManager);
    dialog->exec();

}

void MainWindow::manageNicknames() {

    NicknamesDialog *dialog = new NicknamesDialog(session, this);
    dialog->setManager(nicknameManager);
    dialog->exec();

}

void MainWindow::newAccount() {

    Pippip::ParameterGenerator *gen = new Pippip::ParameterGenerator;
    session = gen;
    NewAccountDialog *dialog = new NewAccountDialog(gen, this);
    dialog->exec();

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
