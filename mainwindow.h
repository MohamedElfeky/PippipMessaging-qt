#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

namespace Pippip {
    struct SessionState;
}

class QLabel;

class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

    public:
        void startFortuna();

    public slots:
        //void fortunaStreamError(QString error);
        //void fortunaUDPError(QString error, bool fatal);
        void logIn();
        void logOut();
        void loggedIn();
        void loggedOut();
        void manageContacts();
        void manageNicknames();
        void newAccount();
        void updateStatus(QString);

    private:
        Ui::MainWindow *ui;
        Pippip::SessionState *session;
        QLabel *statusLabel;

};

#endif // MAINWINDOW_H
