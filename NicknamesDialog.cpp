#include "NicknamesDialog.h"
#include "EditNicknameDialog.h"
#include "ui_NicknamesDialog.h"
#include "SessionState.h"
#include "NicknameManager.h"
#include <QTableWidgetItem>
#include <QHeaderView>
#include <assert.h>

//static QString policyNames[] = { "Private", "Public", "Friends Only", "Friends of Friends" };

NicknamesDialog::NicknamesDialog(Pippip::SessionState *sess, QWidget *parent)
: QDialog(parent),
  ui(new Ui::NicknamesDialog),
  state(sess) {

    ui->setupUi(this);

    QStringList headers;
    headers << "Nickname" << "Policy";
    // Set header labels
    ui->nicknameTableWidget->setHorizontalHeaderLabels(headers);
    // Set columns sizes to fill the table width.
    ui->nicknameTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // Always select the whole row.
    ui->nicknameTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Single selection only.
    ui->nicknameTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addNickname()));
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(deleteNicknames()));
    connect(ui->nicknameTableWidget, SIGNAL(cellDoubleClicked(int,int)),this, SLOT(editNickname(int, int)));
    connect(ui->nicknameTableWidget, SIGNAL(itemSelectionChanged()), this, SLOT(nicknameSelected()));

}

NicknamesDialog::~NicknamesDialog() {
    delete ui;
}

void NicknamesDialog::addNickname() {

    EditNicknameDialog editDialog;
    if (editDialog.exec() == QDialog::Accepted) {
        Pippip::Nickname nickname = { editDialog.getNickname(), editDialog.getPolicy(), state->publicId };
        manager->addNickname(nickname);
    }

}

void NicknamesDialog::deleteNicknames() {

    QList<QTableWidgetItem*> items = ui->nicknameTableWidget->selectedItems();
    for (QTableWidgetItem* item : items) {
        if (item->column() == 0) {
            QString name = item->text();
            manager->deleteNickname(name);
        }
    }

}

void NicknamesDialog::editNickname(int row, int column) {

    QString name = ui->nicknameTableWidget->item(row, column)->text();
    EditNicknameDialog editDialog;
    editDialog.setNickname(name);  // Disables nickname editing.
    editDialog.setPolicy(manager->getPolicy(name));
    if (editDialog.exec() == QDialog::Accepted) {
        Pippip::Nickname nickname = { name, editDialog.getPolicy(), state->publicId };
        manager->updatePolicy(nickname);
    }

}

void NicknamesDialog::nicknameAdded(QString name, QString policy) {

    int rows = ui->nicknameTableWidget->rowCount();
    ui->nicknameTableWidget->setRowCount(rows + 1);
    QTableWidgetItem *nicknameItem = new QTableWidgetItem(name);
    nicknameItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    nicknameItem->setFlags(nicknameItem->flags() & ~Qt::ItemIsEditable);
    ui->nicknameTableWidget->setItem(rows, 0, nicknameItem);
    QTableWidgetItem *policyItem = new QTableWidgetItem(policy);
    policyItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    policyItem->setFlags(nicknameItem->flags() & ~Qt::ItemIsEditable);
    ui->nicknameTableWidget->setItem(rows, 1, policyItem);
    ui->statusLabel->setText(name + " added");
    qApp->processEvents();

}

void NicknamesDialog::nicknameDeleted(QString name) {

    QList<QTableWidgetItem*> items = ui->nicknameTableWidget->findItems(name, Qt::MatchExactly);
    assert(items.count() > 0);
    int row = items.first()->row();
    ui->nicknameTableWidget->removeRow(row);
    ui->statusLabel->setText(name + " deleted");
    qApp->processEvents();

}

void NicknamesDialog::nicknameSelected() {

    ui->deleteButton->setEnabled(true);

}

void NicknamesDialog::nicknamesLoaded() {

    const Pippip::NicknameList& nicknames = manager->getNicknames();
    //int rows = nicknames.size();
    ui->nicknameTableWidget->setRowCount(0);
    for (Pippip::Nickname nickname : nicknames) {
        nicknameAdded(nickname.nickname, nickname.policy);
    }
    ui->statusLabel->setText("Nicknames loaded");
    qApp->processEvents();

}
void NicknamesDialog::policyUpdated(QString name, QString policy) {

    QList<QTableWidgetItem*> items = ui->nicknameTableWidget->findItems(name, Qt::MatchExactly);
    assert(items.count() > 0);
    QTableWidgetItem *item = items.first();
    int row = item->row();
    ui->nicknameTableWidget->item(row, 1)->setText(policy);
    ui->statusLabel->setText("Policy updated");
    qApp->processEvents();

}

void NicknamesDialog::setManager(Pippip::NicknameManager *man) {

    manager = man;
    connect(manager, SIGNAL(nicknamesLoaded()), this, SLOT(nicknamesLoaded()));
    connect(manager, SIGNAL(nicknameDeleted(QString)), this, SLOT(nicknameDeleted(QString)));
    connect(manager, SIGNAL(policyUpdated(QString, QString)), this,
                                                SLOT(policyUpdated(QString,QString)));
    connect(manager, SIGNAL(nicknameAdded(QString, QString)), this,
                                                SLOT(nicknameAdded(QString,QString)));

    manager->loadNicknames();

}
