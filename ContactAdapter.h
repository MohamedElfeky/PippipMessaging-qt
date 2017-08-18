#ifndef CONTACTADAPTER_H
#define CONTACTADAPTER_H

#include "Contact.h"
#include <QByteArray>

namespace Pippip {

class SessionState;

class ContactAdapter {

    public:
        ContactAdapter(const Contact& contact);
        ContactAdapter(const ServerContact& contact);
        ContactAdapter(const DatabaseContact& contact);
        ~ContactAdapter() {}

    public:
        const DatabaseContact& toDatabase(SessionState *state); // throws CK::EncodingException
        const Contact& toLocal(SessionState *state);            // throws CK::EncodingException
        const ServerContact& toServer(SessionState *state);     // throws CK::EncodingException

    private:
        void decode(SessionState *state);
        void encode(SessionState *state);

    private:
        enum Type { local_contact, server_contact, database_contact };
        Type type;

        Contact localContact;
        ServerContact serverContact;
        DatabaseContact databaseContact;
        QByteArray encoded;

};

}

#endif // CONTACTADAPTER_H
