#include "EditWhitelistDialog.h"
#include "ui_EditWhitelistDialog.h"
#include "CriticalAlert.h"
#include <QLineEdit>
#include <QTableWidgetItem>

static const int PUID_LENGTH = 64;
static const QString LETTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const QString PUID_CHARS = "01234567890abcdef";

EditWhitelistDialog::EditWhitelistDialog(QWidget *parent)
: QDialog(parent),
  ui(new Ui::EditWhitelistDialog) {

    ui->setupUi(this);

    // Set up column headings.
    QStringList headings;
    headings << "Nickname" << "Public ID";
    ui->whitelistTableWidget->setHorizontalHeaderLabels(headings);
    ui->whitelistTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    // Always select the whole row.
    ui->whitelistTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Single selection only.
    ui->whitelistTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(ui->whitelistTableWidget, SIGNAL(itemChanged(QTableWidgetItem*)),
                                                this, SLOT(itemChanged(QTableWidgetItem*)));
    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addEntry()));

}

EditWhitelistDialog::~EditWhitelistDialog() {

    delete ui;

}

void EditWhitelistDialog::addEntry() {

    ui->addButton->setEnabled(false);
    int rows = ui->whitelistTableWidget->rowCount();
    ui->whitelistTableWidget->setRowCount(rows + 1);
    QTableWidgetItem *nicknameItem = new QTableWidgetItem;
    nicknameItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->whitelistTableWidget->setItem(rows, 0, nicknameItem);
    QTableWidgetItem *puidItem = new QTableWidgetItem;
    puidItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->whitelistTableWidget->setItem(rows, 1, puidItem);

    Pippip::Entity entity;
    whitelist.push_back(entity);
    ui->whitelistTableWidget->editItem(nicknameItem);

}

void EditWhitelistDialog::itemChanged(QTableWidgetItem *item) {

    editItem = item;
    QString entry = item->text();

    if (entry.length() > 0) {
        if (validateEntry(entry)) {
            updateWhitelist(entry);
        }
        else {
            showAlert();
        }
        ui->addButton->setEnabled(true);
    }

}

void EditWhitelistDialog::showAlert() {

    if (editItem->column() == 0) {
        Pippip::CriticalAlert alert("Invalid Nickname",
                                    "Invalid nickname, please choose another",
                                    "Nicknames must be at least five characters long and must begin with an upper or lower case letter");
        alert.exec();
    }
    else {
        Pippip::CriticalAlert alert("Invalid Public ID",
                                    "Invalid public ID, please enter a valid ID",
                                    "Public IDs must be 64 characters long and must contain digits and lower case 'a' through 'f'");
        alert.exec();
    }
    editItem->setText("");

}

void EditWhitelistDialog::updateWhitelist(const QString &entry) {

    Pippip::Entity& entity = whitelist[editItem->row()];
    switch (editItem->column()) {
        case 0:
            entity.nickname = entry;
            break;
        case 1:
            entity.publicId = entry;
            break;
    }

    int editRow = editItem->row();
    QTableWidgetItem *nicknameItem = ui->whitelistTableWidget->item(editRow, 0);
    nicknameItem->setText(entity.nickname);
    QTableWidgetItem *puidItem = ui->whitelistTableWidget->item(editRow, 1);
    puidItem->setText(entity.publicId);
    qApp->processEvents();

}

bool EditWhitelistDialog::validateEntry(const QString &entry) const {

    switch (editItem->column()) {
        case 0:
            if (entry.length() < 5) {
                return false;
            }
            if (LETTERS.indexOf(entry[0]) < 0) {
                return false;
            }
            break;
        case 1:
            if (entry.length() < PUID_LENGTH) {
                return false;
            }
            for (auto check : entry) {
                if (PUID_CHARS.indexOf(check) < 0) {
                    return false;
                }
            }
            break;
    }

    return true;

}
