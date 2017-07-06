#include "ContactsDialog.h"
#include "ContactManager.h"
#include "NicknameManager.h"
#include "ContactRequest.h"
#include "ContactHandler.h"
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
    contactHandler = new ContactHandler(ui, this);

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

void ContactsDialog::contactRequested(Pippip::Contact contact) {

    if (contact.status == "rejected") {
        Pippip::CriticalAlert alert("Contact Request Failed", "Contact request rejected");
        alert.exec();
    }

    ui->contactsStatusLabel->setText("Contact request " + contact.status);
    qApp->processEvents();
    contactManager->loadContacts();

    /*
    int row = ui->contactsTableWidget->rowCount();
    ui->contactsTableWidget->setRowCount(row + 1);
    QTableWidgetItem *statusItem = new QTableWidgetItem(contact.status);
    statusItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->contactsTableWidget->setItem(row, 0, statusItem);
    QTableWidgetItem *nicknameItem = new QTableWidgetItem(contact.entity.nickname);
    nicknameItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->contactsTableWidget->setItem(row, 1, nicknameItem);
    QTableWidgetItem *idItem = new QTableWidgetItem(contact.entity.publicId);
    idItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    if (contact.entity.nickname.length() > 0) {
        ui->contactsStatusLabel->setText("Added " + contact.entity.nickname);
        qApp->processEvents();
    }
    else {
        ui->contactsStatusLabel->setText("Added " + contact.entity.publicId);
        qApp->processEvents();
    }
    */

}

void ContactsDialog::contactsLoaded() {

    const Pippip::ContactList& contacts = contactManager->getContacts();
    ui->contactsTableWidget->setRowCount(contacts.size());
    int row = 0;
    for (auto contact : contacts) {
        QTableWidgetItem *statusItem = new QTableWidgetItem(contact.status);
        statusItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->contactsTableWidget->setItem(row, 0, statusItem);
        QTableWidgetItem *nicknameItem = new QTableWidgetItem(contact.entity.nickname);
        nicknameItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->contactsTableWidget->setItem(row, 1, nicknameItem);
        QTableWidgetItem *idItem = new QTableWidgetItem(contact.entity.publicId);
        idItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->contactsTableWidget->setItem(row, 2, idItem);
        row++;
    }
    ui->contactsStatusLabel->setText("Contacts loaded");
    qApp->processEvents();

}
/*
void ContactsDialog::requestContact() {

    AddContactDialog dialog(state);
    dialog.setNicknameManager(nicknameManager);
    if (dialog.exec() == QDialog::Accepted) {
        QString requestedId = dialog.getRequestedId();
        QString requestingNickname = dialog.getRequestingNickname();
        QString idType = dialog.getIdType();
        bool privateRequest = dialog.getPrivateRequest();
        Pippip::ContactRequest request;
        request.requesting.nickname = privateRequest ? "" : requestingNickname;
        request.requesting.publicId = state->publicId;
        if (idType == "Nickname") {
            request.requested.nickname = requestedId;
        }
        else {
            request.requested.publicId = requestedId;
        }
        request.privateRequest = privateRequest;
        ui->contactsStatusLabel->setText("Requesting contact with " + requestedId);
        qApp->processEvents();
        contactManager->requestContact(request);
    }

}
*/
void ContactsDialog::requestsLoaded() {

}

void ContactsDialog::setContactManager(Pippip::ContactManager *man) {

    contactManager = man;
    contactHandler->setContactManager(contactManager);

    connect(contactManager, SIGNAL(contactsLoaded()), this, SLOT(contactsLoaded()));
    connect(contactManager, SIGNAL(requestsLoaded()), this, SLOT(requestsLoaded()));
    connect(contactManager, SIGNAL(requestedContact(Pippip::Contact)),
                                        this, SLOT(contactRequested(Pippip::Contact)));

    contactManager->loadContacts();

}

void ContactsDialog::setNicknameManager(Pippip::NicknameManager *manager) {

    contactHandler->setNicknames(manager->getNicknames());

}
