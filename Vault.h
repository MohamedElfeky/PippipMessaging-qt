/*
 * Vault.h
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

#ifndef VAULT_H
#define VAULT_H

#include "SessionState.h"
#include <coder/ByteArray.h>
#include <string>

namespace Pippip {

class Vault : public SessionState {

    public:
        Vault();
        ~Vault();
        Vault(const SessionState& state);

    private:
        Vault(const Vault& other);
        Vault& operator&(const Vault& other);

    private:
        coder::ByteArray vaultKey;

};

}

#endif // VAULT_H
