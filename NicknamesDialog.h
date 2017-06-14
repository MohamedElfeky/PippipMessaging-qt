#ifndef NICKNAMESDIALOG_H
#define NICKNAMESDIALOG_H

#include <QDialog>
#include <QModelIndex>
#include <vector>

namespace Ui {
class NicknamesDialog;
}

namespace Pippip {
    struct SessionState;
    struct Nickname;
    class NicknameManager;
}

class NicknamesDialog : public QDialog {
        Q_OBJECT

    public:
        explicit NicknamesDialog(Pippip::SessionState * state, QWidget *parent = 0);
        ~NicknamesDialog();

    public slots:
        void nicknameAdded(QString name, QString policy);
        void nicknameDeleted(QString name);
        void policyUpdated(QString name, QString policy);
        void nicknamesLoaded();

    private slots:
        void addNickname();
        void editNickname(int row, int column);
        void deleteNicknames();
        void nicknameSelected();

    public:
        void setManager(Pippip::NicknameManager *man);

    private:
        Ui::NicknamesDialog *ui;
        Pippip::SessionState *state;
        Pippip::NicknameManager *manager;

};

#endif // NICKNAMESDIALOG_H
