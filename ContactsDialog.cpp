#include "ContactsDialog.h"
#include "ui_ContactsDialog.h"
#include "Constants.h"
#include "ContactDirector.h"
#include "ContactRequest.h"
#include "ContactHandler.h"
#include "Nicknames.h"
#include "LoadNicknamesTask.h"
#include "RequestHandler.h"
#include "SessionState.h"
#include "StatusController.h"
#include <QStringList>
#include <QTableWidgetItem>

/**
 * @brief ContactsDialog::ContactsDialog
 * @param st
 * @param parent
 */
ContactsDialog::ContactsDialog(Pippip::SessionState *st, QWidget *parent)
: QDialog(parent),
  ui(new Ui::ContactsDialog),
  state(st) {

    ui->setupUi(this);
    ui->reloadNicknamesButton->setVisible(false);
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

    connect(ui->reloadNicknamesButton, SIGNAL(clicked()), this, SLOT(reloadNicknames()));
    connect(ui->contactsTableWidget, SIGNAL(itemSelectionChanged()), this, SLOT(contactSelected()));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabSelected(int)));

    loadNicknamesTask = new Pippip::LoadNicknamesTask(state, this);
    connect(loadNicknamesTask, SIGNAL(loadNicknamesComplete()), this, SLOT(nicknamesLoaded()));
    connect(loadNicknamesTask, SIGNAL(loadNicknamesFailed(QString)),
            this, SLOT(loadNicknamesFailed(QString)));

}

/**
 * @brief ContactsDialog::~ContactsDialog
 */
ContactsDialog::~ContactsDialog() {

    delete ui;

}

/**
 * @brief ContactsDialog::contactSelected
 */
void ContactsDialog::contactSelected() {

    contactHandler->checkButtons();

}

/**
 * @brief ContactsDialog::contactsModified
 */
void ContactsDialog::contactsModified() {

    contactHandler->loadTable();

}

/**
 * @brief ContactsDialog::exec
 * @return
 */
int ContactsDialog::exec() {

    if (contactDirector->getNicknames().empty()) {
        loadNicknamesTask->loadNicknames();
    }
    contactHandler->loadTable();
    return QDialog::exec();

}

/**
 * @brief ContactsDialog::loadNicknamesFailed
 * @param error
 */
void ContactsDialog::loadNicknamesFailed(const QString &error) {

    QString fullError = QString("Failed to fetch nicknames - ") + error;
    updateStatus(Constants::REDX_ICON, fullError);
    StatusController::instance()->updateStatus(StatusController::error, fullError);
    ui->reloadNicknamesButton->setVisible(true);

}

/**
 * @brief ContactsDialog::nicknamesLoaded
 */
void ContactsDialog::nicknamesLoaded() {

    contactDirector->setNicknames(loadNicknamesTask->getNicknameList());
    ui->addButton->setEnabled(true);
    updateStatus(Constants::CHECK_ICON, "Nicknames loaded");
    ui->reloadNicknamesButton->setVisible(false);

}

/**
 * @brief ContactsDialog::reloadNicknames
 */
void ContactsDialog::reloadNicknames() {

    updateStatus(Constants::CHECK_ICON, "Contacts loaded");
    loadNicknamesTask->loadNicknames();

}

/**
 * @brief ContactsDialog::requestsAcknowledged
 * @param acknowledged
 */
void ContactsDialog::requestsAcknowledged(Pippip::ContactRequests *acknowledged) {

    //contactsDatabase->requestsAcknowledged(acknowledged);
    ui->tabWidget->setCurrentIndex(0);
    contactHandler->loadTable();

}

/**
 * @brief ContactsDialog::setContactDirector
 * @param director
 */
void ContactsDialog::setContactDirector(Pippip::ContactDirector *director) {

    contactDirector = director;
    contactHandler->setContactDirector(director);

}
/*
void ContactsDialog::setNicknameManager(Pippip::NicknameManager *manager) {

    nicknameManager = manager;
    connect(nicknameManager, SIGNAL(nicknamesLoaded()), this, SLOT(nicknamesLoaded()));
    connect(nicknameManager, SIGNAL(updateStatus(QString,QString)),
            this, SLOT(updateStatus(QString,QString)));
    nicknameManager->loadNicknames();

}
*/
/**
 * @brief ContactsDialog::tabSelected
 * @param tab
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

/**
 * @brief ContactsDialog::updateStatus
 * @param icon
 * @param status
 */
void ContactsDialog::updateStatus(const QString &icon, const QString& status) {

    ui->statusIconLabel->setText(icon);
    ui->statusLabel->setText(status);
    qApp->processEvents();

}
