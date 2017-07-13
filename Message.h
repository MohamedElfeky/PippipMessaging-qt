#ifndef MESSAGE_H
#define MESSAGE_H

#include <QtGlobal>
#include <QString>
#include <QByteArray>
#include <vector>

namespace Pippip {

struct Message {
    QString version;
    qint64 messageId;
    qint64 timestamp;
    QString sender;         // Nickname
    QString recipient;      // Nickname
    QString recipientId;    // Public ID
    int keyIndex;
    QByteArray message;
};

typedef std::vector<Message> MessageList;

}

#endif // MESSAGE_H
