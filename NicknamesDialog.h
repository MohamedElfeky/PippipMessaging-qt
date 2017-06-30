#ifndef NICKNAMESDIALOG_H
#define NICKNAMESDIALOG_H

#include "Nickname.h"
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

class QLineEdit;
class QComboBox;

class NicknamesDialog : public QDialog {
        Q_OBJECT

    public:
        explicit NicknamesDialog(Pippip::SessionState * state, QWidget *parent = 0);
        ~NicknamesDialog();

    public slots:
        void nicknameAdded(QString name, QString policy);
        void nicknameDeleted(QString name);
        void nicknamesLoaded();
        void nicknameUpdated(Pippip::Nickname nickname);

    private slots:
        void addNickname();
        void deleteNicknames();
        void editPolicy(int row, int column);
        void nicknameEditComplete();
        void nicknameSelected();
        void policyEditComplete(int);

    public:
        void setManager(Pippip::NicknameManager *man);

    private:
        QString getPolicyName(QString policy) const;
        bool validateNickname(const QString& nickname) const;

    private:
        Ui::NicknamesDialog *ui;
        Pippip::SessionState *state;
        Pippip::NicknameManager *manager;

        bool newNickname;
        QLineEdit *newNicknameLineEdit;
        int editRow;
        QString editedNickname;

        QComboBox *newPolicyComboBox;
        QString editedPolicy;

};

#endif // NICKNAMESDIALOG_H
