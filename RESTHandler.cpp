/*
 * RESTHandler.cpp
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

#include "RESTHandler.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>

RESTHandler::RESTHandler(QObject *parent)
: QObject(parent),
  success(false) {

    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(managerFinished(QNetworkReply*)));

}

RESTHandler::~RESTHandler() {

    delete manager;

}

void RESTHandler::doGet(const QString& url) {

    manager->get(QNetworkRequest(QUrl(url)));

}

void RESTHandler::doPost(const QString& url, const QJsonObject& jsonObj) {

    QJsonDocument doc(jsonObj);
    QByteArray json("jason=");
    json.push_back(doc.toJson());
    manager->post(QNetworkRequest(QUrl(url)), json);

}

void RESTHandler::managerFinished(QNetworkReply *reply) {

    QNetworkReply::NetworkError errorType = reply->error();
    if (errorType == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        response = doc.object();
        success = true;
    }
    else {
        error = reply->errorString();
    }

    emit requestComplete(this);

}
