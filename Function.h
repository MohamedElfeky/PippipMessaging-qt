#ifndef FUNCTION_H
#define FUNCTION_H

#include <iostream>

namespace coder {
    class ByteStreamCodec;
}

namespace Pippip {
    struct Contact;
}

class QString;

coder::ByteStreamCodec& operator >> (coder::ByteStreamCodec& in, QString& str);
coder::ByteStreamCodec& operator << (coder::ByteStreamCodec& out, const QString& str);

coder::ByteStreamCodec& operator >> (coder::ByteStreamCodec& in, Pippip::Contact& contact);
coder::ByteStreamCodec& operator << (coder::ByteStreamCodec& out, const Pippip::Contact& contact);

std::ostream& operator << (std::ostream& out, const QString& str);
std::istream& operator >> (std::istream& in, QString& str);

#endif // FUNCTION_H
