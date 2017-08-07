/*
 * TaskNames.cpp
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

#include "Constants.h"

// Task names
const QString Constants::ADD_NICKNAME = "Add Nickname";
const QString Constants::DELETE_NICKNAME = "Delete Nickname";
const QString Constants::LOAD_NICKNAMES = "Load Nicknames";
const QString Constants::UPDATE_CONTACT_POLICY = "Update Contact Policy";
const QString Constants::UPDATE_WHITELIST = "Update Friends List";

// Policy names
const QString Constants::PUBLIC_POLICY = "Public";
const QString Constants::FRIENDS_ONLY_POLICY = "FriendsOnly";
const QString Constants::FRIENDS_OF_FRIENDS_POLICY = "FriendsOfFriends";
const QString Constants::POLICIES[] = { PUBLIC_POLICY, FRIENDS_ONLY_POLICY, FRIENDS_OF_FRIENDS_POLICY };

// RTF graphics display text
const QString Constants::CHECK_ICON = "<img src=:/mainwindow/images/check.ico width=16 height=16> ";
const QString Constants::CAUTION_ICON = "<img src=:/mainwindow/images/check.ico width=16 height=16> ";
const QString Constants::INFO_ICON = "<img src=:/mainwindow/images/info.ico width=18 height=18> ";
const QString Constants::REDBAR_ICON = "<img src=:/mainwindow/images/redbar.ico width=16 height=16> ";
const QString Constants::REDX_ICON = "<img src=:/mainwindow/images/redx.ico width=16 height=16> ";
