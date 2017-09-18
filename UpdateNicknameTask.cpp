#include "UpdateNicknameTask.h"
#include "Constants.h"
#include "EnclaveRequest.h"
#include "EnclaveResponse.h"
#include "SessionState.h"
#include "Nicknames.h"

namespace Pippip {

static Nickname EMPTY;

UpdateNicknameTask::UpdateNicknameTask(SessionState *state, QObject *parent)
: EnclaveRequestTask(state, parent) {

    taskName = Constants::UPDATE_NICKNAME_TASK;

}

void UpdateNicknameTask::restComplete(const QJsonObject& resp) {
/*
    QJsonValue statusVal = response->getValue("status");
    if (!statusVal.isString()) {
        error = "Invalid server response";
        return false;
    }

    QString status = statusVal.toString();

    if (status == "updated") {
        if (updateType == "policy") {
            getUpdatedNickname().policy = policy;
        }
        else {
            updateWhitelist();
        }
        return true;
    }
    else if (status == "not found") {
        error = "Nickname not found";
    }
    else if (status == "mismatch") {
        error = "Nickname ownership conflict";
    }
    else if (status == "whitelist not found") {
        error = "Friends list entry not found";
    }
    return false;
*/
}

Nickname& UpdateNicknameTask::getUpdatedNickname() {

    //for (auto& nick : *state->nicknames) {
    //    if (nick.entity.nickname == nickname) {
    //        return nick;
    //    }
    //}

    // Never happen.
    return EMPTY;

}

void UpdateNicknameTask::setNickname(const QString& nick) {

    nickname = nick;
    request->setStringValue("nickname", nickname);

}

void UpdateNicknameTask::setPolicy(const QString& pol) {

    policy = pol;
    request->setStringValue("policy", policy);

}

void UpdateNicknameTask::setUpdateAction(const QString& action) {

    updateAction = action;
    request->setStringValue("action", action);

}

void UpdateNicknameTask::setUpdateType(const QString& type) {

    updateType = type;
    request->setStringValue("type", type);

}

void UpdateNicknameTask::setWhitelistEntity(const Entity& wlEntry) {

    entity = wlEntry;
    request->setStringValue("whitelistNickname", entity.nickname);
    request->setStringValue("whitelistPublicId", entity.publicId);

}

void UpdateNicknameTask::updateWhitelist() {


    if (updateAction == "add") {
        getUpdatedNickname().whitelist.push_back(entity);
    }
    else if (updateAction == "delete") {
        EntityList tmp;
        Nickname& nick = getUpdatedNickname();
        for (auto wlEntry : nick.whitelist) {
            if (wlEntry.nickname != entity.nickname) {
                tmp.push_back(wlEntry);
            }
        }
        nick.whitelist.swap(tmp);
    }
    else if (updateAction == "modify") {
        Nickname& nick = getUpdatedNickname();
        for (auto& wlEntry : nick.whitelist) {
            if (wlEntry.nickname == entity.nickname) {
                wlEntry.publicId = entity.publicId;
            }
        }
    }

}

}
