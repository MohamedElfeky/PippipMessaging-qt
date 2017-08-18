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

#include "Nickname.h"
#include <QObject>
#include <QRegularExpression>

namespace Ui {
    class NicknamesDialog;
}

namespace Pippip {
    class Nicknames;
    class SessionState;
    class EnclaveRequestTask;
}

class QLineEdit;
class EmptyStringValidator;

class WhitelistHandler : public QObject
{
        Q_OBJECT
    public:
        explicit WhitelistHandler(Ui::NicknamesDialog *ui, QObject *parent = 0);
        ~WhitelistHandler() {}

    private slots:
        void addEntry();
        void deleteEntry();
        void editEntry(int row, int column);
        void nicknameEdited();
        void pasteValue();
        void puidEdited();
        void requestComplete(Pippip::EnclaveRequestTask *task);
        void requestFailed(Pippip::EnclaveRequestTask *task);

    public:
        int getSelectedRow() const { return selectedRow; }
        void loadTable(int start = 0);
        void setCurrentIndex(unsigned index);
        void setCurrentNickname(unsigned index) { currentNickname = index; }
        void setSelectedRow(int row) { selectedRow = row; }
        void setSessionState(Pippip::SessionState *st) { state = st; }

    private:
        bool newItem;
        int selectedRow;
        unsigned currentNickname;
        Ui::NicknamesDialog *ui;
        Pippip::SessionState *state;
        Pippip::Entity working;
        QLineEdit *nicknameLineEdit;
        QLineEdit *puidLineEdit;
        EmptyStringValidator *nicknameValidator;
        EmptyStringValidator *puidValidator;
        QRegularExpression nicknameRE;
        QRegularExpression puidRE;

};

#endif // WHITELISTHANDLER_H
