#include "ContactAdapter.h"
#include "SessionState.h"
#include "ByteCodec.h"
#include "Function.h"
#include <coder/ByteArray.h>
#include <CryptoKitty-C/encoding/GCMCodec.h>
#include <assert.h>

namespace Pippip {

ContactAdapter::ContactAdapter(const Contact& contact)
: type(local_contact),
  localContact(contact) {

}
/*
ContactAdapter::ContactAdapter(const ServerContact& contact)
: type(server_contact),
  serverContact(contact) {

}

ContactAdapter::ContactAdapter(const DatabaseContact& contact)
: type(database_contact),
  databaseContact(contact) {

}
*/
void ContactAdapter::decode(SessionState *state) {

    coder::ByteArray bytes;

    switch (type) {
        case database_contact:
            //bytes = ByteCodec(databaseContact.contact);
            break;
        case local_contact:
            break;
        case server_contact:
            //bytes = ByteCodec(serverContact.contact);
            break;
    }

    CK::GCMCodec codec(bytes);
    codec.decrypt(state->contactKey, state->authData);
    codec >> localContact;

}

void ContactAdapter::encode(SessionState *state) {

    CK::GCMCodec codec;
    codec << localContact;
    codec.encrypt(state->contactKey, state->authData);
    encoded = ByteCodec(codec.toArray());

}
/*
const DatabaseContact& ContactAdapter::toDatabase(SessionState *state) {

    switch (type) {
        case local_contact:
            databaseContact.id = localContact.contactId;
            encode(state);
            databaseContact.contact = encoded;
            break;
        case server_contact:
            databaseContact.id = serverContact.contactId;
            databaseContact.contact = serverContact.contact;
            break;
        case database_contact:
            assert(false);
    }

    return databaseContact;

}
*/
const Contact& ContactAdapter::toLocal(SessionState *state) {

    switch (type) {
        case local_contact:
            assert(false);
        case server_contact:
            //localContact.contactId = serverContact.contactId;
            decode(state);
            break;
        case database_contact:
            //localContact.contactId = databaseContact.id;
            decode(state);
            break;
    }

    return localContact;

}
/*
const ServerContact& ContactAdapter::toServer(SessionState *state) {

    switch (type) {
        case local_contact:
            serverContact.contactId = localContact.contactId;
            serverContact.status = localContact.status;
            encode(state);
            serverContact.contact = encoded;
            break;
        case server_contact:
            assert(false);
        case database_contact:
            serverContact.contactId = databaseContact.id;
            decode(state);
            serverContact.status = localContact.status;
            serverContact.contact = databaseContact.contact;
            break;
    }

    return serverContact;

}
*/
}
