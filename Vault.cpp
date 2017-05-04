/*
 * Vault.cpp
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

#include "Vault.h"
#include "VaultException.h"
#include <CryptoKitty-C/encoding/GCMCodec.h>
#include <CryptoKitty-C/encoding/PEMCodec.h>
#include <CryptoKitty-C/random/FortunaSecureRandom.h>
#include <CryptoKitty-C/digest/SHA256.h>
#include <CryptoKitty-C/exceptions/EncodingException.h>
#include <QDir>
#include <fstream>
#include <sstream>
#include <memory>

namespace Pippip {

static const std::string CONFIG_PATH("/.config/PippipMessaging/");

Vault::Vault() {

    vaultPath = QDir::homePath().toStdString() + CONFIG_PATH;

}

Vault::~Vault() {
}

Vault::Vault(const SessionState &state)
: SessionState(state) {

    vaultPath = QDir::homePath().toStdString() + CONFIG_PATH;

}

void Vault::decodeVault(const coder::ByteArray &ciphertext, const std::string &passphrase) {

    try {
        s2k(passphrase);

        std::string userkey;
        std::string serverkey;
        CK::GCMCodec codec(ciphertext);
        codec.decrypt(vaultKey, coder::ByteArray(passphrase, false));
        codec >> publicId >> genpass >> enclaveKey >> accountRandom
              >> userkey >> serverkey;

        CK::PEMCodec pem(true); // X.509 keys
        serverPublicKey = pem.decodePublicKey(serverkey);
        userPrivateKey = pem.decodePrivateKey(userkey);
        userPublicKey = pem.decodePublicFromPrivate(userkey);

        CK::SHA256 digest;
        authData = digest.digest(genpass);
    }
    catch (CK::EncodingException& e) {
        throw VaultException(e.what());
    }

}

void Vault::encodeVault(const std::string& passphrase) {

    try {
        s2k(passphrase);

        CK::PEMCodec pem(true); // X.509 keys
        std::ostringstream sstr;
        pem.encode(sstr, *serverPublicKey);
        std::ostringstream ustr;
        pem.encode(ustr, *userPrivateKey, *userPublicKey);

        CK::GCMCodec codec;
        codec << publicId << genpass << enclaveKey << accountRandom
              << ustr.str() << sstr.str();
        codec.encrypt(vaultKey, coder::ByteArray(passphrase, false));
        encoded = codec.toArray();

    }
    catch (CK::EncodingException& e) {
        throw VaultException(e.what());
    }
}

void Vault::loadVault(const std::string& accountName, const std::string& passphrase) {

    std::ifstream in(vaultPath + accountName, std::ios::binary);
    if (in) {
        // Find the file size
        std::streampos vsize = in.tellg();
        in.seekg(0, std::ios::end);
        vsize = in.tellg() - vsize;
        in.seekg(0, std::ios::beg);
        // Read the vault.
        std::unique_ptr<uint8_t[]> buffer(new uint8_t[vsize]);
        in.read(reinterpret_cast<char*>(buffer.get()), vsize);
        in.close();
        decodeVault(coder::ByteArray(buffer.get(), vsize), passphrase);
    }
    else {
        throw VaultException("Vault file read error");
    }

}

void Vault::s2k(const std::string& passphrase) {

    char c = passphrase[passphrase.length()-1];
    int count =  c & 0x0f;
    if (count == 0) {
        count = 0x0c;
    }
    count = count << 16;
    coder::ByteArray message(passphrase);
    coder::ByteArray salt(std::string("client vault"));
    message.append(salt);
    CK::SHA256 digest;
    coder::ByteArray hash(digest.digest(message));
    count -= 32;
    while (count > 0) {
        coder::ByteArray ctx(message);
        ctx.append(hash);
        hash = digest.digest(ctx);
        count = count - 32 - message.getLength();
    }
    vaultKey = hash;

}

void Vault::storeVault(const std::string& accountName, const std::string& passphrase) {

    encodeVault(passphrase);
    std::ofstream out(vaultPath + accountName, std::ios::binary);
    if (out) {
        std::unique_ptr<uint8_t[]> buffer(encoded.asArray());
        out.write(reinterpret_cast<const char*>(buffer.get()), encoded.getLength());
        out.close();
    }
    else {
        throw VaultException("Vault file write error");
    }

}

}
