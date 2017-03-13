/*
 * EntropyStream.h
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

#ifndef ENTROPYSTREAM_H
#define ENTROPYSTREAM_H

#include <QObject>
#include <coder/ByteStreamCodec.h>
#include <deque>

class QMutex;

namespace CK {
    class FortunaGenerator;
}

namespace Pippip {

class EntropyStream : public QObject {
    Q_OBJECT

    public:
        explicit EntropyStream(QObject *parent = 0);
        ~EntropyStream();

    private:
        EntropyStream(const EntropyStream& other);
        EntropyStream& operator= (const EntropyStream& other);

    signals:
        void error(QString errStr);
        void finished();

    public slots:
        void quit();
        void threadFunction();

    public:
        void endStream();
        void readStream(coder::ByteArray& bytes, unsigned count);      // This may block.

    private:
        bool run;
        typedef std::deque<coder::ByteStreamCodec> BucketQueue;
        BucketQueue buckets;
        CK::FortunaGenerator *generator;
        QMutex *mutex;

};

}

#endif // ENTROPYSTREAM_H

