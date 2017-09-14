#ifndef ADDNICKNAMETASK_H
#define ADDNICKNAMETASK_H

#include "EnclaveRequestTask.h"
#include "Nickname.h"

class QJsonArray;

namespace Pippip {

class SessionState;
struct Nickname;

class AddNicknameTask : public EnclaveRequestTask {
        Q_OBJECT

    public:
        AddNicknameTask(SessionState *state, QObject *parent = 0);
        ~AddNicknameTask() {}

    public:
        void setNickname(const Nickname& nickname);

    public:
        const QString& getStatus() const { return status; }

    protected:
        void restComplete(const QJsonObject& resp);

    private:
        QJsonArray encodeWhitelist();

    private:
        Nickname nickname;
        QString status;

};

}

#endif // ADDNICKNAMETASK_H
