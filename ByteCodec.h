#ifndef BYTECODEC_H
#define BYTECODEC_H

#include <QByteArray>
#include <coder/ByteArray.h>

namespace Pippip {

class ByteCodec {

    public:
        ByteCodec(const QByteArray& qtBytes);
        ByteCodec(const coder::ByteArray& coderBytes);
        ByteCodec(const QString& str);
        ~ByteCodec();

    public:
        explicit operator const char* () const { return data; }
        operator const QByteArray& () const { return qtBytes; }
        operator const coder::ByteArray& () const { return coderBytes; }

    public:
        const coder::ByteArray& getCoderBytes() const { return coderBytes; }
        const QByteArray& getQtBytes() const { return qtBytes; }
        size_t size() const { return byteSize; }

    private:
        QByteArray qtBytes;
        coder::ByteArray coderBytes;
        bool qt;
        char* data;
        size_t byteSize;

};

}

#endif // BYTECODEC_H
