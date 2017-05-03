#ifndef NICKNAMESDIALOG_H
#define NICKNAMESDIALOG_H

#include <QDialog>
#include <QModelIndex>
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
        void editNickname(QModelIndex);
        void deleteNickname();

    public:
        void setNicknames(NicknameModel *model);

    private:
        Ui::NicknamesDialog *ui;
        Pippip::SessionState *state;
        NicknameModel *nicknames;

};

#endif // NICKNAMESDIALOG_H
