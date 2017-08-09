#include "RequestHandler.h"
#include "RequestManager.h"
#include "Constants.h"
#include "ContactRequests.h"
#include "ContactsDialog.h"
#include "ui_ContactsDialog.h"
#include <QLabel>
#include <QPushButton>
#include <assert.h>

RequestHandler::RequestHandler(Ui::ContactsDialog *u, Pippip::SessionState *st, ContactsDialog *parent)
: QObject(parent),
  ui(u),
  state(st) {

    requestManager = new Pippip::RequestManager(state, this);
    requests = requestManager->getContactRequests();
    acknowledged = requestManager->getAcknowledged();
    connect(requestManager, SIGNAL(requestsLoaded()), this, SLOT(requestsLoaded()));
    connect(requestManager, SIGNAL(loadFailed(QString)), this, SLOT(loadFailed(QString)));
    connect(requestManager, SIGNAL(requestsAcknowledged()), parent, SLOT(requestsAcknowledged()));

    connect(ui->approveButton, SIGNAL(clicked()), SLOT(acceptContact()));

}

void RequestHandler::acceptContact() {

    int row = ui->requestsTableWidget->currentRow();
    requestManager->ackRequest("accept", (*requests)[row].requestId);

}

void RequestHandler::loadFailed(const QString &error) {

    ui->requestStatusLabel->setText(Constants::REDX_ICON + "Request load failed - " + error);
    qApp->processEvents();

}

void RequestHandler::loadRequests() {

    requestManager->loadRequests();

}

void RequestHandler::loadTable() {

    ui->requestsTableWidget->clearContents();
    ui->requestsTableWidget->setRowCount(requests->size());

    int row = 0;
    for (auto request : *requests) {
        QString requested;
        if (request.requested.nickname.length() != 0) {
            requested = request.requested.nickname;
        }
        else if (request.requested.publicId.length() != 0) {
            requested = "My Public ID";
        }
        assert(requested.length() > 0);
        QLabel *requestedLabel = new QLabel(requested);
        requestedLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        requestedLabel->setMargin(5);
        ui->requestsTableWidget->setCellWidget(row, 0, requestedLabel);
        QLabel *requestingNicknameLabel = new QLabel(request.requesting.nickname);
        requestingNicknameLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        requestingNicknameLabel->setMargin(5);
        ui->requestsTableWidget->setCellWidget(row, 1, requestingNicknameLabel);
        QLabel *requestingPublicIdLabel = new QLabel(request.requesting.publicId);
        requestingPublicIdLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        requestingPublicIdLabel->setMargin(5);
        ui->requestsTableWidget->setCellWidget(row++, 2, requestingPublicIdLabel);
    }

    if (requests->size() > 0) {
        ui->requestsTableWidget->selectRow(0);
    }

}

void RequestHandler::requestsLoaded() {

    loadTable();
    ui->requestStatusLabel->setText(Constants::CHECK_ICON + "Requests loaded");
    qApp->processEvents();

}
