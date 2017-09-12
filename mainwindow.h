#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

namespace Pippip {
    class SessionState;
    class MessageManager;
    class ContactDirector;
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
        void loggedIn(bool newAccount);
        void loggedOut();
        void manageContacts();
        void manageNicknames();
        void newAccount();
        void newMessage();
        void nicknamesLoaded();
        void requestFailed(const QString& reqName, const QString& error);
        void sendMessage(Qt::Key);

    public:
        void updateStatus(const QString& icon, const QString& status);

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
        QLabel *iconLabel;
        QLabel *statusLabel;
        Pippip::ContactDirector *contactDirector;
        Pippip::MessageManager *messageManager;

};

#endif // MAINWINDOW_H
