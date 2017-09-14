#include "AddNicknameTask.h"
#include "SessionState.h"
#include "EnclaveResponse.h"
#include "EnclaveRequest.h"
#include "Nicknames.h"
#include "Constants.h"
#include <QJsonArray>

namespace Pippip {

AddNicknameTask::AddNicknameTask(SessionState *state, QObject *parent)
: EnclaveRequestTask(state, parent) {

    taskName = Constants::ADD_NICKNAME_TASK;

}

QJsonArray AddNicknameTask::encodeWhitelist() {

    QJsonArray whitelist;
    for (auto entity : nickname.whitelist) {
        QJsonObject wlEntityObj;
        wlEntityObj["nickname"] = entity.nickname;
        wlEntityObj["publicId"] = entity.publicId;
        whitelist.append(wlEntityObj);
    }
    return whitelist;

}

void AddNicknameTask::setNickname(const Nickname &nick) {

    nickname = nick;
    request->setStringValue("nickname", nick.entity.nickname);
    request->setStringValue("policy", nick.policy);
    request->setArrayValue("whitelist", encodeWhitelist());

}

void AddNicknameTask::restComplete(const QJsonObject& resp) {
/*
    QJsonValue statusVal = response->getValue("status");
    if (!statusVal.isString()) {
        error = "Invalid server response";
        return false;
    }

    status = response->getValue("status").toString();
    if (status == "added") {
        state->nicknames->append(nickname);
        return true;
    }
    else if (status == "exists") {
        error = "Nickname exists";
    }
    else {
        error = "Invalid server response";
    }
*/
}

}
