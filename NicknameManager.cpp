#include "NicknameManager.h"
#include "EnclaveRequest.h"
#include "SessionState.h"
#include "RESTHandler.h"
#include "EnclaveResponse.h"
#include "EnclaveException.h"
#include <QTimer>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <assert.h>
#include <iostream>

namespace Pippip {

NicknameManager::NicknameManager(QWidget *parent, SessionState *sess)
: QObject(parent),
  loaded(false),
  requestComplete(false),
  timedOut(false),
  state(sess) {
}

void NicknameManager::addComplete(RESTHandler *handler) {

    if (!timedOut) {
        requestComplete = true;

        EnclaveResponse response(handler->getResponse(), state);
        if (!handler->successful()) {
            emit requestFailed("Add Nickname", handler->getError());
        }
        else if (response) {
            Nickname nickname;
            if (getNickname(response, nickname)) {
                nicknames.push_back(nickname);
                emit nicknameAdded(nickname.entity.nickname);
            }
            else {
                emit requestFailed("Add Nickname", "Invalid server response");
            }
        }
        else {
            emit requestFailed("Add Nickname", response.getError());
        }
    }

}

void NicknameManager::addNickname(const Nickname &nick) {


    EnclaveRequest req(state);
    req.setRequestType("addNickname");
    req.setValue("publicId", state->publicId);
    QJsonObject nickname = encodeNickname(nick);
    QJsonArray array;
    array.append(nickname);
    req.setValue("nicknames", array);
    RESTHandler *handler = new RESTHandler(this);
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(addComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(addComplete(RESTHandler*)));
    QTimer::singleShot(10000, this, SLOT(requestTimedOut()));
    handler->doPost(req);

}

QJsonObject NicknameManager::encodeNickname(const Nickname &nickname) {

    QJsonObject nickObj;
    QJsonObject entityObj;
    entityObj["nickname"] = nickname.entity.nickname;
    entityObj["publicId"] = state->publicId;
    entityObj["encryptionRSA"] = nickname.entity.encryptionRSA;
    entityObj["signingRSA"] = nickname.entity.signingRSA;
    nickObj["entity"] = entityObj;
    nickObj["policy"] = nickname.policy;
    QJsonArray whitelist;
    for (auto entity : nickname.whitelist) {
        QJsonObject wlEntityObj;
        wlEntityObj["nickname"] = entity.nickname;
        wlEntityObj["publicId"] = entity.publicId;
        wlEntityObj["encryptionRSA"] = entity.encryptionRSA;
        wlEntityObj["signingRSA"] = entity.signingRSA;
        whitelist.append(wlEntityObj);
    }
    nickObj["whitelist"] = whitelist;

    return nickObj;

}

void NicknameManager::delComplete(RESTHandler *handler) {

    if (!timedOut) {
        requestComplete = true;

        EnclaveResponse response(handler->getResponse(), state);
        if (!handler->successful()) {
            emit requestFailed("Delete Nickname", handler->getError());
        }
        else if (response) {
            Nickname nickname;
            if (getNickname(response, nickname)) {
                emit nicknameDeleted(nickname.entity.nickname);
            }
            else {
                emit requestFailed("Delete Nickname", "Invalid server response");
            }
        }
        else {
            emit requestFailed("Delete Nickname", response.getError());
        }
    }

}

void NicknameManager::deleteNickname(const QString& nick) {

    std::cout << "deleteNickname called" << std::endl;
    std::cout << "Nickname : " << nick.toUtf8().toStdString() << std::endl;

    emit nicknameDeleted(nick);

/*
    EnclaveRequest req(state);
    req.setRequestType("deleteNickname");
    req.setValue("publicId", state->publicId);
    Nickname nickname;
    nickname.entity.nickname = nick;
    nickname.entity.publicId = state->publicId;
    QJsonObject nickObj = encodeNickname(nickname);
    QJsonArray array;
    array.append(nickObj);
    req.setValue("nicknames", array);
    RESTHandler *handler = new RESTHandler(this);
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(delComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(delComplete(RESTHandler*)));
    QTimer::singleShot(10000, this, SLOT(requestTimedOut()));
    handler->doPost(req);
*/
}

/*
 * Returns the first nickname in the nicknames array.
 */
bool NicknameManager::getNickname(const EnclaveResponse& resp, Nickname& nickname) {

    QJsonValue nicksValue = resp.getValue("nicknames");
    if (nicksValue.isArray()) {
        QJsonArray nicknames  = nicksValue.toArray();
        QJsonValue nickValue = nicknames[0];
        if (!nickValue.isObject()) {
            return false;
        }
        QJsonObject nick = nickValue.toObject();
        QJsonValue entityValue = nick["entity"];
        if (!entityValue.isObject()) {
            return false;
        }
        QJsonObject entity = entityValue.toObject();
        QJsonValue name = entity["nickname"];
        QJsonValue policy = nick["policy"];
        QJsonValue publicId = entity["publicId"];
        if (!name.isString() || !policy.isString() || !publicId.isString()) {
            return false;
        }
        nickname.entity.nickname = name.toString();
        nickname.policy = policy.toString();
        nickname.entity.publicId = publicId.toString();
        return true;
    }
    return false;

}

const Nickname& NicknameManager::getNickname(const QString &name) const {

    for (const Nickname& nickname : nicknames) {
        if (nickname.entity.nickname == name) {
            return nickname;
        }
    }
    assert(false);

}

void NicknameManager::loadComplete(RESTHandler *handler) {

    if (!timedOut) {
        requestComplete = true;

        EnclaveResponse response(handler->getResponse(), state);
        if (!handler->successful()) {
            emit requestFailed("Fetch Nicknames", handler->getError());
        }
        else if (response) {
            if (loadNicknames(response.getJson())) {
                loaded = true;
                emit nicknamesLoaded(nicknames);
            }
            else {
                emit requestFailed("Fetch Nicknames", "Invalid server response");
            }
        }
        else {
            emit requestFailed("Fetch Nicknames", response.getError());
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
    else {
        emit nicknamesLoaded(nicknames);
    }

}

bool NicknameManager::loadNicknames(const QJsonObject &json) {

    QJsonValue nickValue = json["nicknames"];
    if (!nickValue.isArray()) {
        return false;
    }

    QJsonArray nicknamesJson = nickValue.toArray();
    for (const QJsonValue& value : nicknamesJson) {
        if (!value.isObject()) {
            return false;
        }
        QJsonObject nickObj = value.toObject();
        QJsonValue entityValue = nickObj["entity"];
        if (!entityValue.isObject()) {
            return false;
        }
        QJsonObject entityObj = entityValue.toObject();
        QJsonValue nameValue = entityObj["nickname"];
        QJsonValue policyValue = nickObj["policy"];
        QJsonValue idValue = entityObj["publicId"];
        if (!nameValue.isString() || !policyValue.isString() || !idValue.isString()) {
            return false;
        }
        Entity entity = { nameValue.toString(), idValue.toString(), "", "" };
        EntityList whitelist;
        Nickname nickname = { entity, policyValue.toString(), whitelist };
        nicknames.push_back(nickname);
    }
    return true;

}

void NicknameManager::requestTimedOut() {

    if (!requestComplete) {
        timedOut = true;
        emit requestFailed("Request", "Request timed out");
    }

}

void NicknameManager::updateNickname(const Nickname& nick) {

    std::cout << "updateNickname called" << std::endl;
    std::cout << "Nickname : " << nick.entity.nickname.toUtf8().toStdString() << std::endl;
    std::cout << "Policy = : " << nick.policy.toUtf8().toStdString() << std::endl;

/*    EnclaveRequest req(state);
    req.setRequestType("updateNickname");
    req.setValue("publicId", state->publicId);
    QJsonObject nickname = encodeNickname(nick);
    QJsonArray array;
    array.append(nickname);
    req.setValue("nicknames", array);
    RESTHandler *handler = new RESTHandler(this);
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(updateComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(updateComplete(RESTHandler*)));
    QTimer::singleShot(10000, this, SLOT(requestTimedOut()));
    handler->doPost(req);
*/
}

void NicknameManager::updateComplete(RESTHandler *handler) {

    if (!timedOut) {
        requestComplete = true;

        EnclaveResponse response(handler->getResponse(), state);
        if (!handler->successful()) {
            emit requestFailed("Update Nickname", handler->getError());
        }
        else if (response) {
            Nickname nickname;
            if (getNickname(response, nickname)) {
                emit nicknameUpdated(nickname);
            }
            else {
                emit requestFailed("Update Nickname", "Invalid server response");
            }
        }
        else {
            emit requestFailed("Update Nickname", response.getError());
        }
    }

}

} // namespace Pippip
