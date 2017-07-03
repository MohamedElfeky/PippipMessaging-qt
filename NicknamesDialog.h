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

class QTableWidgetItem;
class QComboBox;

class NicknamesDialog : public QDialog {
        Q_OBJECT

    public:
        explicit NicknamesDialog(Pippip::SessionState * state, QWidget *parent = 0);
        ~NicknamesDialog();

    public slots:
        void nicknameAdded(const QString& name);
        void nicknameDeleted(const QString& name);
        void nicknamesLoaded(const Pippip::NicknameList& list);
        void nicknameUpdated(Pippip::Nickname nickname);
        void requestFailed(const QString& reqName, const QString& error);

    private slots:
        void addNickname();
        void deleteNickname();
        void editPolicy(int row, int column);
        void itemChanged(QTableWidgetItem*);
        void nicknameSelected();
        void policyChanged(const QString&);
        void policySelected();

    public:
        void setManager(Pippip::NicknameManager *man);

    private:
        const QString& getPolicy(const QString& name) const;
        const QString& getPolicyName(const QString& policy) const;
        void policyEdited(const QString& policy);
        void loadTable();
        bool validateNickname(const QString& nickname) const;

    private:
        Ui::NicknamesDialog *ui;
        Pippip::SessionState *state;
        Pippip::NicknameManager *manager;
        Pippip::NicknameList nicknames;
        QTableWidgetItem *editItem;
        QComboBox *policyComboBox;
        bool newItem;
        int newColumnCount;

        /*
        bool newNickname;
        QLineEdit *newNicknameLineEdit;
        int editRow;
        QString editedNickname;

        QComboBox *newPolicyComboBox;
        QString editedPolicy;
*/
};

#endif // NICKNAMESDIALOG_H
