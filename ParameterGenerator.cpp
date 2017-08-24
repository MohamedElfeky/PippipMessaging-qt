/*
 * ParameterGenerator.cpp
 * Copyright (C) 2017 Steve Brenneis <steve.brenneis@secomm.org>
 *
 * PippipMessaging is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PippipMessaging is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ParameterGenerator.h"
//#include "StringCodec.h"
#include "ByteCodec.h"
#include <CryptoKitty-C/random/FortunaSecureRandom.h>
#include <CryptoKitty-C/digest/SHA256.h>
#include <CryptoKitty-C/digest/SHA1.h>
#include <coder/ByteStreamCodec.h>
#include <CryptoKitty-C/keys/RSAKeyPairGenerator.h>
#include <CryptoKitty-C/keys/KeyPair.h>
#include <CryptoKitty-C/keys/RSAPublicKey.h>
#include <CryptoKitty-C/keys/RSAPrivateKey.h>
#include <ctime>

namespace Pippip {

ParameterGenerator::ParameterGenerator() {
}

void ParameterGenerator::generateParameters(const QString& username) {

    accountName = username;

    CK::FortunaSecureRandom rnd;

    // Generate the account password
    genpass.setLength(20);
    rnd.nextBytes(genpass);

    // Generate the GCM authentication data.
    CK::SHA256 digest;
    authData = digest.digest(genpass);

    // Generate the enclave block cipher key
    digest.reset();
    coder::ByteArray rndbytes(32,0);
    rnd.nextBytes(rndbytes);
    digest.update(rndbytes);

    // Encode time in milliseconds
    timespec ctime;
    clock_gettime(CLOCK_REALTIME, &ctime);
    uint64_t millis = ctime.tv_sec * 1000;
    millis += ctime.tv_nsec / 1.0e06;
    coder::ByteStreamCodec bytecodec;
    bytecodec << millis;

    digest.update(bytecodec.toArray());
    enclaveKey = digest.digest();

    // Generate the contact list encryption key.
    contactKey.setLength(32);
    rnd.nextBytes(contactKey);

    // Generate the user authentication keys.
    CK::RSAKeyPairGenerator keygen(&rnd, 2048);
    CK::KeyPair<CK::RSAPublicKey, CK::RSAPrivateKey> *keypair = keygen.generateKeyPair();
    userPrivateKey = keypair->privateKey();
    userPublicKey = keypair->publicKey();
    keypair->releaseKeys();
    delete keypair;

    //Generate the public ID
    CK::SHA1 sha1;
    ByteCodec nameBytes(accountName);
    sha1.update(nameBytes);
    clock_gettime(CLOCK_REALTIME, &ctime);
    millis = ctime.tv_sec;
    millis += ctime.tv_nsec / 1.0e06;
    bytecodec.clear();
    bytecodec << millis;
    sha1.update(bytecodec.toArray());
    sha1.update(coder::ByteArray("@secomm.org"));
    coder::ByteArray idBytes(sha1.digest());
    publicId = QString(idBytes.toHexString().c_str());

}

}
