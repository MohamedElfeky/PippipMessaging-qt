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
#include <CryptoKitty-C/random/FortunaSecureRandom.h>
#include <digest/SHA256.h>
#include <digest/SHA1.h>
#include <coder/ByteStreamCodec.h>
#include <CryptoKitty-C/keys/RSAKeyPairGenerator.h>
#include <CryptoKitty-C/keys/KeyPair.h>
#include <ctime>

namespace Pippip {

ParameterGenerator::ParameterGenerator() {
}

ParameterGenerator::~ParameterGenerator() {
}

void ParameterGenerator::generateParameters(const std::string& username) {

    CK::FortunaSecureRandom rnd;

    // Generate the account password
    genpass.setLength(20);
    rnd.nextBytes(genpass);

    // Generate the enclave block cipher key
    CK::SHA256 sha256;
    coder::ByteArray rndbytes(32,0);
    rnd.nextBytes(rndbytes);
    sha256.update(rndbytes);

    // Encode time in milliseconds
    timespec ctime;
    clock_gettime(CLOCK_REALTIME, &ctime);
    uint64_t millis = ctime.tv_sec * 1000;
    millis += ctime.tv_nsec / 1.0e06;
    coder::ByteStreamCodec bytecodec;
    bytecodec << millis;

    sha256.update(bytecodec.toArray());
    enclaveKey = sha256.digest();

    // Generate the user authentication keys.
    CK::RSAKeyPairGenerator keygen(&rnd, 2048);
    CK::KeyPair<CK::RSAPublicKey, CK::RSAPrivateKey> *keypair = keygen.generateKeyPair();
    userPrivateKey = keypair->privateKey();
    userPublicKey = keypair->publicKey();
    keypair->releaseKeys();
    delete keypair;

    CK::SHA1 sha1;
    sha1.update(coder::ByteArray(username));
    clock_gettime(CLOCK_REALTIME, &ctime);
    millis = ctime.tv_sec;
    millis += ctime.tv_nsec / 1.0e06;
    bytecodec.clear();
    bytecodec << millis;
    sha1.update(bytecodec.toArray());
    sha1.update(coder::ByteArray("@secomm.org"));
    publicId = sha1.digest();

}

}
