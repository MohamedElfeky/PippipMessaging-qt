/*
 * ContactHandler.h
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

#ifndef CONTACTHANDLER_H
#define CONTACTHANDLER_H

#include "Nickname.h"
#include <QObject>
#include <QRegularExpression>

namespace Ui {
    class ContactsDialog;
}

namespace Pippip {
    class ContactManager;
    class NicknameManager;
}

class QLineEdit;
class QComboBox;
class EmptyStringValidator;

class ContactHandler : public QObject {
        Q_OBJECT
    public:
        explicit ContactHandler(Ui::ContactsDialog *ui, QObject *parent = 0);
        ~ContactHandler() {}

    signals:

    private slots:
        void nicknameEdited();
        void nicknameSet(const QString& nick);
        void nicknameSelected();
        void puidEdited();
        void requestContact();

    public:
        void setContactManager(Pippip::ContactManager *manager) { contactManager = manager; }
        void setNicknames(const Pippip::NicknameList& nicks) { nicknames = nicks; }

    private:
        void loadTable();

    private:
        Ui::ContactsDialog *ui;
        Pippip::ContactManager *contactManager;
        Pippip::NicknameList nicknames;
        QComboBox *nicknameComboBox;
        QLineEdit *nicknameLineEdit;
        QLineEdit *puidLineEdit;
        QRegularExpression nicknameRE;
        EmptyStringValidator *nicknameValidator;
        QRegularExpression puidRE;
        EmptyStringValidator *puidValidator;

};

#endif // CONTACTHANDLER_H
