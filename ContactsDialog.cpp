#include "ContactsDialog.h"
#include "ui_ContactsDialog.h"
#include "SessionState.h"
#include "ContactManager.h"
#include "AddContactDialog.h"
#include <QStringList>
#include <QTableWidgetItem>

ContactsDialog::ContactsDialog(Pippip::SessionState *st, QWidget *parent)
: QDialog(parent),
  ui(new Ui::ContactsDialog),
  state(st) {

    ui->setupUi(this);

    // Set up column headings.
    QStringList headings;
    headings << "Status" << "Nickname" << "Public ID";
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

    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addContact()));

}

ContactsDialog::~ContactsDialog() {

    delete ui;

}

void ContactsDialog::addContact() {

    AddContactDialog dialog;
    if (dialog.exec() == QDialog::Accepted) {
        QString id = dialog.getId();
        QString idType = dialog.getIdType();
        Pippip::Contact contact;
        if (idType == "Nickname") {
            contact.nickname = id;
        }
        else {
            contact.publicId = id;
        }
        ui->contactsStatusLabel->setText("Adding " + id);
        qApp->processEvents();
        manager->addContact(contact);
    }

}

void ContactsDialog::contactAdded(Pippip::Contact contact) {

    int row = ui->contactsTableWidget->rowCount();
    ui->contactsTableWidget->setRowCount(row + 1);
    QTableWidgetItem *statusItem = new QTableWidgetItem(contact.status);
    statusItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->contactsTableWidget->setItem(row, 0, statusItem);
    QTableWidgetItem *nicknameItem = new QTableWidgetItem(contact.nickname);
    nicknameItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->contactsTableWidget->setItem(row, 1, nicknameItem);
    QTableWidgetItem *idItem = new QTableWidgetItem(contact.publicId);
    idItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    if (contact.nickname.length() > 0) {
        ui->contactsStatusLabel->setText("Added " + contact.nickname);
        qApp->processEvents();
    }
    else {
        ui->contactsStatusLabel->setText("Added " + contact.publicId);
        qApp->processEvents();
    }

}

void ContactsDialog::contactsLoaded() {

    const Pippip::ContactList& contacts = manager->getContacts();
    ui->contactsTableWidget->setRowCount(contacts.size());
    int row = 0;
    for (auto contact : contacts) {
        QTableWidgetItem *statusItem = new QTableWidgetItem(contact.status);
        statusItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->contactsTableWidget->setItem(row, 0, statusItem);
        QTableWidgetItem *nicknameItem = new QTableWidgetItem(contact.nickname);
        nicknameItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->contactsTableWidget->setItem(row, 1, nicknameItem);
        QTableWidgetItem *idItem = new QTableWidgetItem(contact.publicId);
        idItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->contactsTableWidget->setItem(row, 2, idItem);
        row++;
    }
    ui->contactsStatusLabel->setText("Contacts loaded");
    qApp->processEvents();

}

void ContactsDialog::requestsLoaded() {

}

void ContactsDialog::setManager(Pippip::ContactManager *man) {

    manager = man;
    connect(manager, SIGNAL(contactsLoaded()), this, SLOT(contactsLoaded()));
    connect(manager, SIGNAL(requestsLoaded()), this, SLOT(requestsLoaded()));

    manager->loadContacts();

}
