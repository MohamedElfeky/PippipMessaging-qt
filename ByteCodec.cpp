#include "ByteCodec.h"
#include <QString>

namespace Pippip {

ByteCodec::ByteCodec(const QByteArray& q)
: qtBytes(q),
  qt(true),
  byteSize(q.size()) {

    data = qtBytes.data();
    uint8_t *ubytes = reinterpret_cast<uint8_t*>(data);
    coderBytes = coder::ByteArray(ubytes, byteSize);

}

ByteCodec::ByteCodec(const coder::ByteArray& c)
: coderBytes(c),
  qt(false),
  byteSize(c.getLength()) {

    data = reinterpret_cast<char*>(coderBytes.asArray());
    qtBytes = QByteArray(data, byteSize);

}

ByteCodec::ByteCodec(const QString &str)
: qtBytes(str.toUtf8()),
  qt(true),
  byteSize(str.length()) {

    data = qtBytes.data();
    uint8_t *ubytes = reinterpret_cast<uint8_t*>(data);
    coderBytes = coder::ByteArray(ubytes, byteSize);

}

ByteCodec::~ByteCodec() {

    if (!qt) {
        delete[] data;
    }

}

}
