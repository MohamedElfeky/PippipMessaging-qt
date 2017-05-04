#ifndef NICKNAMESDIALOG_H
#define NICKNAMESDIALOG_H

#include <QDialog>
#include <vector>

namespace Ui {
class NicknamesDialog;
}

namespace Pippip {
    class SessionState;
}

class NicknameModel;

class NicknamesDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit NicknamesDialog(Pippip::SessionState * state, QWidget *parent = 0);
        ~NicknamesDialog();

    private slots:
        void addNickname();
        void editNickname();
        void deleteNickname();

    public:
        struct Nickname {
            QString nickname;
            int policy;
        };

        typedef std::vector<Nickname> NicknameList;
        typedef NicknameList::iterator NickIter;
        typedef NicknameList::const_iterator NickConstIter;

        static const int PUBLIC;
        static const int PRIVATE;
        static const int FRIENDSONLY;
        static const int FRIEDSOFFRIENDS;

    public:
        void setNicknames(const NicknameList& list);

    private:
        Ui::NicknamesDialog *ui;
        Pippip::SessionState *state;
        NicknameList nicknames;

};

#endif // NICKNAMESDIALOG_H
