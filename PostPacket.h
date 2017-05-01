#ifndef POSTPACKET_H
#define POSTPACKET_H

#include <QString>
#include <QJsonObject>

namespace Pippip {

class PostPacket {

    protected:
        PostPacket() {}

    public:
        virtual ~PostPacket() {}

    private:
        PostPacket(const PostPacket& other);
        PostPacket& operator =(const PostPacket& other);

    public:
        virtual QJsonObject *getJson() const=0;
        virtual const QString& getUrl() const=0;

};

}
#endif // POSTPACKET_H
