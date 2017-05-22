#ifndef NEWACCOUNTREQUEST_H
#define NEWACCOUNTREQUEST_H

#include "PostPacket.h"

namespace Pippip {

struct SessionState;

class NewAccountRequest : public PostPacket {

    public:
        NewAccountRequest(SessionState *state);
        ~NewAccountRequest();

    private:
        NewAccountRequest(const NewAccountRequest& other);
        NewAccountRequest& operator =(const NewAccountRequest& other);

    public:
        QJsonObject *getJson() const;
        const QString& getUrl() const;

    private:
        SessionState *state;

};

}

#endif // NEWACCOUNTREQUEST_H
