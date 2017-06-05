#include "LogoutRequest.h"
#include "SessionState.h"
#include <QJsonObject>

namespace Pippip {

static const QString LOGOUT_URL = "https://pippip.io:2880/io.pippip.rest/LogoutRequest";

LogoutRequest::LogoutRequest(SessionState *st)
: state(st) {

}

QJsonObject *LogoutRequest::getJson() const {

    QJsonObject *json  = new QJsonObject;
    (*json)["sessionId"] = static_cast<int>(state->sessionId);
    (*json)["authToken"] = static_cast<double>(state->authToken);

    return json;

}

const QString& LogoutRequest::getUrl() const {

    return LOGOUT_URL;

}

} // namespace Pippip
