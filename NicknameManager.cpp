#include "NicknameManager.h"
#include "EnclaveRequest.h"
#include "SessionException.h"
#include "SessionState.h"
#include "RESTHandler.h"
#include "CriticalAlert.h"
#include "NicknamesDialog.h"
#include "EnclaveResponse.h"
#include <QTimer>
#include <QJsonArray>

namespace Pippip {

NicknameManager::NicknameManager(QWidget *parent, SessionState *sess)
: QWidget(parent),
  loadedComplete(false),
  timedOut(false),
  state(sess) {

    policyMap["private"] = NicknamesDialog::PRIVATE;
    policyMap["public"] = NicknamesDialog::PUBLIC;
    policyMap["acquaintance"] = NicknamesDialog::FRIENDSONLY;
    policyMap["acquaintance_cirle"] = NicknamesDialog::FRIENDSOFFRIENDS;

}

void NicknameManager::manageNicknames() {

    if (state->sessionState != SessionState::authenticated) {
        throw SessionException("Session not authenticated");
    }

    EnclaveRequest req(state);
    RESTHandler *handler = new RESTHandler(this);
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(loadComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(loadComplete(RESTHandler*)));
    QTimer::singleShot(20000, this, SLOT(loadTimedOut()));
    handler->doPost(req);

}

void NicknameManager::loadTimedOut() {

    if (!loadedComplete) {
        timedOut = true;
        CriticalAlert alert("Enclave Request Error", "Account request timed out");
        alert.exec();
    }

}

void NicknameManager::loadComplete(RESTHandler *handler) {

    if (!timedOut) {
        loadedComplete = true;

        EnclaveResponse response(handler->getResponse(), state);
        if (!handler->successful()) {
            CriticalAlert alert("Nickname Load Failed", "Unable to fetch nicknames", handler->getError());
            alert.exec();
        }
        else if (response && loadNicknames(response.getJson())) {
            NicknamesDialog *dialog = new NicknamesDialog(state, this);
            dialog->setManager(this);
            dialog->show();
        }
        else {
            CriticalAlert alert("Nickname Load Failed", "Unable to fetch nicknames", response.getError());
            alert.exec();
        }
    }

}

bool NicknameManager::loadNicknames(const QJsonObject &json) {

    QJsonValue nickValue = json["nicknames"];
    if (nickValue.isNull() || !nickValue.isArray()) {
        CriticalAlert alert("Fetch Nicknames Error", "Invalid server response");
        alert.exec();
        return false;
    }

    QJsonArray nicknames = nickValue.toArray();
    for (const QJsonValue& value : nicknames) {
        if (!value.isObject()) {
            CriticalAlert alert("Fetch Nicknames Error", "Invalid server response");
            alert.exec();
            return false;
        }
        QJsonObject nickObj = value.toObject();
        QJsonValue nameValue = nickObj["nickname"];
        QJsonValue policyValue = nickObj["policy"];
        if (nameValue.isNull() || ! nameValue.isString()
                                || policyValue.isNull() || !policyValue.isString()) {
            CriticalAlert alert("Fetch Nicknames Error", "Invalid server response");
            alert.exec();
            return false;
        }
        Pippip::Nickname nickname;
        nickname.nickname = nameValue.toString();
        PolicyConstIter it = policyMap.find(policyValue.toString());
        if (it == policyMap.end()) {
            CriticalAlert alert("Fetch Nicknames Error", "Invalid server response");
            alert.exec();
            return false;
        }
        nickname.policy = it->second;
        state->nicknames.push_back(nickname);
    }
    return true;

}

} // namespace Pippip
