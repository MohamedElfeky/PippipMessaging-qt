#include "UpdateContactTask.h"
#include "EnclaveResponse.h"
#include "Contact.h"
#include "Function.h"
#include "SessionState.h"
#include "StringCodec.h"
#include "TaskException.h"
#include "Constants.h"
#include <CryptoKitty-C/encoding/GCMCodec.h>
#include <CryptoKitty-C/exceptions/EncodingException.h>

namespace Pippip {

UpdateContactTask::UpdateContactTask(SessionState *state, QObject *parent)
: EnclaveRequestTask(state, parent) {

    taskName = Constants::UPDATE_CONTACT_TASK;

}

bool UpdateContactTask::requestComplete() {

    QJsonValue resultVal = response->getValue("result");
    if (!resultVal.isString()) {
        error = "Invalid server response";
        return false;
    }

    result = resultVal.toString();
    return true;

}

void UpdateContactTask::setContact(const Contact &contact) {

    try {
        QJsonObject contactObj;
        contactObj["contactId"] = static_cast<int>(contact.contactId);
        contactObj["status"] = contact.status;
        CK::GCMCodec codec;
        codec << contact;
        codec.encrypt(state->contactKey, state->authData);
        StringCodec encoded(codec.toArray().toHexString());
        contactObj["encoded"] = encoded.getQtString();
        request.setObjectValue("contact", contactObj);
    }
    catch (CK::EncodingException& e) {
        throw TaskException(std::string("Contact encoding error - ") + e.what());
    }

}

}
