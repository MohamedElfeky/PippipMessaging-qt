/*
 * EntropyStream.cpp
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

#include "EntropyStream.h"
#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>
#include <QThread>
#include <CryptoKitty-C/random/FortunaGenerator.h>
#include <coder/ByteArray.h>

static const int BUCKETSIZE = 16384;
static const int BUCKETCOUNT = 8;

namespace Pippip {

EntropyStream::EntropyStream(QObject *parent)
: QObject(parent),
  run(false),
  generator(0),
  mutex(new QReadWriteLock) {
}

EntropyStream::~EntropyStream() {

    delete generator;
    delete mutex;

}

void EntropyStream::quit() {

    run = false;

}

void EntropyStream::readStream(coder::ByteArray& bytes, unsigned count) {

    while (buckets.size() * BUCKETSIZE < count) {
        QThread::msleep(100);
    }

    coder::ByteArray data;
    unsigned bytesOut = 0;
    QReadLocker locker(mutex);
    while (bytesOut < count) {
        unsigned toRead = count - bytesOut;
        data.setLength(std::min(toRead, buckets.front().available()));
        buckets.front().getBytes(data);
        bytesOut += toRead;
        bytes.append(data);
        if (buckets.front().available() == 0) {
            buckets.pop_front();
        }
    }

}

void EntropyStream::threadFunction() {

    generator = new CK::FortunaGenerator;
    generator->start();
    run = true;

    coder::ByteArray data;
    while (run) {
        QWriteLocker locker(mutex);
        while (buckets.size() < BUCKETCOUNT) {
            data.clear();
            generator->generateRandomData(data, BUCKETSIZE);
            buckets.push_back(coder::ByteStreamCodec(data));
        }
    }

    emit finished();

}

}
