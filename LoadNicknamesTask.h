#ifndef LOADNICKNAMESTASK_H
#define LOADNICKNAMESTASK_H

#include "EnclaveRequestTask.h"
#include "Nickname.h"

class QJsonObject;

namespace Pippip {

class SessionState;

class LoadNicknamesTask : public EnclaveRequestTask {
        Q_OBJECT

    public:
        LoadNicknamesTask(SessionState *state, QObject *parent = 0);
        ~LoadNicknamesTask() {}

    signals:
        void loadNicknamesComplete();
        void loadNicknamesFailed(const QString& error);

    public:
        const NicknameList& getNicknameList() const { return nicknameList; }
        void loadNicknames();

    protected:
        void restComplete(const QJsonObject& resp);
        void restFailed(const QString& error);
        void restTimedOut();

    private:
        void decodeEntity(const QJsonObject& obj, Entity& entity) const;
        void decodeNickname(const QJsonObject& obj, Nickname& nickname) const;
        void decodeRSAKeys(const QJsonObject& obj, RSAKeys& rsaKey) const;

    private:
        NicknameList nicknameList;

};

}

#endif // LOADNICKNAMESTASK_H
