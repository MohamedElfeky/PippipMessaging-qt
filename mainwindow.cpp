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
#include "EnterKeyFilter.h"
#include "MessageManager.h"
#include "MessageException.h"
//#include "MessageDatabase.h"
#include <CryptoKitty-C/keys/RSAPrivateKey.h>
#include <CryptoKitty-C/keys/RSAPublicKey.h>
#include <QThread>
#include <QLabel>
#include <QKeySequence>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QSettings>
#include <QByteArray>
#include <QHBoxLayout>
#include <QComboBox>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QKeySequence>

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent),
  ui(new Ui::MainWindow),
  createMessage(false),
  session(0),
  nicknameManager(0),
  contactManager(0) {

    ui->setupUi(this);
    setDefaults();
    restoreSettings();

    connect(ui->NewAccountAction, SIGNAL(triggered()), this, SLOT(newAccount()));
    //connect(loginAction, SIGNAL(toggled()), this, SLOT(logIn()));
    connect(ui->LoginAction, SIGNAL(triggered()), this, SLOT(logIn()));
    connect(ui->LogoutAction, SIGNAL(triggered()), this, SLOT(logOut()));
    connect(ui->NicknamesAction, SIGNAL(triggered()), this, SLOT(manageNicknames()));
    connect(ui->ContactsAction, SIGNAL(triggered()), this, SLOT(manageContacts()));
    connect(ui->NewMessageAction, SIGNAL(triggered()), this, SLOT(newMessage()));

    statusLabel = new QLabel("Log in", this);
    statusLabel->setMargin(3);
    statusLabel->setAlignment(Qt::AlignLeft);
    statusLabel->setIndent(10);
    statusBar()->addWidget(statusLabel);

    EnterKeyFilter *keyFilter = new EnterKeyFilter(ui->messageTextEdit);
    ui->messageTextEdit->installEventFilter(keyFilter);
    connect(keyFilter, SIGNAL(enterPressed()), this, SLOT(sendMessage()));

}

MainWindow::~MainWindow() {

    delete session;
    delete ui;

}

void MainWindow::closeEvent(QCloseEvent *event) {

    saveSettings();
    QMainWindow::closeEvent(event);

}

void MainWindow::contactsLoaded() {

    nicknameManager->loadNicknames();

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
    dialog->exec();

}

void MainWindow::loggedIn(const QString& account) {

    try {
        // Do this first in case the database open fails
        messageManager = new Pippip::MessageManager(account, session, this);

        ui->LoginAction->setEnabled(false);
        ui->NewAccountAction->setEnabled(false);
        ui->LogoutAction->setEnabled(true);
        nicknameManager = new Pippip::NicknameManager(this, session);
        connect(nicknameManager, SIGNAL(nicknamesLoaded()), this, SLOT(nicknamesLoaded()));
        connect(nicknameManager, SIGNAL(requestFailed(QString,QString)),
                this, SLOT(requestFailed(QString,QString)));
        contactManager = new Pippip::ContactManager(this, session);
        connect(contactManager, SIGNAL(contactsLoaded()), this, SLOT(contactsLoaded()));
        connect(contactManager, SIGNAL(requestFailed(QString,QString)),
                this, SLOT(requestFailed(QString,QString)));
        contactManager->loadContacts();

        updateStatus("Authentication Complete");
    }
    catch (Pippip::MessageException& e) {
        updateStatus("Authentication failed - " + QString(e.what()));
    }

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
    nicknameManager->deleteLater();
    nicknameManager = 0;
    contactManager->deleteLater();
    contactManager = 0;
    messageManager->deleteLater();
    messageManager = 0;
    ui->LoginAction->setEnabled(true);
    ui->NewAccountAction->setEnabled(true);
    ui->LogoutAction->setEnabled(false);
    ui->NicknamesAction->setEnabled(false);
    ui->ContactsAction->setEnabled(false);
    ui->NewMessageAction->setEnabled(false);
    updateStatus("Logged out");

}

void MainWindow::manageContacts() {

    statusLabel->clear();
    ContactsDialog *dialog = new ContactsDialog(session, this);
    disconnect(contactManager, SIGNAL(requestFailed(QString,QString)),
                                        this, SLOT(requestFailed(QString,QString)));
    connect(contactManager, SIGNAL(requestFailed(QString,QString)),
                                                dialog, SLOT(requestFailed(QString,QString)));
    dialog->setContactManager(contactManager);
    dialog->setNicknameManager(nicknameManager);
    dialog->exec();
    connect(contactManager, SIGNAL(requestFailed(QString,QString)),
                                                this, SLOT(requestFailed(QString,QString)));

}

void MainWindow::manageNicknames() {

    NicknamesDialog *dialog = new NicknamesDialog(this);
    disconnect(nicknameManager, SIGNAL(requestFailed(QString,QString)),
                                        this, SLOT(requestFailed(QString,QString)));
    connect(nicknameManager, SIGNAL(requestFailed(QString,QString)),
                                        dialog, SLOT(requestFailed(QString,QString)));
    dialog->setManager(nicknameManager);
    dialog->exec();
    connect(nicknameManager, SIGNAL(requestFailed(QString,QString)),
                                        dialog, SLOT(requestFailed(QString,QString)));

}

void MainWindow::newAccount() {

    Pippip::ParameterGenerator *gen = new Pippip::ParameterGenerator;
    session = gen;
    NewAccountDialog *dialog = new NewAccountDialog(gen, this);
    dialog->exec();

}

void MainWindow::newMessage() {

    createMessage = true;

    ui->messagesTable->clearContents();
    ui->messagesTable->setRowCount(1);
    ui->messagesTable->setColumnCount(5);

    QLabel *fromLabel = new QLabel("From: ");
    QComboBox *fromComboBox = new QComboBox;
    const Pippip::NicknameList& nicknames = nicknameManager->getNicknames();
    QStringList nicknameItems;
    for (auto nickname : nicknames) {
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

void MainWindow::sendMessage() {

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
    messageManager->sendMessage(sender, recipient, message);

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

void MainWindow::updateStatus(QString status) {

    statusLabel->setText(status);
    statusBar()->show();

}
