#include "NicknamesDialog.h"
#include "ui_NicknamesDialog.h"
#include "NicknameHandler.h"
#include "WhitelistHandler.h"
#include "NicknameManager.h"
#include "Nicknames.h"
#include "Constants.h"
#include <QHeaderView>
#include <assert.h>

NicknamesDialog::NicknamesDialog(QWidget *parent)
: QDialog(parent),
  ui(new Ui::NicknamesDialog) {

    ui->setupUi(this);
    ui->statusLabel->setText(Constants::INFO_ICON + "Fetching nicknames");
    nicknameHandler = new NicknameHandler(ui, this);
    whitelistHandler = new WhitelistHandler(ui, this);
//    connect(whitelistHandler, SIGNAL(whitelistUpdated(Pippip::EntityList)),
//            nicknameHandler, SLOT(whitelistUpdated(Pippip::EntityList)));

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

void NicknamesDialog::nicknameSelected() {

    ui->deleteNicknameButton->setEnabled(true);
    bool enableTab = false;
    unsigned row = ui->nicknameTableWidget->currentRow();
    assert(row < nicknames->size());
    nicknames->setCurrentIndex(row);
    QLabel *policyLabel = dynamic_cast<QLabel*>(ui->nicknameTableWidget->cellWidget(row, 1));
    if (policyLabel != 0) {     // Empty row or widget is QComboBox
        QString policy = policyLabel->text();
        enableTab = policy != "Public";
    }
    ui->tabWidget->setTabEnabled(1, enableTab);

}
/*
void NicknamesDialog::policyUpdated(const QString& policy) {

    ui->whitelistTab->setEnabled(policy != "Public");

}
*/
void NicknamesDialog::requestFailed(const QString &reqName, const QString& error) {

    ui->statusLabel->setText(Constants::REDX_ICON + reqName + " failed: " + error);
    qApp->processEvents();

}

void NicknamesDialog::setManager(Pippip::NicknameManager *manager) {

    nicknameManager = manager;
    nicknameHandler->setManager(manager);
    whitelistHandler->setNicknameManager(manager);

    // Nickname handler signals
    connect(nicknameManager, SIGNAL(addCompleted(QString)),
            nicknameHandler, SLOT(nicknameAdded(QString)));
    connect(nicknameManager, SIGNAL(deleteCompleted(QString)),
            nicknameHandler, SLOT(nicknameDeleted(QString)));
    connect(nicknameManager, SIGNAL(deleteCompleted(QString)),
            nicknameHandler, SLOT(nicknameDeleted(QString)));
    connect(nicknameManager, SIGNAL(nicknamesLoaded()),
            nicknameHandler, SLOT(nicknamesLoaded()));
    connect(nicknameManager, SIGNAL(policyUpdated(QString)),
            nicknameHandler, SLOT(policyUpdated(QString)));
    connect(nicknameManager, SIGNAL(taskFailed(QString)),
            nicknameHandler, SLOT(taskFailed(QString)));

    // Whitelist handler signals
    connect(nicknameManager, SIGNAL(whitelistUpdated(QString)),
            whitelistHandler, SLOT(whitelistUpdated(QString)));
    connect(nicknameManager, SIGNAL(taskFailed(QString)),
            whitelistHandler, SLOT(taskFailed(QString)));

    nicknames = nicknameManager->getNicknames();
    nicknameManager->loadNicknames();

}

void NicknamesDialog::wlEntrySelected() {

    ui->deleteFriendButton->setEnabled(true);
    unsigned row = ui->whitelistTableWidget->currentRow();
    whitelistHandler->setCurrentIndex(row);

}

void NicknamesDialog::tabChanged(int tab) {

    switch (tab) {
        case 0:
            break;
        case 1:
            whitelistHandler->loadTable();
            break;
    }

}
