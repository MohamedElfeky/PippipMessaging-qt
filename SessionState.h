/*
 * SessionState.h
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

#ifndef SESSIONSTATE_H
#define SESSIONSTATE_H

#include <coder/ByteArray.h>
#include <CryptoKitty-C/keys/RSAPrivateKey.h>
#include <CryptoKitty-C/keys/RSAPublicKey.h>
#include <QString>

namespace CK {
    class RSAPublicKey;
    class RSAPrivateKey;
}
namespace Pippip {

class SessionState {

    public:
        SessionState() : serverPublicKey(0), userPrivateKey(0), userPublicKey(0) {}
        ~SessionState() { delete serverPublicKey;
                          delete userPrivateKey;
                          delete userPublicKey; }

    public:
        enum StateValue { authenticated, established, failed, not_started, started };
        StateValue sessionState;
        uint32_t sessionId;
        uint64_t authToken;
        QString publicId;
        coder::ByteArray genpass;
        coder::ByteArray enclaveKey;
        coder::ByteArray contactKey;
        coder::ByteArray authData;
        coder::ByteArray accountRandom;
        coder::ByteArray serverAuthRandom;
        coder::ByteArray clientAuthRandom;
        CK::RSAPublicKey *serverPublicKey;
        CK::RSAPrivateKey *userPrivateKey;
        CK::RSAPublicKey *userPublicKey;

};

}

#endif // SESSIONSTATE_H
