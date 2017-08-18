#ifndef LOADNICKNAMESTASK_H
#define LOADNICKNAMESTASK_H

#include "EnclaveRequestTask.h"

class QJsonObject;

namespace Pippip {

class SessionState;
struct Nickname;
struct Entity;
struct RSAKeys;

class LoadNicknamesTask : public EnclaveRequestTask {
        Q_OBJECT

    public:
        LoadNicknamesTask(SessionState *state, QObject *parent = 0);
        ~LoadNicknamesTask() {}

    protected:
        bool requestComplete();

    private:
        bool decodeEntity(const QJsonObject& obj, Entity& entity) const;
        bool decodeNickname(const QJsonObject& obj, Nickname& nickname) const;
        bool decodeRSAKeys(const QJsonObject& obj, RSAKeys& rsaKey) const;

};

}

#endif // LOADNICKNAMESTASK_H
