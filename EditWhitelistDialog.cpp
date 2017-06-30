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

    newEntry = true;
    ui->addButton->setEnabled(false);
    editRow = ui->whitelistTableWidget->rowCount();
    ui->whitelistTableWidget->setRowCount(editRow + 1);
    QTableWidgetItem *nicknameItem = new QTableWidgetItem;
    nicknameItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->whitelistTableWidget->setItem(editRow, 0, nicknameItem);
    QTableWidgetItem *puidItem = new QTableWidgetItem;
    puidItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->whitelistTableWidget->setItem(editRow, 1, puidItem);

/*    entryLineEdit = new QLineEdit(this);
    editRow = rows;
    editColumn = 0;
    ui->whitelistTableWidget->setCellWidget(editRow, editColumn, entryLineEdit);
    connect(entryLineEdit, SIGNAL(editingFinished()), this, SLOT(editEntryComplete()));
    entryLineEdit->setFocus();*/

}

void EditWhitelistDialog::editEntryComplete() {

    QString entry = entryLineEdit->text();

    if (entry.length() > 0) {
        if (validateEntry(entry)) {
            updateWhitelist(entry);
            ui->whitelistTableWidget->removeCellWidget(editRow, editColumn);
            QTableWidgetItem *entryItem = new QTableWidgetItem(entry);
            entryItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            entryItem->setFlags(entryItem->flags() & ~Qt::ItemIsEditable);
            ui->whitelistTableWidget->setItem(editRow, editColumn, entryItem);
            int nextColumn = editColumn + 1;
            if (nextColumn > 1) {
                nextColumn = 0;
            }
            QTableWidgetItem *nextItem = new QTableWidgetItem;
            nextItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            nextItem->setFlags(nextItem->flags() & ~Qt::ItemIsEditable);
            ui->whitelistTableWidget->setItem(editRow, nextColumn, nextItem);
        }
    }
    else {
        ui->whitelistTableWidget->removeCellWidget(editRow, editColumn);
        editColumn++;
        if (editColumn > 1) {
            editColumn = 0;
        }
        entryLineEdit = new QLineEdit(this);
        ui->whitelistTableWidget->setCellWidget(editRow, editColumn, entryLineEdit);
        connect(entryLineEdit, SIGNAL(editingFinished()), this, SLOT(editEntryComplete()));
        entryLineEdit->setFocus();
    }

}

void EditWhitelistDialog::itemChanged(QTableWidgetItem *item) {

    editRow = item->row();
    editColumn = item->column();
    QString entry = item->text();

    if (entry.length() > 0) {
        if (validateEntry(entry)) {
            updateWhitelist(entry);
        }
        else {
            showAlert();
        }
    }

}

void EditWhitelistDialog::showAlert() {

    if (editColumn == 0) {
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

}

void EditWhitelistDialog::updateWhitelist(const QString &entry) {

    Pippip::Entity entity;
    switch (editColumn) {
        case 0:
            entity.nickname = entry;
            entity.publicId = "";
            break;
        case 1:
            entity.nickname = "";
            entity.publicId = entry;
            break;
    }
    if (newEntry) {
        whitelist.push_back(entity);
    }
    else {
        whitelist[editRow] = entity;
    }

    QTableWidgetItem *nicknameItem = ui->whitelistTableWidget->item(editRow, 0);
    nicknameItem->setText(entity.nickname);
    QTableWidgetItem *puidItem = ui->whitelistTableWidget->item(editRow, 1);
    puidItem->setText(entity.publicId);

}

bool EditWhitelistDialog::validateEntry(const QString &entry) const {

    switch (editColumn) {
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
