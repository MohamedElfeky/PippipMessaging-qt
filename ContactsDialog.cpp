#include "ContactsDialog.h"
#include "ContactManager.h"
#include "NicknameManager.h"
#include "ContactRequest.h"
#include "ContactHandler.h"
#include "ContactsDatabase.h"
#include "RequestHandler.h"
#include "ui_ContactsDialog.h"
#include "SessionState.h"
#include "ContactManager.h"
#include "AddContactDialog.h"
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
    //ui->contactsTableWidget->setHorizontalHeaderLabels(headings);
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

void ContactsDialog::nicknamesLoaded() {

    contactHandler->setNicknames(nicknameManager->getNicknames());

}

void ContactsDialog::requestFailed(const QString &reqName, const QString& error) {

    ui->contactsStatusLabel->setText(Constants::REDX_ICON + reqName + " failed - "
                                     + error);
    qApp->processEvents();

}

void ContactsDialog::requestsAcknowledged(Pippip::ContactRequests *acknowledged) {

    contactsDatabase->requestsAcknowledged(acknowledged);
    ui->tabWidget->setCurrentIndex(0);
    contactHandler->loadTable();

}

void ContactsDialog::setContactsDatabase(Pippip::ContactsDatabase *database) {

    contactsDatabase = database;
    contactHandler->setContactsDatabase(database);
    contactHandler->loadTable();

}

void ContactsDialog::setNicknameManager(Pippip::NicknameManager *manager) {

    nicknameManager = manager;
    connect(nicknameManager, SIGNAL(nicknamesLoaded()), this, SLOT(nicknamesLoaded()));
    connect(nicknameManager, SIGNAL(requestFailed(QString,QString)),
            this, SLOT(requestFailed(QString,QString)));
    nicknameManager->loadNicknames();

}

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
