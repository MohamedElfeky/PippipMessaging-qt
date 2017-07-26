#include "NicknamesDialog.h"
#include "ui_NicknamesDialog.h"
#include "NicknameHandler.h"
#include "WhitelistHandler.h"
#include "NicknameManager.h"
#include "CriticalAlert.h"
#include <QHeaderView>
#include <assert.h>

NicknamesDialog::NicknamesDialog(QWidget *parent)
: QDialog(parent),
  ui(new Ui::NicknamesDialog) {

    ui->setupUi(this);
    nicknameHandler = new NicknameHandler(ui, this);
    whitelistHandler = new WhitelistHandler(ui, this);
    connect(whitelistHandler, SIGNAL(whitelistChanged(Pippip::EntityList)),
                                nicknameHandler, SLOT(whitelistChanged(Pippip::EntityList)));
    connect(nicknameHandler, SIGNAL(policyUpdated(QString)), this, SLOT(policyUpdated(QString)));

    ui->tabWidget->setTabEnabled(1, false);

    QStringList nicknameHeaders;
    nicknameHeaders << "Nickname" << "Policy";
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
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(wlTabSelected(int)));

}

NicknamesDialog::~NicknamesDialog() {
    delete ui;
}

void NicknamesDialog::nicknameSelected() {

    ui->deleteNicknameButton->setEnabled(true);
    bool enableTab = false;
    if (ui->nicknameTableWidget->rowCount() > 0) {
        int row = ui->nicknameTableWidget->currentRow();
        QLabel *policyLabel = dynamic_cast<QLabel*>(ui->nicknameTableWidget->cellWidget(row, 1));
        if (policyLabel != 0) {     // Empty row or widget is QComboBox
            QString policy = policyLabel->text();
            enableTab = policy != "Public";
        }
    }
    ui->tabWidget->setTabEnabled(1, enableTab);

}

void NicknamesDialog::policyUpdated(const QString& policy) {

    ui->whitelistTab->setEnabled(policy != "Public");

}

void NicknamesDialog::requestFailed(const QString &reqName, const QString& error) {

    Pippip::CriticalAlert alert(reqName + " Failed", "Unable to process request", error);
    alert.exec();

}

void NicknamesDialog::setManager(Pippip::NicknameManager *man) {

    manager = man;
    nicknameHandler->setManager(manager);

    connect(manager, SIGNAL(nicknamesLoaded()), nicknameHandler, SLOT(nicknamesLoaded()));
    connect(manager, SIGNAL(nicknameDeleted(QString)), nicknameHandler, SLOT(nicknameDeleted(QString)));
    connect(manager, SIGNAL(nicknameUpdated(Pippip::Nickname)),
                                                nicknameHandler, SLOT(nicknameUpdated(Pippip::Nickname)));
    connect(manager, SIGNAL(nicknameAdded()), nicknameHandler, SLOT(nicknameAdded()));
    connect(manager, SIGNAL(requestFailed(QString,QString)), nicknameHandler, SLOT(requestFailed(QString,QString)));

    manager->loadNicknames();

}

void NicknamesDialog::wlEntrySelected() {

    ui->deleteFriendButton->setEnabled(true);

}

void NicknamesDialog::wlTabSelected(int tab) {

    if (tab == 1) {
        const Pippip::NicknameList& list = manager->getNicknames();
        size_t row = ui->nicknameTableWidget->currentRow();
        assert(row < list.size());
        whitelistHandler->setWhitelist(list[row].whitelist);
    }

}
