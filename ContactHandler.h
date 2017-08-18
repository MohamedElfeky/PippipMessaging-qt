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
#include <QStringList>

namespace Ui {
    class ContactsDialog;
}

namespace Pippip {
    class ContactsDatabase;
    class SessionState;
    struct ContactRequestIn;
    class EnclaveRequestTask;
}

class QLineEdit;
class QComboBox;
class QRegularExpressionValidator;

class ContactHandler : public QObject {
        Q_OBJECT
    public:
        explicit ContactHandler(Ui::ContactsDialog *ui, Pippip::SessionState *state, QObject *parent = 0);
        ~ContactHandler() {}

    public slots:
        void requestComplete(Pippip::EnclaveRequestTask *task);
        void requestFailed(Pippip::EnclaveRequestTask *task);
        void updateStatus(const QString& icon, const QString& status);

    private slots:
        void queryContact();
        void requestedIdEdited();
        void requestContact();
        void idTypeSelected(Qt::Key);
        void requestingIdSelected(Qt::Key);

    public:
        void checkButtons();
        void loadTable(int startingRow = 0);

    private:
        int columnGeometry() const;

    private:
        Ui::ContactsDialog *ui;
        Pippip::SessionState *state;
        Pippip::ContactsDatabase *contactsDatabase;
        // Editing widgets
        QStringList requestHeadings;
        QStringList contactHeadings;
        QComboBox *requestingIdComboBox;
        QComboBox *idTypeComboBox;
        QLineEdit *requestedIdLineEdit;
        QRegularExpression nicknameRE;
        QRegularExpressionValidator *nicknameValidator;
        QRegularExpression puidRE;
        QRegularExpressionValidator *puidValidator;
        // Contact request variables
        Pippip::ContactRequestOut workingRequest;
        QString requestingType;     // P or N
        QString requestedType;      // P or N

};

#endif // CONTACTHANDLER_H
