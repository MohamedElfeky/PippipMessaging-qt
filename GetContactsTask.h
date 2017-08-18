#ifndef GETCONTACTSTASK_H
#define GETCONTACTSTASK_H

#include "EnclaveRequestTask.h"
#include "Contact.h"

namespace Pippip {

class GetContactsTask : public EnclaveRequestTask {
        Q_OBJECT

    public:
        GetContactsTask(SessionState *state, QObject *parent = 0);
        ~GetContactsTask() {}

    public:
        void doRequest();
        const ServerContactList& getContacts() const { return contacts; }

    protected:
        bool requestComplete();

    private:
        ServerContactList contacts;

};

}

#endif // GETCONTACTSTASK_H
