#ifndef DELETENICKNAMETASK_H
#define DELETENICKNAMETASK_H

#include "EnclaveRequestTask.h"

namespace Pippip {

class DeleteNicknameTask : public EnclaveRequestTask {
        Q_OBJECT

    public:
        DeleteNicknameTask(SessionState *state, QObject *parent = 0);
        ~DeleteNicknameTask() {}

    public:
        const QString& getNickname() const { return nickname; }
        void setNickname(const QString& nickname);

    protected:
        bool requestComplete();

    private:
        QString nickname;

};

}

#endif // DELETENICKNAMETASK_H
