#ifndef ADDCONTACTSTASK_H
#define ADDCONTACTSTASK_H

#include "EnclaveRequestTask.h"
#include "Contact.h"

namespace Pippip {

class AddContactsTask : public EnclaveRequestTask {
        Q_OBJECT

    public:
        AddContactsTask(SessionState *state, QObject *parent = 0);
        ~AddContactsTask() {}

    public:
        void addContact(const ServerContact& contact);
        const QString& getStatus() const { return status; }
        void setContacts(const ServerContactList& list) { contacts = list; }

    public:
        void doRequest();

    protected:
        bool requestComplete();

    private:
        QString status;
        ServerContactList contacts;

};

}

#endif // ADDCONTACTSTASK_H
