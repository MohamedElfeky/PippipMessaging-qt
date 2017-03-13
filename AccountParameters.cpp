/*
 * AccountParameters.cpp
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

#include "AccountParameters.h"
#include <CryptoKitty-C/keys/RSAPrivateCrtKey.h>
#include <CryptoKitty-C/keys/RSAPublicKey.h>

namespace Pippip {

AccountParameters::AccountParameters()
: serverPublicKey(0),
  userPrivateKey(0),
  userPublicKey(0) {
}

AccountParameters::~AccountParameters() {

    delete serverPublicKey;
    delete userPrivateKey;
    delete userPublicKey;

}

AccountParameters& AccountParameters::operator =(const AccountParameters& other) {

    accountRandom = other.accountRandom;
    additionalData = other.additionalData;
    enclaveKey = other.enclaveKey;
    genpass = other.genpass;
    publicId = other.publicId;

    if (serverPublicKey != 0) {
        serverPublicKey = new CK::RSAPublicKey(*other.serverPublicKey);
    }

    if (userPublicKey != 0) {
        userPublicKey = new CK::RSAPublicKey(*other.serverPublicKey);
    }

    if (userPrivateKey != 0) {
        CK::RSAPrivateCrtKey *prv = dynamic_cast<CK::RSAPrivateCrtKey*>(other.userPrivateKey);
        userPrivateKey = new CK::RSAPrivateCrtKey(*prv);
    }

    return *this;

}

}

