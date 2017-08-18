#ifndef NICKNAMESDIALOG_H
#define NICKNAMESDIALOG_H

#include <QDialog>
#include <vector>

namespace Ui {
class NicknamesDialog;
}

namespace Pippip {
    class NicknameManager;
    class SessionState;
}

class NicknameHandler;
class WhitelistHandler;

class NicknamesDialog : public QDialog {
        Q_OBJECT

    public:
        explicit NicknamesDialog(Pippip::SessionState *state, QWidget *parent = 0);
        ~NicknamesDialog();

    public slots:
        void updateStatus(const QString& icon, const QString& status);

    private slots:
        void nicknameSelected();    // Enables delete button
        void wlEntrySelected();     // Enables delete button
        void tabChanged(int);

    public:
        int exec();

    private:
        Ui::NicknamesDialog *ui;
        Pippip::NicknameManager *nicknameManager;
        Pippip::SessionState *state;
        NicknameHandler *nicknameHandler;
        WhitelistHandler *whitelistHandler;

};

#endif // NICKNAMESDIALOG_H
