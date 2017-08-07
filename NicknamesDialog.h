#ifndef NICKNAMESDIALOG_H
#define NICKNAMESDIALOG_H

#include <QDialog>
#include <vector>

namespace Ui {
class NicknamesDialog;
}

namespace Pippip {
    class NicknameManager;
    class Nicknames;
}

class NicknameHandler;
class WhitelistHandler;

class NicknamesDialog : public QDialog {
        Q_OBJECT

    public:
        explicit NicknamesDialog(QWidget *parent = 0);
        ~NicknamesDialog();

    public slots:
        void requestFailed(const QString& reqName, const QString& error);

    private slots:
        void nicknameSelected();    // Enables delete button
        //void policyUpdated(const QString&);
        void wlEntrySelected();     // Enables delete button
        void tabChanged(int);

    public:
        void setManager(Pippip::NicknameManager *man);

    private:
        Ui::NicknamesDialog *ui;
        Pippip::NicknameManager *nicknameManager;
        Pippip::Nicknames *nicknames;
        NicknameHandler *nicknameHandler;
        WhitelistHandler *whitelistHandler;

};

#endif // NICKNAMESDIALOG_H
