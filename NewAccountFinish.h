#ifndef NEWACCOUNTFINISH_H
#define NEWACCOUNTFINISH_H

#include "PostPacket.h"

namespace Pippip {

class SessionState;

class NewAccountFinish : public PostPacket {

    public:
        NewAccountFinish(SessionState *state);
        ~NewAccountFinish() {}

    private:
        NewAccountFinish(const NewAccountFinish& other);
        NewAccountFinish& operator =(const NewAccountFinish& other);

    public:
        QJsonObject *getJson() const;
        const QString& getUrl() const;

    private:
        SessionState *state;

};

}

#endif // NEWACCOUNTFINISH_H
