#ifndef NEWACCOUNTFINAL_H
#define NEWACCOUNTFINAL_H

#include <QJsonObject>

namespace Pippip {

struct SessionState;

class NewAccountFinal {

    public:
        NewAccountFinal(const QJsonObject& json, SessionState *state);
        ~NewAccountFinal();

    private:
        NewAccountFinal(const NewAccountFinal& other);
        NewAccountFinal& operator =(const NewAccountFinal& other);

    public:
        operator bool();

    public:
        const QString& getError() const { return error; }

    private:
        QJsonObject json;
        SessionState *state;
        QString error;

};

}

#endif // NEWACCOUNTFINAL_H
