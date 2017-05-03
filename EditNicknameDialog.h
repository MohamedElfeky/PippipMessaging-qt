#ifndef EDITNICKNAMEDIALOG_H
#define EDITNICKNAMEDIALOG_H

#include <QDialog>

namespace Ui {
class EditNicknameDialog;
}

class EditNicknameDialog : public QDialog {
        Q_OBJECT

    public:
        explicit EditNicknameDialog(QWidget *parent = 0);
        ~EditNicknameDialog();

    public:
        static const int PUBLIC;
        static const int PRIVATE;
        static const int FRIENDSONLY;
        static const int FRIEDSOFFRIENDS;

    public:
        void accept();
        const QString& getNickname() const { return nickname; }
        int getPolicy() const { return policy; }
        void setNickname(const QString& nickname);  // Editing policy only.
        void setPolicy(int policy);

    private:
        Ui::EditNicknameDialog *ui;
        QString nickname;
        int policy;

};

#endif // EDITNICKNAMEDIALOG_H
