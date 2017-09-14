#include "AddContactsTask.h"
#include "EnclaveResponse.h"
#include "Constants.h"
#include <QJsonArray>

namespace Pippip {

AddContactsTask::AddContactsTask(SessionState *state, QObject *parent)
: EnclaveRequestTask(state, parent) {

    taskName = Constants::ADD_CONTACTS_TASK;

}
/*
void AddContactsTask::addContact(const ServerContact &contact) {

    contacts.push_back(contact);

}
*/
void AddContactsTask::doRequest() {
/*
    QJsonArray contactsArray;
    for (ServerContact contact : contacts) {
        QJsonObject contactObj;
        contactObj["contactId"] = static_cast<int>(contact.contactId);
        contactObj["status"] = contact.status;
        contactObj["encoded"] = QString(contact.contact.toHex());
        contactsArray.append(contactObj);
    }
    request.setArrayValue("contacts", contactsArray);
*/
    EnclaveRequestTask::doRequest(10);

}

void AddContactsTask::restComplete(const QJsonObject& resp) {
/*
    QJsonValue statusVal = response->getValue("status");
    if (!statusVal.isString()) {
        error = "Invalid server response";
        return false;
    }
    status = statusVal.toString();
    return true;
*/
}

}
