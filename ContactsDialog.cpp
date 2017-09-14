#include "ContactsDialog.h"
#include "ContactRequest.h"
#include "ContactHandler.h"
#include "SQLLiteContactsDatabaseImpl.h"
#include "RequestHandler.h"
#include "Nicknames.h"
#include "LoadNicknamesTask.h"
#include "ui_ContactsDialog.h"
#include "SessionState.h"
#include "Constants.h"
#include <QStringList>
#include <QTableWidgetItem>

ContactsDialog::ContactsDialog(Pippip::SessionState *st, QWidget *parent)
: QDialog(parent),
  ui(new Ui::ContactsDialog),
  state(st) {

    ui->setupUi(this);
    contactHandler = new ContactHandler(ui, state, this);
    requestHandler = new RequestHandler(ui, state, this);

    // Set up request column headings.
    QStringList headings;
    headings << "Requested ID" << "Requesting Nickname" << "Requesting Public ID";
    ui->requestsTableWidget->setHorizontalHeaderLabels(headings);
    // Initially, the column headings will be equal. After loading, the contacts, the resize
    // mode will be set to ResizeToContents
    ui->contactsTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->requestsTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    // Always select the whole row.
    ui->contactsTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->requestsTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Single selection only.
    ui->contactsTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->requestsTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(ui->contactsTableWidget, SIGNAL(itemSelectionChanged()), this, SLOT(contactSelected()));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabSelected(int)));

}

ContactsDialog::~ContactsDialog() {

    delete ui;

}

void ContactsDialog::contactSelected() {

    contactHandler->checkButtons();

}

void ContactsDialog::contactsModified() {

    contactHandler->loadTable();

}

int ContactsDialog::exec() {

    if (state->nicknames->size() == 0) {
        Pippip::LoadNicknamesTask *task = new Pippip::LoadNicknamesTask(state, this);
        connect(task, SIGNAL(requestComplete(Pippip::EnclaveRequestTask*)),
                contactHandler, SLOT(requestComplete(Pippip::EnclaveRequestTask*)));
        connect(task, SIGNAL(requestFailed(Pippip::EnclaveRequestTask*)),
                contactHandler, SLOT(requestFailed(Pippip::EnclaveRequestTask*)));
        //task->doRequest();
    }
    contactHandler->loadTable();
    updateStatus(Constants::CHECK_ICON, "Contacts loaded");
    return QDialog::exec();

}

void ContactsDialog::nicknamesLoaded() {

    //contactHandler->setNicknames(nicknameManager->getNicknames());
    ui->addButton->setEnabled(true);

}

void ContactsDialog::requestsAcknowledged(Pippip::ContactRequests *acknowledged) {

    //contactsDatabase->requestsAcknowledged(acknowledged);
    ui->tabWidget->setCurrentIndex(0);
    contactHandler->loadTable();

}
/*
void ContactsDialog::setContactsDatabase(Pippip::ContactsDatabase *database) {

    contactsDatabase = database;
    connect(contactsDatabase, SIGNAL(updateStatus(QString,QString)),
            this, SLOT(updateStatus(QString,QString)));
    connect(contactsDatabase, SIGNAL(contactsModified()), this, SLOT(contactsModified()));
    contactHandler->setContactsDatabase(database);

}

void ContactsDialog::setNicknameManager(Pippip::NicknameManager *manager) {

    nicknameManager = manager;
    connect(nicknameManager, SIGNAL(nicknamesLoaded()), this, SLOT(nicknamesLoaded()));
    connect(nicknameManager, SIGNAL(updateStatus(QString,QString)),
            this, SLOT(updateStatus(QString,QString)));
    nicknameManager->loadNicknames();

}
*/
void ContactsDialog::tabSelected(int tab) {

    switch (tab) {
        case 0:
            contactHandler->loadTable();
            break;
        case 1:
            requestHandler->loadRequests();
            break;
    }

}
void ContactsDialog::updateStatus(const QString &icon, const QString& status) {

    ui->statusIconLabel->setText(icon);
    ui->statusLabel->setText(status);
    qApp->processEvents();

}

