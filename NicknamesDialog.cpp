#include "NicknamesDialog.h"
#include "ui_NicknamesDialog.h"
#include "NicknameHandler.h"
#include "WhitelistHandler.h"
#include "Nicknames.h"
#include "Constants.h"
#include "SessionState.h"
#include <QHeaderView>
#include <assert.h>

NicknamesDialog::NicknamesDialog(Pippip::SessionState *st, QWidget *parent)
: QDialog(parent),
  ui(new Ui::NicknamesDialog),
  state(st) {

    ui->setupUi(this);
    ui->statusIconLabel->setText(Constants::INFO_ICON);
    ui->statusLabel->setText("Fetching nicknames");
    nicknameHandler = new NicknameHandler(ui, this);
    whitelistHandler = new WhitelistHandler(ui, this);
    whitelistHandler->setSessionState(state);

    ui->tabWidget->setTabEnabled(1, false);

    QStringList nicknameHeaders;
    nicknameHeaders << "Nickname" << "Contact Policy";
    QStringList whitelistHeaders;
    whitelistHeaders << "Nickname" << "Public ID";
    // Set header labels
    ui->nicknameTableWidget->setHorizontalHeaderLabels(nicknameHeaders);
    ui->whitelistTableWidget->setHorizontalHeaderLabels(whitelistHeaders);
    // Set columns sizes to fill the table width.
    ui->nicknameTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // Always select the whole row.
    ui->nicknameTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->whitelistTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Single selection only.
    ui->nicknameTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->whitelistTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(ui->nicknameTableWidget, SIGNAL(itemSelectionChanged()), this, SLOT(nicknameSelected()));
    connect(ui->whitelistTableWidget, SIGNAL(itemSelectionChanged()), this, SLOT(wlEntrySelected()));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));

}

NicknamesDialog::~NicknamesDialog() {
    delete ui;
}

int NicknamesDialog::exec() {

    nicknameHandler->loadNicknames(state);
    return QDialog::exec();

}

void NicknamesDialog::nicknameSelected() {

    ui->deleteNicknameButton->setEnabled(true);
    int row = ui->nicknameTableWidget->currentRow();
    nicknameHandler->setSelectedRow(row);
    QLabel *policyLabel = dynamic_cast<QLabel*>(ui->nicknameTableWidget->cellWidget(row, 1));
    if (policyLabel != 0) {
        bool enableTab = policyLabel->text() != "Public";
        ui->tabWidget->setTabEnabled(1, enableTab);
    }

}

void NicknamesDialog::updateStatus(const QString &icon, const QString& status) {

    ui->statusIconLabel->setText(icon);
    ui->statusLabel->setText(status);
    qApp->processEvents();

}

void NicknamesDialog::wlEntrySelected() {

    ui->deleteFriendButton->setEnabled(true);
    unsigned row = ui->whitelistTableWidget->currentRow();
    whitelistHandler->setSelectedRow(row);

}

void NicknamesDialog::tabChanged(int tab) {

    switch (tab) {
        case 0:
            nicknameHandler->loadTable();
            break;
        case 1:
            whitelistHandler->setCurrentNickname(ui->nicknameTableWidget->currentRow());
            whitelistHandler->loadTable();
            break;
    }

}
