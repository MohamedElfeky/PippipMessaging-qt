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
#include "ByteCodec.h"
#include "StringCodec.h"
#include <CryptoKitty-C/encoding/GCMCodec.h>
#include <CryptoKitty-C/encoding/PEMCodec.h>
#include <CryptoKitty-C/random/FortunaSecureRandom.h>
#include <CryptoKitty-C/digest/SHA256.h>
#include <CryptoKitty-C/exceptions/EncodingException.h>
#include <CryptoKitty-C/keys/RSAPrivateKey.h>
#include <CryptoKitty-C/keys/RSAPublicKey.h>
#include <CryptoKitty-C/keys/RSAPrivateCrtKey.h>
#include <QSettings>
#include <QByteArray>
#include <QFile>
#include <fstream>
#include <sstream>
#include <memory>

namespace Pippip {

static const std::string CURRENT_VERSION = "1.0";

Vault::Vault() {

    QSettings settings;
    vaultPath = settings.value("Defaults/vaultPath").toString();

}

Vault::Vault(const SessionState &state)
: SessionState(state) {

    // We do this so that Vault doesn't steal the key pointers.
    // It would cause a double delete in MainWindow::loggedOut.
    serverPublicKey = new CK::RSAPublicKey(*state.serverPublicKey);
    CK::RSAPrivateCrtKey *crtKey = dynamic_cast<CK::RSAPrivateCrtKey*>(state.userPrivateKey);
    userPrivateKey = new CK::RSAPrivateCrtKey(*crtKey);
    userPublicKey = new CK::RSAPublicKey(*state.userPublicKey);

    QSettings settings;
    vaultPath = settings.value("Defaults/vaultPath").toString();

}

void Vault::decodeVault(const coder::ByteArray &ciphertext, const std::string &passphrase) {

    try {
        s2k(passphrase);

        std::string version;
        std::string userkey;
        std::string serverkey;
        CK::GCMCodec codec(ciphertext);
        codec.decrypt(vaultKey, coder::ByteArray(passphrase, false));
        coder::ByteArray idBytes;
        // Pre-conversion
        //codec >> idBytes >> genpass >> enclaveKey >> accountRandom
        //      >> userkey >> serverkey;
        codec >> version;
        if (version != CURRENT_VERSION) {
            throw VaultException("Not a valid user vault");
        }
        codec >> idBytes >> genpass >> enclaveKey >> contactKey
              >> accountRandom >> userkey >> serverkey;
        publicId = QString(idBytes.toHexString().c_str());

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

        // One time for vault conversion to version 1.0
        //CK::FortunaSecureRandom rnd;
        //contactKey.setLength(32);
        //rnd.nextBytes(contactKey);

        CK::GCMCodec codec;
        coder::ByteArray idBytes(publicId.toStdString(), true);
        codec << CURRENT_VERSION << idBytes << genpass << enclaveKey << contactKey
              << accountRandom << ustr.str() << sstr.str();
        codec.encrypt(vaultKey, coder::ByteArray(passphrase, false));
        encoded = codec.toArray();

    }
    catch (CK::EncodingException& e) {
        throw VaultException(e.what());
    }
}

void Vault::loadVault(const QString& account, const QString& passphrase) {

    accountName = account;
    QFile vault(vaultPath + "/" + accountName);
    if (vault.open(QIODevice::ReadOnly)) {
        QByteArray bytes = vault.readAll();
        vault.close();
        decodeVault(ByteCodec(bytes), StringCodec(passphrase));
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

void Vault::storeVault(const QString& passphrase) {

    encodeVault(StringCodec(passphrase));
    QFile vault(vaultPath + "/" + accountName);
    if (vault.open(QIODevice::WriteOnly)) {
        ByteCodec codec(encoded);
        vault.write(codec);

        vault.close();
    }
    else {
        throw VaultException("Vault file write error");
    }

}

}
