#include "ContactsDialog.h"
#include "ContactManager.h"
#include "NicknameManager.h"
#include "ContactRequest.h"
#include "ContactHandler.h"
#include "RequestHandler.h"
#include "ui_ContactsDialog.h"
#include "SessionState.h"
#include "ContactManager.h"
#include "AddContactDialog.h"
#include "CriticalAlert.h"
#include <QStringList>
#include <QTableWidgetItem>

ContactsDialog::ContactsDialog(Pippip::SessionState *st, QWidget *parent)
: QDialog(parent),
  ui(new Ui::ContactsDialog),
  state(st) {

    ui->setupUi(this);
    contactHandler = new ContactHandler(ui, state, this);
    requestHandler = new RequestHandler(ui, this);

    // Set up column headings.
    QStringList headings;
    headings << "Status" << "Requested By" << "Nickname" << "Public ID";
    ui->contactsTableWidget->setHorizontalHeaderLabels(headings);
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

}

ContactsDialog::~ContactsDialog() {

    delete ui;

}

void ContactsDialog::contactsLoaded() {

    nicknameManager->loadNicknames();

}

void ContactsDialog::nicknamesLoaded() {

    contactHandler->setNicknames(nicknameManager->getNicknames());
    ui->nicknamesStatusLabel->setText("Nicknames loaded");
    qApp->processEvents();

}

void ContactsDialog::requestFailed(const QString &reqName, const QString& error) {

    Pippip::CriticalAlert alert(reqName + " Failed", "Unable to process request", error);
    alert.exec();
    contactHandler->setNicknames(nicknameManager->getNicknames());
    requestHandler->setRequests(contactManager->getRequests());

}

void ContactsDialog::setContactManager(Pippip::ContactManager *man) {

    contactManager = man;
    contactHandler->setContactManager(contactManager);

    connect(contactManager, SIGNAL(contactsLoaded()), contactHandler, SLOT(contactsLoaded()));
    connect(contactManager, SIGNAL(contactsLoaded()), this, SLOT(contactsLoaded()));
    //connect(contactManager, SIGNAL(requestsLoaded()), this, SLOT(requestsLoaded()));
    connect(contactManager, SIGNAL(contactRequested(QString)),
                                        contactHandler, SLOT(contactRequested(QString)));

    contactManager->loadContacts();

}

void ContactsDialog::setNicknameManager(Pippip::NicknameManager *manager) {

    nicknameManager = manager;
    connect(nicknameManager, SIGNAL(nicknamesLoaded()), this, SLOT(nicknamesLoaded()));

}
