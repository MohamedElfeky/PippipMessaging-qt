#include "GetContactsTask.h"
#include "Constants.h"
#include "EnclaveRequest.h"
#include "EnclaveResponse.h"
#include <QJsonArray>

namespace Pippip {

GetContactsTask::GetContactsTask(SessionState *state, QObject *parent)
: EnclaveRequestTask(state, parent) {

    taskName = Constants::GET_CONTACTS_TASK;

}

void GetContactsTask::doRequest() {

    QJsonArray statusArray;
    statusArray.append("active");
    statusArray.append("pending");
    statusArray.append("rejected");
    request->setArrayValue("status", statusArray);

    EnclaveRequestTask::doRequest(10);

}

void GetContactsTask::restComplete(const QJsonObject& resp) {
/*
    QJsonValue contactsVal = response->getValue("contacts");
    if (!contactsVal.isArray()) {
        error = "Invalid server response";
        return false;
    }

    QJsonArray contactsArray = contactsVal.toArray();
    for (auto val : contactsArray) {
        if (!val.isObject()) {
            error = "Invalid server response";
            return false;
        }
        QJsonObject contactObj = val.toObject();
        ServerContact contact;
        contact.contactId = contactObj["contactId"].toInt();
        QString encoded = contactObj["contact"].toString();
        contact.contact = QByteArray::fromHex(encoded.toUtf8());
        contacts.push_back(contact);
    }

    return true;
*/
}

}
