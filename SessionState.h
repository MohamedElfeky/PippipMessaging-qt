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

namespace CK {
    class RSAPublicKey;
    class RSAPrivateKey;
}
namespace Pippip {

struct SessionState {
    enum StateValue { authenticated, established, failed, not_started, started };
    StateValue sessionState;
    uint32_t sessionId;
    uint32_t handlerKey;
    uint64_t authToken;
    coder::ByteArray publicId;
    coder::ByteArray genpass;
    coder::ByteArray enclaveKey;
    coder::ByteArray accountRandom;
    CK::RSAPrivateKey *userPrivateKey;
    CK::RSAPublicKey *userPublicKey;
    CK::RSAPublicKey *serverPublicKey;
};

}

#endif // SESSIONSTATE_H
