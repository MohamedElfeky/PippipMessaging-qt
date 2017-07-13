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
#include "Contact.h"
#include "ContactRequest.h"
#include <QObject>
#include <QRegularExpression>

namespace Ui {
    class ContactsDialog;
}

namespace Pippip {
    class ContactManager;
    class NicknameManager;
    class SessionState;
}

class QLineEdit;
class QComboBox;
class EmptyStringValidator;

class ContactHandler : public QObject {
        Q_OBJECT
    public:
        explicit ContactHandler(Ui::ContactsDialog *ui, Pippip::SessionState *state, QObject *parent = 0);
        ~ContactHandler() {}

    signals:

    public slots:
        void contactsLoaded();
        void contactRequested(const QString& name);

    private slots:
        void nicknameEdited();
        void puidEdited();
        void requestContact();
        void requestingSelected();
        void requestingSet(const QString& nick);

    public:
        void setContactManager(Pippip::ContactManager *manager) { contactManager = manager; }
        void setNicknames(const Pippip::NicknameList& nicks);

    private:
        void loadTable();

    private:
        Ui::ContactsDialog *ui;
        Pippip::SessionState *state;
        Pippip::ContactManager *contactManager;
        Pippip::NicknameList nicknames;
        Pippip::ContactList contacts;
        QComboBox *requestingComboBox;
        QLineEdit *nicknameLineEdit;
        QLineEdit *puidLineEdit;
        QRegularExpression nicknameRE;
        EmptyStringValidator *nicknameValidator;
        QRegularExpression puidRE;
        EmptyStringValidator *puidValidator;
        Pippip::ContactRequest working;

};

#endif // CONTACTHANDLER_H
