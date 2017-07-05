/*
 * WhitelistHandler.h
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

#ifndef WHITELISTHANDLER_H
#define WHITELISTHANDLER_H

#include "Entity.h"
#include <QObject>
#include <QRegularExpression>

namespace Ui {
    class NicknamesDialog;
}

namespace Pippip {
    class NicknameManager;
}

class QLineEdit;
class QRegularExpressionValidator;

class WhitelistHandler : public QObject
{
        Q_OBJECT
    public:
        explicit WhitelistHandler(Ui::NicknamesDialog *ui, QObject *parent = 0);
        ~WhitelistHandler() {}

    signals:
        void whitelistChanged(const Pippip::EntityList& list);

    public slots:

    public:
        void setWhitelist(const Pippip::EntityList& whitelist);

    private:
        void loadTable();

    private:
        Ui::NicknamesDialog *ui;
        Pippip::EntityList whitelist;
        QLineEdit *nicknameLineEdit;
        QLineEdit *puidLineEdit;
        QRegularExpressionValidator *nicknameValidator;
        QRegularExpressionValidator *puidValidator;
        QRegularExpression nicknameRE;
        QRegularExpression puidRE;

};

#endif // WHITELISTHANDLER_H
