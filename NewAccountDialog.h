/*
 * NewAccountDialog.h
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

#ifndef NEWACCOUNTDIALOG_H
#define NEWACCOUNTDIALOG_H

namespace Pippip {
    class NewAccountCreator;
}

#include <QDialog>

namespace Ui {
class NewAccountDialog;
}

namespace Pippip {
    class SessionState;
}

class NewAccountDialog : public QDialog {
    Q_OBJECT

    public:
        explicit NewAccountDialog(Pippip::SessionState *session, QWidget *parent = 0);
        ~NewAccountDialog();

    public:
        void accept();

    public slots:
        void accountComplete();
        void doHelp();

    protected:
        void showEvent(QShowEvent *);

    private:
        void accountNameAlert();
        bool passphraseAlert();

    private:
        Ui::NewAccountDialog *ui;
        Pippip::SessionState *session;
        Pippip::NewAccountCreator *creator;

};

#endif // NEWACCOUNTDIALOG_H
