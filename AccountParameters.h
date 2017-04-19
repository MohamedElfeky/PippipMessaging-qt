/*
 * AccountParameters.h
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

#ifndef ACCOUNTPARAMETERS_H
#define ACCOUNTPARAMETERS_H

#include <coder/ByteArray.h>
#include <string>

namespace CK {
    class RSAPrivateKey;
    class RSAPublicKey;
}

namespace Pippip {

class AccountParameters {

    public:
        AccountParameters();
        virtual ~AccountParameters();
        AccountParameters& operator =(const AccountParameters& other);

    private:
        AccountParameters(const AccountParameters& other);

    public:
        const coder::ByteArray& getEnclaveKey() const { return enclaveKey; }
        const coder::ByteArray& getGenpass() const { return genpass; }
        const coder::ByteArray& getPublicId() const { return publicId; }
        CK::RSAPublicKey *getServerPublicKey() const { return serverPublicKey; }
        CK::RSAPrivateKey *getUserPrivateKey() const { return userPrivateKey; }
        CK::RSAPublicKey *getUserPublicKey() const { return userPublicKey; }
        void setAccountRandom(const coder::ByteArray& rnd) { accountRandom = rnd; }
        void setServerPublicKey(const std::string& pem);

    protected:
        // Account random number. Used to build private ID
        coder::ByteArray accountRandom;
        // Additional data. Additional authentication data for GCM encryption.
        coder::ByteArray additionalData;
        // Enclave symmetric encryption key.
        coder::ByteArray enclaveKey;
        // Generated password. ZKP password.
        coder::ByteArray genpass;
        // Public ID.
        coder::ByteArray publicId;
        // Server's RSA public key. Used for authentication only.
        CK::RSAPublicKey *serverPublicKey;
        // User's RSA private key. Used for authentication only.
        CK::RSAPrivateKey *userPrivateKey;
        // User's RSA public key. Used for authentication only.
        CK::RSAPublicKey *userPublicKey;

};

}

#endif // ACCOUNTPARAMETERS_H

