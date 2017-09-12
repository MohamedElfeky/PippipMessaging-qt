/*
 * DatabaseImpl.cpp
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

#include "DatabaseImpl.h"
#include "SQLLiteContactsDatabaseImpl.h"

namespace Pippip {

ContactsDatabase *DatabaseImpl::createContactsDatabase(const QString& account) {

    ContactsDatabase *impl = new SQLLiteContactsDatabaseImpl;
    impl->create(account);
    return impl;

}

ContactsDatabase *DatabaseImpl::openContactsDatabase(const QString& account) {

    ContactsDatabase *impl = new SQLLiteContactsDatabaseImpl;
    impl->open(account);
    return impl;

}

}
