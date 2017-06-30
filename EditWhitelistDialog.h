#ifndef EDITWHITELISTDIALOG_H
#define EDITWHITELISTDIALOG_H

#include "Entity.h"
#include <QDialog>

namespace Ui {
    class EditWhitelistDialog;
}

class QLineEdit;
class QTableWidgetItem;

class EditWhitelistDialog : public QDialog {
        Q_OBJECT

    public:
        explicit EditWhitelistDialog(QWidget *parent = 0);
        ~EditWhitelistDialog();

    private slots:
        void addEntry();
        void itemChanged(QTableWidgetItem* item);

    public:
        const Pippip::EntityList& getWhitelist() const { return whitelist; }
        void setWhitelist(const Pippip::EntityList& wl) { whitelist = wl; }

    private:
        void showAlert();
        void updateWhitelist(const QString& entry);
        bool validateEntry(const QString& entry) const;

    private:
        Ui::EditWhitelistDialog *ui;
        Pippip::EntityList whitelist;

        QTableWidgetItem *editItem;

};

#endif // EDITWHITELISTDIALOG_H
