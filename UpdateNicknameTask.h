#ifndef UPDATENICKNAMETASK_H
#define UPDATENICKNAMETASK_H

#include "EnclaveRequestTask.h"
#include "Entity.h"

namespace Pippip {

struct Entity;
struct Nickname;

class UpdateNicknameTask : public EnclaveRequestTask {
        Q_OBJECT

    public:
        UpdateNicknameTask(SessionState *state, QObject *parent = 0);
        ~UpdateNicknameTask() {}

    public:
        const QString& getNickname() const { return nickname; }
        const QString& getUpdateAction() const { return updateAction; }
        void setNickname(const QString& nickname);
        void setPolicy(const QString& policy);
        void setUpdateAction(const QString& action);
        void setUpdateType(const QString& type);
        void setWhitelistEntity(const Entity& entity);

    protected:
        bool requestComplete();

    private:
        Nickname& getUpdatedNickname();
        void updateWhitelist();

    private:
        QString nickname;
        QString policy;
        QString updateType;
        QString updateAction;
        Entity entity;

};

}

#endif // UPDATENICKNAMETASK_H
