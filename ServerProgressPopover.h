#ifndef SERVERPROGRESSPOPOVER_H
#define SERVERPROGRESSPOPOVER_H

#include <QDialog>

namespace Ui {
    class ServerProgressPopover;
}

class ServerProgressPopover : public QDialog {
        Q_OBJECT

    public:
        explicit ServerProgressPopover(QWidget *parent = 0);
        ~ServerProgressPopover();

    public slots:
        void done(int res);
        void show();

    protected slots:
        void updateProgress();

    public:
        void setInfo1(const QString& info);
        void setInfo2(const QString& info);
        void setTimeEstimate(int estimate);

    private:
        Ui::ServerProgressPopover *ui;
        bool finished;
        int increments;

};

#endif // SERVERPROGRESSPOPOVER_H
