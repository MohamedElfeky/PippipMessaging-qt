#include "DeleteNicknameTask.h"
#include "EnclaveRequest.h"
#include "EnclaveResponse.h"
#include "Constants.h"
#include "Nicknames.h"
#include "SessionState.h"

namespace Pippip {

DeleteNicknameTask::DeleteNicknameTask(SessionState *state, QObject *parent)
: EnclaveRequestTask(state, parent) {

    taskName = Constants::DELETE_NICKNAME_TASK;

}

void DeleteNicknameTask::restComplete(const QJsonObject& resp) {
/*
    QString status = response->getValue("status").toString();
    if (status == "deleted") {
        state->nicknames->erase(nickname);
        return true;
    }
    else if (status == "not found") {
        error = nickname + " not found";
    }
    else {
        error = "Invalid server response";
    }
    return false;
*/
}

void DeleteNicknameTask::setNickname(const QString &nick) {

    nickname = nick;
    request->setStringValue("nickname", nickname);

}

}
