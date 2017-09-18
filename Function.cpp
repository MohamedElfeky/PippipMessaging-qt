#include "Function.h"
#include "Contact.h"
#include "StringCodec.h"
#include <coder/ByteStreamCodec.h>

/**
 * @brief operator >>
 * @param in
 * @param str
 * @return
 */
coder::ByteStreamCodec& operator >> (coder::ByteStreamCodec& in, QString& str) {

    std::string inString;
    in >> inString;
    str = Pippip::StringCodec(inString).getQtString();

    return in;

}

/**
 * @brief operator <<
 * @param out
 * @param str
 * @return
 */
coder::ByteStreamCodec& operator << (coder::ByteStreamCodec& out, const QString& str) {

    Pippip::StringCodec outCodec(str);
    out << outCodec.getStdString();

    return out;

}

/*
 * QString contactOf;
 * Entity entity;
 * RSAKeys rsaKeys;
 * QString nonce;
 * QString authData;
 * QStringList messageKeys;
 * int currentKey;
 * int currentSequence;
 */
/**
 * @brief operator >>
 * @param in
 * @param contact
 * @return
 */
coder::ByteStreamCodec& operator >> (coder::ByteStreamCodec& in, Pippip::Contact& contact) {

    in >> contact.version >> contact.status >> contact.contactId >> contact.requestId
        >> contact.contactOf >> contact.entity.nickname >> contact.entity.publicId
        >> contact.rsaKeys.encryptionRSA >> contact.rsaKeys.signingRSA
        >> contact.nonce >> contact.authData;

    unsigned keysSize;
    in >> keysSize;
    while (contact.messageKeys.size() < keysSize) {
        coder::ByteArray key;
        in >> key;
        contact.messageKeys.push_back(key);
    }

    in >> contact.currentKey >> contact.currentSequence;

    return in;

}

/**
 * @brief operator <<
 * @param out
 * @param contact
 * @return
 */
coder::ByteStreamCodec& operator << (coder::ByteStreamCodec& out, const Pippip::Contact& contact) {

    out << contact.version << contact.status << contact.contactId << contact.requestId
        << contact.contactOf << contact.entity.nickname << contact.entity.publicId
        << contact.rsaKeys.encryptionRSA << contact.rsaKeys.signingRSA
        << contact.nonce << contact.authData;

    unsigned keysSize =  contact.messageKeys.size();
    out << keysSize;
    for (auto key : contact.messageKeys) {
        out << key;
    }

    out << contact.currentKey << contact.currentSequence;

    return out;

}

/**
 * @brief operator <<
 * @param out
 * @param str
 * @return
 */
std::ostream& operator << (std::ostream& out, const QString& str) {

    out << Pippip::StringCodec(str).getStdString();
    return out;

}

/**
 * @brief operator >>
 * @param in
 * @param str
 * @return
 */
std::istream& operator >> (std::istream& in, QString& str) {

    std::string instr;
    in >> instr;
    str = Pippip::StringCodec(instr).getQtString();
    return in;

}

