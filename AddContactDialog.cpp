#include "AddContactDialog.h"
#include "NicknameManager.h"
#include "ui_AddContactDialog.h"
#include <QPushButton>
#include <QClipboard>
#include <QApplication>
#include <QDialogButtonBox>
#include <QMessageBox>

static const int PUID_INDEX = 1;  // ID combobox index for public ID
static const int PUID_LENGTH = 64;
static const QString LETTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const QString PUID_CHARS = "01234567890abcdef";

const QString AddContactDialog::NICKNAME_TYPE = "Nickname";

AddContactDialog::AddContactDialog(Pippip::SessionState* st, QWidget *parent)
: QDialog(parent),
  ui(new Ui::AddContactDialog),
  state(st),
  privateRequest(false),
  loadedNicknames(false) {

    ui->setupUi(this);

    // Disable the OK button until the nicknames are loded and a nickname
    // has been entered.
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    connect(ui->pastePushButton, SIGNAL(clicked(bool)), this, SLOT(pasteId(bool)));
    connect(ui->privateCheckBox, SIGNAL(clicked(bool)), this, SLOT(setPrivateRequest(bool)));

}

AddContactDialog::~AddContactDialog()
{
    delete ui;
}

void AddContactDialog::accept() {

    requestedId = ui->requestedIdLineEdit->text();
    idType = ui->idTypeComboBox->currentText();
    requestingNickname = ui->nicknameComboBox->currentText();

    if (validateId(requestedId, idType)) {
        QDialog::accept();
    }
    else if (idType == NICKNAME_TYPE) {
        QMessageBox *message = new QMessageBox;
        message->addButton(QMessageBox::Ok);
        message->setWindowTitle("Invalid Nickname");
        message->setText("Invalid nickname, please choose another");
        message->setInformativeText(
                    "Nicknames must be at least five characters long and must begin with an upper or lower case letter");
        message->setIcon(QMessageBox::Critical);
        message->exec();
    }
    else {
        QMessageBox *message = new QMessageBox;
        message->addButton(QMessageBox::Ok);
        message->setWindowTitle("Invalid Public ID");
        message->setText("Invalid public ID, please enter a valid ID");
        message->setInformativeText(
                    "Public IDs must be 64 characters long and contain only digits and lower case 'a' through 'f'");
        message->setIcon(QMessageBox::Critical);
        message->exec();
    }

}

void AddContactDialog::nicknamesLoaded() {

    const Pippip::NicknameList& nicknames = nicknameManager->getNicknames();
    if (nicknames.size() > 0) {
        for (auto nickname : nicknames) {
            ui->nicknameComboBox->addItem(nickname.entity.nickname);
        }
        ui->nicknameComboBox->setCurrentIndex(0);
        loadedNicknames = true;
    }
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(loadedNicknames || privateRequest);

}

void AddContactDialog::pasteId(bool) {

    QClipboard *clipboard = QApplication::clipboard();
    ui->requestedIdLineEdit->setText(clipboard->text());
    qApp->processEvents();

}

void AddContactDialog::setNicknameManager(Pippip::NicknameManager *manager) {

    nicknameManager = manager;
    connect(nicknameManager, SIGNAL(nicknamesLoaded()), this, SLOT(nicknamesLoaded()));
    nicknameManager->loadNicknames();

}

void AddContactDialog::setPrivateRequest(bool checked) {

    privateRequest = checked;
    if (privateRequest) {
        ui->idTypeComboBox->setCurrentIndex(PUID_INDEX);
    }
    ui->nicknameComboBox->setEnabled(!privateRequest);
    ui->idTypeComboBox->setEnabled(!privateRequest);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(loadedNicknames || privateRequest);

}

bool AddContactDialog::validateId(const QString& id, const QString& type) {

    if (type == NICKNAME_TYPE) {
        if (id.length() < 5) {
            return false;
        }
        if (LETTERS.indexOf(id[0]) < 0) {
            return false;
        }
    }
    else {
        if (id.length() < PUID_LENGTH) {
            return false;
        }
        for (auto check : id) {
            if (PUID_CHARS.indexOf(check) < 0) {
                return false;
            }
        }
    }

    return true;

}
