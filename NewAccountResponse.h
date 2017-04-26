#ifndef NEWACCOUNTRESPONSE_H
#define NEWACCOUNTRESPONSE_H

#include <QJsonObject>

namespace Pippip {

class SessionState;

class NewAccountResponse {

    public:
        NewAccountResponse(const QJsonObject& json, SessionState *state);
        ~NewAccountResponse();

    private:
        NewAccountResponse(const NewAccountResponse& other);
        NewAccountResponse& operator =(const NewAccountResponse& other);

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

#endif // NEWACCOUNTRESPONSE_H
