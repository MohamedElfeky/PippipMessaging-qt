/*
 * RESTHandler.h
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

#ifndef RESTHANDLER_H
#define RESTHANDLER_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QList>
#include <QSslError>

class QNetworkAccessManager;
class QNetworkReply;

class RESTHandler : public QObject {
    Q_OBJECT

    public:
        explicit RESTHandler(QObject *parent = 0);
        ~RESTHandler();

    public:
        void doGet(const QString& url);
        void doPost(const QString& url, const QJsonObject& json);
        const QString& getError() const { return error; }
        const QJsonObject&  getResponse() const { return response; }
        bool successful() const { return success; }

    signals:
        void requestComplete(RESTHandler *handler);
        void requestFailed(RESTHandler *handler);

    private slots:
        void managerFinished(QNetworkReply* reply);
        void managerSSLErrors(QNetworkReply* replay, QList<QSslError> errors);

    private:
        QNetworkAccessManager *manager;

        QJsonObject response;
        QString error;
        bool success;

};

#endif // RESTHANDLER_H
