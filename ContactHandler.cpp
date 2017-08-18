/*
 * ContactHandler.cpp
 * Copyright (C) 2017 Steve Brenneis <steve.brenneis@secomm.org>
 *
 * PippipMessaging is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PippipMessaging is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ContactHandler.h"
#include "ui_ContactsDialog.h"
#include "KeyFilter.h"
#include "ContactsDatabase.h"
#include "ContactManager.h"
#include "SessionState.h"
#include "Nicknames.h"
#include "Contacts.h"
#include "Constants.h"
#include "RequestContactTask.h"
#include "AddContactsTask.h"
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QStringList>
#include <QRegularExpressionValidator>
#include <QDebug>

ContactHandler::ContactHandler(Ui::ContactsDialog *u, Pippip::SessionState *s, QObject *parent)
: QObject(parent),
  ui(u),
  state(s) {

    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(requestContact()));
    connect(ui->queryButton, SIGNAL(clicked()), this, SLOT(queryContact()));

    nicknameRE.setPattern("[A-z]+[A-z0-9 _\\-]{5,}");
    nicknameRE.setPatternOptions(QRegularExpression::OptimizeOnFirstUsageOption);
    nicknameValidator = new QRegularExpressionValidator(nicknameRE, this);

    puidRE.setPattern("[1-9a-f]+[0-9a-f]{19,}");
    puidRE.setPatternOptions(QRegularExpression::OptimizeOnFirstUsageOption);
    puidValidator = new QRegularExpressionValidator(puidRE, this);

    requestHeadings << "Status" << "Requesting ID" << "Requested ID Type" << "Requested ID";
    contactHeadings << "Status" << "My Nickname" << "Contact Nickname" << "Contact Public ID";

}

void ContactHandler::checkButtons() {

    // All contacts can be deleted.
    ui->deleteButton->setEnabled(true);
    int row = ui->contactsTableWidget->currentRow();
    QLabel *statusLabel =
            dynamic_cast<QLabel*>(ui->contactsTableWidget->cellWidget(row, 0));
    QString status = statusLabel->text();
    ui->queryButton->setEnabled(status == "pending");
    ui->suspendButton->setEnabled(status == "active");

}

/*
 * Calculates the remaining column width for coumn 3.
 */
int ContactHandler::columnGeometry() const {

    int tableWidth = ui->contactsTableWidget->width();
    int columnWidths = ui->contactsTableWidget->columnWidth(0);
    columnWidths += ui->contactsTableWidget->columnWidth(1);
    columnWidths += ui->contactsTableWidget->columnWidth(2);

    return tableWidth - columnWidths;

}
/*
void ContactHandler::contactRequestComplete() {

    loadTable();
    ui->addButton->setEnabled(true);
    ui->pastePushButton->setEnabled(false);
    ui->statusIconLabel->setText(Constants::CHECK_ICON);
    ui->statusLabel->setText("Contact requested");
    qApp->processEvents();

}

void ContactHandler::contactRequestFailed(const QString &error) {

    loadTable();
    ui->addButton->setEnabled(true);
    ui->pastePushButton->setEnabled(false);
    ui->statusIconLabel->setText(Constants::REDX_ICON);
    ui->statusLabel->setText("Contact request failed - " + error);
    qApp->processEvents();

}
*/
/*
 * Invoked by key filter signal on enter or return pressed.
 */
void ContactHandler::idTypeSelected(Qt::Key) {

    QString type = idTypeComboBox->currentText();
    QLabel *requestedLabel = new QLabel(type);
    requestedLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    requestedLabel->setMargin(5);
    ui->contactsTableWidget->setCellWidget(0, 2, requestedLabel);
    if (type == Constants::PUBLIC_ID) {
        requestedType = "P";
    }
    else {
        requestedType = "N";
    }

    requestedIdLineEdit = new QLineEdit;
    if (requestedType == "N") {
        requestedIdLineEdit->setValidator(nicknameValidator);
    }
    else {
        requestedIdLineEdit->setValidator(puidValidator);
    }
    ui->contactsTableWidget->setCellWidget(0, 3, requestedIdLineEdit);
    connect(requestedIdLineEdit, SIGNAL(editingFinished()), this, SLOT(requestedIdEdited()));
    ui->pastePushButton->setEnabled(true);
    ui->contactsTableWidget->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    ui->contactsTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->contactsTableWidget->setColumnWidth(3, columnGeometry());
    requestedIdLineEdit->setFocus();

}

void ContactHandler::loadTable(int startingRow) {

    ui->contactsTableWidget->clearContents();
    ui->contactsTableWidget->setRowCount(state->contacts->size() + startingRow);
    if (startingRow == 0) {
        ui->contactsTableWidget->setHorizontalHeaderLabels(contactHeadings);
    }

    int row = startingRow;
    for (const auto contact : *state->contacts) {
        QLabel *statusLabel = new QLabel(contact.status);
        statusLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        statusLabel->setMargin(5);
        ui->contactsTableWidget->setCellWidget(row, 0, statusLabel);

        QLabel *requestingLabel = new QLabel(contact.contactOf);
        requestingLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        requestingLabel->setMargin(5);
        ui->contactsTableWidget->setCellWidget(row, 1, requestingLabel);

        QLabel *nicknameLabel = new QLabel(contact.entity.nickname);
        nicknameLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        nicknameLabel->setMargin(5);
        ui->contactsTableWidget->setCellWidget(row, 2, nicknameLabel);

        QLabel *puidLabel = new QLabel(contact.entity.publicId);
        puidLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        puidLabel->setMargin(5);
        ui->contactsTableWidget->setCellWidget(row++, 3, puidLabel);
    }
    if (startingRow > 0) {
        ui->contactsTableWidget->selectRow(0);
    }

}

void ContactHandler::queryContact() {
/*
    int row = ui->contactsTableWidget->currentRow();
    long requestId = (*contacts)[row].requestId;
    //contactsDatabase->getContactManager()->queryContact(requestId, true);
*/
}
/*
void ContactHandler::queryFailed(const QString &error) {

    ui->statusIconLabel->setText(Constants::REDX_ICON);
    ui->statusLabel->setText(error);
    qApp->processEvents();

}
*/
void ContactHandler::requestComplete(Pippip::EnclaveRequestTask *task) {

    QString status;
    QString taskName = task->getTaskName();
    if (taskName == Constants::LOAD_NICKNAMES_TASK) {
        status = "Nicknames loaded";
        ui->addButton->setEnabled(state->nicknames->size() > 0);
    }
    else if (taskName == Constants::REQUEST_CONTACT_TASK) {
        Pippip::RequestContactTask *reqTask = dynamic_cast<Pippip::RequestContactTask*>(task);
        Pippip::AddContactsTask *addTask = new Pippip::AddContactsTask(state, this);
        addTask->addContact(reqTask->getServerContact());
        connect(addTask, SIGNAL(requestComplete(Pippip::EnclaveRequestTask*)),
                this, SLOT(requestComplete(Pippip::EnclaveRequestTask*)));
        connect(addTask, SIGNAL(requestFailed(Pippip::EnclaveRequestTask*)),
                this, SLOT(requestFailed(Pippip::EnclaveRequestTask*)));
        status = "Contact requested";
    }
    else if (taskName == Constants::ADD_CONTACTS_TASK) {
        status = "Contacts uploaded";
    }

    updateStatus(Constants::CHECK_ICON, status);
    task->deleteLater();

}

void ContactHandler::requestContact() {

    ui->addButton->setEnabled(false);
    ui->deleteButton->setEnabled(false);
    ui->suspendButton->setEnabled(false);
    ui->queryButton->setEnabled(false);
    loadTable(1);   // Starting at row 1.
    ui->contactsTableWidget->setHorizontalHeaderLabels(requestHeadings);

    QLabel *statusLabel = new QLabel("pending");
    statusLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    statusLabel->setMargin(5);
    ui->contactsTableWidget->setCellWidget(0, 0, statusLabel);

    requestingIdComboBox = new QComboBox;
    QStringList items;
    for (auto nickname : *state->nicknames) {
        items << nickname.entity.nickname;
    }
    items << Constants::MY_PUBLIC_ID;
    requestingIdComboBox->addItems(items);
    KeyFilter *keyFilter = new KeyFilter(requestingIdComboBox);
    keyFilter->addKey(Qt::Key_Enter);
    keyFilter->addKey(Qt::Key_Return);
    keyFilter->addKey(Qt::Key_Tab);
    requestingIdComboBox->installEventFilter(keyFilter);
    connect(keyFilter, SIGNAL(keyPressed(Qt::Key)), this, SLOT(requestingIdSelected(Qt::Key)));
    ui->contactsTableWidget->setCellWidget(0, 1, requestingIdComboBox);
    ui->contactsTableWidget->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    ui->contactsTableWidget->horizontalHeader()->setStretchLastSection(true);
    requestingIdComboBox->setFocus();

}

void ContactHandler::requestFailed(Pippip::EnclaveRequestTask *task) {

    QString status;
    QString taskName = task->getTaskName();
    if (taskName == Constants::LOAD_NICKNAMES_TASK) {
        status = "Failed to load nicknames - " + task->getError();
    }
    else if (taskName == Constants::REQUEST_CONTACT_TASK) {
        status = "Contact request failed - " + task->getError();
    }
    else if (taskName == Constants::ADD_CONTACTS_TASK) {
        status = "Failed to upload contacts - " + task->getError();
    }

    updateStatus(Constants::REDX_ICON, status);
    task->deleteLater();

}

/*
 * Invoked by the requested ID line edit editing finished signal.
 */
void ContactHandler::requestedIdEdited() {

    requestedIdLineEdit->setEnabled(false);
    QLabel *requestedIdLabel = new QLabel(requestedIdLineEdit->text());
    requestedIdLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    requestedIdLabel->setMargin(5);
    ui->contactsTableWidget->setCellWidget(0, 3, requestedIdLabel);
    workingRequest.requestedId = requestedIdLineEdit->text();
    workingRequest.idTypes = requestingType + requestedType;

    Pippip::RequestContactTask *task = new Pippip::RequestContactTask(state, this);
    task->setRequest(workingRequest);
    connect(task, SIGNAL(requestComplete(Pippip::EnclaveRequestTask*)),
            this, SLOT(requestComplete(Pippip::EnclaveRequestTask*)));
    connect(task, SIGNAL(requestFailed(Pippip::EnclaveRequestTask*)),
            this, SLOT(requestFailed(Pippip::EnclaveRequestTask*)));
    task->doRequest();

}

/*
 * Invoked by key filter signal on enter or return pressed.
 */
void ContactHandler::requestingIdSelected(Qt::Key) {

    QString id = requestingIdComboBox->currentText();
    QLabel *requestingLabel = new QLabel(id);
    requestingLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    requestingLabel->setMargin(5);
    ui->contactsTableWidget->setCellWidget(0, 1, requestingLabel);
    if (id == Constants::MY_PUBLIC_ID) {
        workingRequest.requestingId = state->publicId;
        requestingType = "P";
    }
    else {
        workingRequest.requestingId = id;
        requestingType = "N";
    }

    idTypeComboBox = new QComboBox;
    QStringList items;
    items << Constants::NICKNAME_ID << Constants::PUBLIC_ID;
    idTypeComboBox->addItems(items);
    KeyFilter *keyFilter = new KeyFilter(idTypeComboBox);
    keyFilter->addKey(Qt::Key_Enter);
    keyFilter->addKey(Qt::Key_Return);
    keyFilter->addKey(Qt::Key_Tab);
    idTypeComboBox->installEventFilter(keyFilter);
    connect(keyFilter, SIGNAL(keyPressed(Qt::Key)), this, SLOT(idTypeSelected(Qt::Key)));
    ui->contactsTableWidget->setCellWidget(0, 2, idTypeComboBox);
    ui->contactsTableWidget->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    ui->contactsTableWidget->horizontalHeader()->setStretchLastSection(true);
    idTypeComboBox->setFocus();

}
/*
void ContactHandler::setContactsDatabase(Pippip::ContactsDatabase *database) {

    contactsDatabase = database;
    ui->statusIconLabel->setText(Constants::CHECK_ICON);
    ui->statusLabel->setText("Contacts loaded");
    qApp->processEvents();

}
*/
void ContactHandler::updateStatus(const QString &icon, const QString &status) {

    ui->statusIconLabel->setText(icon);
    ui->statusLabel->setText(status);
    qApp->processEvents();

}
