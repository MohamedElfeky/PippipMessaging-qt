#ifndef ALERT_H
#define ALERT_H

#include <QObject>
#include <QMessageBox>

namespace Pippip {

class Alert : public QObject {
        Q_OBJECT
    protected:
        explicit Alert(const QString& title,
                       const QString& text,
                       const QString& detailed,
                       QMessageBox::Icon icon,
                       QObject *parent);
        virtual ~Alert();

    public:
        virtual void exec();

    private:
        QMessageBox *message;

};

}

#endif // ALERT_H
