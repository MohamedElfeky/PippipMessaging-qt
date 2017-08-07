#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

namespace Pippip {
    class SessionState;
    class NicknameManager;
    // class ContactManager;
    class MessageManager;
    class ContactsDatabase;
}

class QLabel;

class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

    public:
        void startFortuna();

    private slots:
        //void fortunaStreamError(QString error);
        //void fortunaUDPError(QString error, bool fatal);
        void logIn();
        void logOut();
        void loggedIn(const QString& account);
        void loggedOut();
        void manageContacts();
        void manageNicknames();
        void newAccount();
        void newMessage();
        void nicknamesLoaded();
        void requestFailed(const QString& reqName, const QString& error);
        void sendMessage(Qt::Key);
        void updateStatus(const QString& status);

    protected:
        void closeEvent(QCloseEvent *event);

    private:
        void restoreSettings();
        void setDefaults();
        void saveSettings();

    private:
        Ui::MainWindow *ui;
        bool createMessage;
        Pippip::SessionState *session;
        QLabel *statusLabel;
        Pippip::NicknameManager *nicknameManager;
        //Pippip::ContactManager *contactManager;
        Pippip::MessageManager *messageManager;
        Pippip::ContactsDatabase *contactsDatabase;

};

#endif // MAINWINDOW_H
