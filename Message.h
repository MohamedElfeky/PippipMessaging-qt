#ifndef MESSAGE_H
#define MESSAGE_H

#include <QtGlobal>
#include <QString>
#include <QByteArray>
#include <vector>

namespace Pippip {

static const QString MESSAGE_VERSION = "1.0";

struct Message {
    QString version;
    qint64 messageId;
    qint64 timestamp;
    int sequence;
    QString sender;         // Nickname
    QString senderId;
    QString recipient;      // Nickname
    QString recipientId;    // Public ID
    int keyIndex;
    QByteArray message;
};

typedef std::vector<Message> MessageList;

}

#endif // MESSAGE_H
