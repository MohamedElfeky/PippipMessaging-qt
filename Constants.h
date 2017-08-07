/*
 * TaskNames.h
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

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

class Constants
{
    private:
        Constants() {}

    public:
        // Task names
        static const QString ADD_NICKNAME;
        static const QString DELETE_NICKNAME;
        static const QString LOAD_NICKNAMES;
        static const QString UPDATE_CONTACT_POLICY;
        static const QString UPDATE_WHITELIST;

        // Policy names
        static const QString PUBLIC_POLICY;
        static const QString FRIENDS_ONLY_POLICY;
        static const QString FRIENDS_OF_FRIENDS_POLICY;
        static const QString POLICIES[];

        // RTF graphics display text
        static const QString CAUTION_ICON;
        static const QString CHECK_ICON;
        static const QString INFO_ICON;
        static const QString REDBAR_ICON;
        static const QString REDX_ICON;

};

#endif // CONSTANTS_H
