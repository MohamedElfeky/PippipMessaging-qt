#include "NicknameManager.h"
#include "EnclaveRequest.h"
#include "SessionException.h"
#include "SessionState.h"
#include "RESTHandler.h"
#include "CriticalAlert.h"
#include "NicknamesDialog.h"
#include "EnclaveResponse.h"
#include "EnclaveException.h"
#include <coder/ByteArray.h>
#include <QTimer>
#include <QJsonArray>
#include <QJsonValue>
#include <assert.h>

namespace Pippip {

NicknameManager::NicknameManager(QWidget *parent, SessionState *sess)
: QObject(parent),
  loaded(false),
  requestComplete(false),
  timedOut(false),
  state(sess) {

    dialog = dynamic_cast<NicknamesDialog*>(parent);

}

void NicknameManager::addComplete(RESTHandler *handler) {

    if (!timedOut) {
        requestComplete = true;

        EnclaveResponse response(handler->getResponse(), state);
        if (!handler->successful()) {
            CriticalAlert alert("Add Nickname Failed", "Unable to add nickname", handler->getError());
            alert.exec();
        }
        else if (response) {
            Nickname nickname;
            if (getNickname(response, nickname)) {
                emit nicknameAdded(nickname.nickname, nickname.policy);
            }
            else {
                CriticalAlert alert("Add Nickname Failed", "Unable to add nickname", "Invalid server response");
                alert.exec();
            }
        }
        else {
            CriticalAlert alert("Add Nickname Failed", "Unable to add nickname", response.getError());
            alert.exec();
        }
    }

}

void NicknameManager::addNickname(const Nickname &nick) {

    EnclaveRequest req(state);
    req.setRequestType("addNickname");
    req.setValue("publicId", state->publicId);
    QJsonObject nickname;
    nickname["nickname"] = nick.nickname;
    nickname["policy"] = nick.policy;
    nickname["publicId"] = state->publicId;
    QJsonArray array;
    array.append(nickname);
    req.setValue("nicknames", array);
    RESTHandler *handler = new RESTHandler(this);
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(addComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(addComplete(RESTHandler*)));
    QTimer::singleShot(10000, this, SLOT(loadTimedOut()));
    handler->doPost(req);

}

void NicknameManager::delComplete(RESTHandler *handler) {

    if (!timedOut) {
        requestComplete = true;

        EnclaveResponse response(handler->getResponse(), state);
        if (!handler->successful()) {
            CriticalAlert alert("Add Nickname Failed", "Unable to delete nickname", handler->getError());
            alert.exec();
        }
        else if (response) {
            Nickname nickname;
            if (getNickname(response, nickname)) {
                emit nicknameDeleted(nickname.nickname);
            }
            else {
                CriticalAlert alert("Add Nickname Failed", "Unable to delete nickname", "Invalid server response");
                alert.exec();
            }
        }
        else {
            CriticalAlert alert("Add Nickname Failed", "Unable to delete nickname", response.getError());
            alert.exec();
        }
    }

}

void NicknameManager::deleteNickname(const QString& nick) {

    EnclaveRequest req(state);
    req.setRequestType("deleteNickname");
    req.setValue("publicId", state->publicId);
    QJsonObject nickname;
    nickname["nickname"] = nick;
    nickname["policy"] = "";
    nickname["publicId"] = state->publicId;
    QJsonArray array;
    array.append(nickname);
    req.setValue("nicknames", array);
    RESTHandler *handler = new RESTHandler(this);
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(delComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(delComplete(RESTHandler*)));
    QTimer::singleShot(10000, this, SLOT(requestTimedOut()));
    handler->doPost(req);

}

/*
 * Returns the first nickname in the nicknames array.
 */
bool NicknameManager::getNickname(const EnclaveResponse& resp, Nickname& nickname) {

    QJsonValue nicksValue = resp.getValue("nicknames");
    if (nicksValue.isArray()) {
        QJsonArray nicknames  = nicksValue.toArray();
        QJsonValue nickValue = nicknames[0];
        if (nickValue.isObject()) {
            QJsonObject nick = nickValue.toObject();
            QJsonValue name = nick["nickname"];
            QJsonValue policy = nick["policy"];
            QJsonValue publicId = nick["publicId"];
            if (name.isString() && policy.isString() && publicId.isString()) {
                nickname.nickname = name.toString();
                nickname.policy = policy.toString();
                nickname.publicId = publicId.toString();
                return true;
            }
        }
    }
    return false;

}

QString NicknameManager::getPolicy(const QString &nick) const {

    for (Nickname nickname : nicknames) {
        if (nickname.nickname == nick) {
            return nickname.policy;
        }
    }
    assert(false);

}
void NicknameManager::loadComplete(RESTHandler *handler) {

    if (!timedOut) {
        requestComplete = true;

        EnclaveResponse response(handler->getResponse(), state);
        if (!handler->successful()) {
            CriticalAlert alert("Nickname Load Failed", "Unable to fetch nicknames", handler->getError());
            alert.exec();
        }
        else if (response && loadNicknames(response.getJson())) {
            loaded = true;
            emit nicknamesLoaded();
        }
        else {
            CriticalAlert alert("Nickname Load Failed", "Unable to fetch nicknames", response.getError());
            alert.exec();
        }
    }

}

void NicknameManager::loadNicknames() {

    if (!loaded) {
        EnclaveRequest req(state);
        req.setRequestType("getNicknames");
        req.setValue("publicId", state->publicId);
        RESTHandler *handler = new RESTHandler(this);
        connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(loadComplete(RESTHandler*)));
        connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(loadComplete(RESTHandler*)));
        QTimer::singleShot(10000, this, SLOT(requestTimedOut()));
        handler->doPost(req);
    }

}

bool NicknameManager::loadNicknames(const QJsonObject &json) {

    QJsonValue nickValue = json["nicknames"];
    if (!nickValue.isArray()) {
        CriticalAlert alert("Fetch Nicknames Error", "Invalid server response");
        alert.exec();
        return false;
    }

    QJsonArray nicknamesJson = nickValue.toArray();
    for (const QJsonValue& value : nicknamesJson) {
        if (!value.isObject()) {
            CriticalAlert alert("Fetch Nicknames Error", "Invalid server response");
            alert.exec();
            return false;
        }
        QJsonObject nickObj = value.toObject();
        QJsonValue nameValue = nickObj["nickname"];
        QJsonValue policyValue = nickObj["policy"];
        QJsonValue idValue = nickObj["publicId"];
        if (!nameValue.isString() || !policyValue.isString() || !idValue.isString()) {
            CriticalAlert alert("Fetch Nicknames Error", "Invalid server response");
            alert.exec();
            return false;
        }
        Pippip::Nickname nickname = { nameValue.toString(), policyValue.toString(), idValue.toString() };
        nicknames.push_back(nickname);
    }
    return true;

}

void NicknameManager::requestTimedOut() {

    if (!requestComplete) {
        timedOut = true;
        CriticalAlert alert("Enclave Request Error", "Nickname request timed out");
        alert.exec();
    }

}

void NicknameManager::updatePolicy(const Nickname& nick) {

    EnclaveRequest req(state);
    req.setRequestType("updatePolicy");
    req.setValue("publicId", state->publicId);
    QJsonObject nickname;
    nickname["nickname"] = nick.nickname;
    nickname["policy"] = nick.policy;
    nickname["publicId"] = nick.publicId;
    QJsonArray array;
    array.append(nickname);
    req.setValue("nicknames", array);
    RESTHandler *handler = new RESTHandler(this);
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(updateComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(updateComplete(RESTHandler*)));
    QTimer::singleShot(10000, this, SLOT(loadTimedOut()));
    handler->doPost(req);

}

void NicknameManager::updateComplete(RESTHandler *handler) {

    if (!timedOut) {
        requestComplete = true;

        EnclaveResponse response(handler->getResponse(), state);
        if (!handler->successful()) {
            CriticalAlert alert("Add Nickname Failed", "Unable to update policy", handler->getError());
            alert.exec();
        }
        else if (response) {
            Nickname nickname;
            if (getNickname(response, nickname)) {
                emit policyUpdated(nickname.nickname, nickname.policy);
            }
            else {
                CriticalAlert alert("Add Nickname Failed", "Unable to update policy", "Invalid server response");
                alert.exec();
            }
        }
        else {
            CriticalAlert alert("Add Nickname Failed", "Unable to update policy", response.getError());
            alert.exec();
        }
    }

}

} // namespace Pippip
