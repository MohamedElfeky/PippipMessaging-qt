#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Authenticator.h"
#include "Constants.h"
#include "ContactDirector.h"
#include "ContactsDialog.h"
#include "DatabaseException.h"
#include "EntropyStream.h"
#include "KeyFilter.h"
#include "LoginDialog.h"
#include "NewAccountDialog.h"
#include "NicknamesDialog.h"
#include "ParameterGenerator.h"
#include "StatusController.h"
#include "UDPListener.h"
#include "Vault.h"
#include <QComboBox>
#include <QDir>
#include <QLabel>
#include <QMessageBox>
#include <QSettings>
#include <QThread>

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent),
  ui(new Ui::MainWindow),
  createMessage(false),
  session(0),
  contactDirector(0) {

    ui->setupUi(this);
    setDefaults();
    restoreSettings();

    connect(ui->NewAccountAction, SIGNAL(triggered()), this, SLOT(newAccount()));
    connect(ui->LoginAction, SIGNAL(triggered()), this, SLOT(logIn()));
    connect(ui->LogoutAction, SIGNAL(triggered()), this, SLOT(logOut()));
    connect(ui->NicknamesAction, SIGNAL(triggered()), this, SLOT(manageNicknames()));
    connect(ui->ContactsAction, SIGNAL(triggered()), this, SLOT(manageContacts()));
    connect(ui->NewMessageAction, SIGNAL(triggered()), this, SLOT(newMessage()));

    QWidget *status = new QWidget(this);
    iconLabel = new QLabel(status);
    iconLabel->setTextFormat(Qt::RichText);
    iconLabel->setText(Constants::INFO_ICON);
    statusLabel = new QLabel(status);
    statusLabel->setTextFormat(Qt::RichText);
    statusLabel->setText("Log In");
    statusLabel->setAlignment(Qt::AlignVCenter);
    statusLabel->setIndent(5);
    statusBar()->addWidget(iconLabel);
    statusBar()->addWidget(statusLabel);
    StatusController::init(this);

    KeyFilter *keyFilter = new KeyFilter(ui->messageTextEdit);
    keyFilter->addKey(Qt::Key_Enter);
    keyFilter->addKey(Qt::Key_Return);
    ui->messageTextEdit->installEventFilter(keyFilter);
    connect(keyFilter, SIGNAL(keyPressed(Qt::Key)), this, SLOT(sendMessage(Qt::Key)));

}

MainWindow::~MainWindow() {

    delete session;
    delete ui;

}

void MainWindow::closeEvent(QCloseEvent *event) {

    saveSettings();
    QMainWindow::closeEvent(event);

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

    Pippip::Vault *vault = new Pippip::Vault;
    session = vault;
    LoginDialog *dialog = new LoginDialog(vault, this);
    updateStatus(Constants::INFO_ICON, "Logging in");
    dialog->exec();

}

void MainWindow::loggedIn(bool newAccount) {

    try {
        // Do this first in case the database open fails
        // messageManager = new Pippip::MessageManager(account, session, this);

        ui->LoginAction->setEnabled(false);
        ui->NewAccountAction->setEnabled(false);
        ui->LogoutAction->setEnabled(true);
        ui->ContactsAction->setEnabled(true);
        ui->NicknamesAction->setEnabled(true);
        ui->NewMessageAction->setEnabled(true);

        contactDirector = new Pippip::ContactDirector(session, this);
        if (newAccount) {
            contactDirector->createDatabase(session->accountName);
        }
        else {
            contactDirector->openDatabase(session->accountName);
        }
        contactDirector->start();

        /*
        Pippip::ContactsDatabase::initialize(session);
        Pippip::ContactsDatabase *contactsDatabase =
                                    Pippip::ContactsDatabase::open(session);
        Pippip::DatabaseContactList contacts;
        contactsDatabase->getContacts(contacts);
        contactsDatabase->close();
        session->contacts->load(contacts, session);
        */
        updateStatus(Constants::CHECK_ICON, "Authentication Complete");
    }
    //catch (Pippip::MessageException& e) {
    //    updateStatus(Constants::REDX_ICON, "Authentication failed - " + QString(e.what()));
    //}
    catch (Pippip::DatabaseException& e) {
        QMessageBox alert(QMessageBox::Critical, "Database Error",
                          "A database error occurred. Click OK to continue, Cancel to log out",
                          QMessageBox::Ok | QMessageBox::Cancel);
        alert.setDetailedText(e.what());
        if (alert.exec() == QDialog::Accepted) {
            updateStatus(Constants::REDX_ICON, "Database error - " + QString(e.what()));
        }
        else {
            logOut();
        }
    }

}

void MainWindow::logOut() {

    Pippip::Authenticator *auth = new Pippip::Authenticator(session, this);
    connect(auth, SIGNAL(loggedOut()), this, SLOT(loggedOut()));
    auth->logOut();
    //auth->deleteLater();

}

void MainWindow::loggedOut() {

    if (contactDirector != 0) {
        contactDirector->end();
        contactDirector->deleteLater();
        contactDirector = 0;
    }

    delete session;
    session = 0;
    //messageManager->deleteLater();
    //messageManager = 0;
    ui->LoginAction->setEnabled(true);
    ui->NewAccountAction->setEnabled(true);
    ui->LogoutAction->setEnabled(false);
    ui->NicknamesAction->setEnabled(false);
    ui->ContactsAction->setEnabled(false);
    ui->NewMessageAction->setEnabled(false);

    updateStatus(Constants::INFO_ICON, "Offline");

}

void MainWindow::manageContacts() {

    ContactsDialog *dialog = new ContactsDialog(session, this);
    dialog->setContactDirector(contactDirector);
    updateStatus(Constants::INFO_ICON, "Managing contacts");
    dialog->exec();
    updateStatus(Constants::CHECK_ICON, "Online");

}

void MainWindow::manageNicknames() {

    NicknamesDialog *dialog = new NicknamesDialog(session, this);
    updateStatus(Constants::INFO_ICON, "Managing nicknames");
    dialog->exec();
    updateStatus(Constants::CHECK_ICON, "Online");

}

void MainWindow::newAccount() {

    Pippip::ParameterGenerator *gen = new Pippip::ParameterGenerator;
    session = gen;
    NewAccountDialog *dialog = new NewAccountDialog(gen, this);
    dialog->exec();

}

void MainWindow::newMessage() {
/*
    createMessage = true;

    ui->messagesTable->clearContents();
    ui->messagesTable->setRowCount(1);
    ui->messagesTable->setColumnCount(5);

    QLabel *fromLabel = new QLabel("From: ");
    QComboBox *fromComboBox = new QComboBox;
    Pippip::Nicknames *nicknames = nicknameManager->getNicknames();
    QStringList nicknameItems;
    for (auto nickname : *nicknames) {
        nicknameItems << nickname.entity.nickname;
    }
    fromComboBox->addItems(nicknameItems);
    ui->messagesTable->setCellWidget(0, 0, fromLabel);
    ui->messagesTable->setCellWidget(0, 1, fromComboBox);

    QLabel *toLabel = new QLabel("To: ");
    QComboBox *toComboBox = new QComboBox;
    const Pippip::ContactList& contacts = contactManager->getContacts();
    QStringList contactItems;
    for (auto contact : contacts) {
        contactItems << contact.entity.nickname;
    }
    toComboBox->addItems(contactItems);
    ui->messagesTable->setCellWidget(0, 3, toLabel);
    ui->messagesTable->setCellWidget(0, 4, toComboBox);

    ui->messageTextEdit->setFocus();
*/
}

void MainWindow::nicknamesLoaded() {

    ui->NicknamesAction->setEnabled(true);
    ui->ContactsAction->setEnabled(true);
    ui->NewMessageAction->setEnabled(true);

}

void MainWindow::requestFailed(const QString &reqName, const QString &error) {

    statusLabel->setText(reqName + " failed: " + error);

}

void MainWindow::restoreSettings() {

    QSettings settings;
    //QSettings settings("Pippip.plist", QSettings::NativeFormat);
    QByteArray mainwindowState = settings.value("MainWindow/state").value<QByteArray>();
    restoreState(mainwindowState);
    QByteArray vsplitterState = settings.value("MainWindow/vsplitterState").value<QByteArray>();
    ui->vsplitter->restoreState(vsplitterState);
    QByteArray hsplitterState = settings.value("MainWindow/hsplitterState").value<QByteArray>();
    ui->hsplitter->restoreState(hsplitterState);

}

void MainWindow::saveSettings() {

    QSettings settings;
    settings.setValue("MainWindow.state", saveState());
    settings.setValue("MainWindow/vsplitterState", ui->hsplitter->saveState());
    settings.setValue("MainWindow/vsplitterState", ui->vsplitter->saveState());

}

void MainWindow::sendMessage(Qt::Key) {

    QString sender;
    QString recipient;
    QString message;
    if (createMessage) {
        QComboBox *fromComboBox = dynamic_cast<QComboBox*>(ui->messagesTable->cellWidget(0, 1));
        sender = fromComboBox->currentText();
        QComboBox *toComboBox = dynamic_cast<QComboBox*>(ui->messagesTable->cellWidget(0, 4));
        recipient = toComboBox->currentText();
        message = ui->messageTextEdit->toPlainText();
        createMessage = false;
        ui->messagesTable->clearContents();
        ui->messageTextEdit->clear();
    }
    //messageManager->sendMessage(sender, recipient, message);

}

void MainWindow::setDefaults() {

    QSettings settings;
    if (!settings.contains("FirstRun")) {
        settings.setValue("FirstRun", true);
        QString supportPath = QDir::homePath() + "/Library/Application Support/PippipMessaging";
        QDir().mkdir(supportPath);
        QString vaultPath = supportPath + "/Vaults";
        QDir().mkdir(vaultPath);
        settings.setValue("Defaults/vaultPath", vaultPath);
        QString dbPath = supportPath + "/Database";
        QDir().mkdir(dbPath);
        settings.setValue("Defaults/dbPath", dbPath);
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

void MainWindow::updateStatus(const QString& icon, const QString& status) {

    iconLabel->setText(icon);
    statusLabel->setText(status);
    statusBar()->show();

}
