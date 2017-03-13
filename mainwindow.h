#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

namespace Pippip {
    class SessionState;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

    public:
        void startFortuna();

    public slots:
        void fortunaStreamError(QString error);
        void fortunaUDPError(QString error, bool fatal);
        void logIn();
        void newAccount();

    private:
        Ui::MainWindow *ui;
        Pippip::SessionState *session;

};

#endif // MAINWINDOW_H
