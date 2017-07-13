#ifndef ADDCONTACTDIALOG_H
#define ADDCONTACTDIALOG_H

#include <QDialog>

namespace Ui {
    class AddContactDialog;
}

namespace Pippip {
    class SessionState;
    class NicknameManager;
}

class AddContactDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit AddContactDialog(Pippip::SessionState *state, QWidget *parent = 0);
        ~AddContactDialog();

    public slots:
        void nicknamesLoaded();
        void pasteId(bool);
        void setPrivateRequest(bool checked);

    public:
        void accept();
        const QString& getIdType() const { return idType; }
        bool getPrivateRequest() const { return privateRequest; }
        const QString& getRequestingNickname() const { return requestingNickname; }
        const QString& getRequestedId() const { return requestedId; }
        void setNicknameManager(Pippip::NicknameManager *manager);

    private:
        bool validateId(const QString& id, const QString& type);

    public:
        static const QString NICKNAME_TYPE;

    private:
        Ui::AddContactDialog *ui;
        Pippip::SessionState *state;
        bool privateRequest;
        bool loadedNicknames;
        Pippip::NicknameManager *nicknameManager;
        QString idType;
        QString requestingNickname;
        QString requestedId;

};

#endif // ADDCONTACTDIALOG_H
