#include "AddContactDialog.h"
#include "ui_AddContactDialog.h"

AddContactDialog::AddContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddContactDialog)
{
    ui->setupUi(this);
}

AddContactDialog::~AddContactDialog()
{
    delete ui;
}

void AddContactDialog::accept() {

    id = ui->idLineEdit->text();
    idType = ui->idTypeComboBox->currentText();

    QDialog::accept();

}
