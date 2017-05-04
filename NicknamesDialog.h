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

    private slots:
        void addNickname();
        void editNickname(QModelIndex index);
        void deleteNickname();

    public:
        static const int PUBLIC;
        static const int PRIVATE;
        static const int FRIENDSONLY;
        static const int FRIENDSOFFRIENDS;

    public:
        void setManager(Pippip::NicknameManager *man) { manager = man; }

    private:
        void addNickname(const Pippip::Nickname& nickname, int row);
        void setNicknames();

    private:
        Ui::NicknamesDialog *ui;
        Pippip::SessionState *state;
        Pippip::NicknameManager *manager;

};

#endif // NICKNAMESDIALOG_H
