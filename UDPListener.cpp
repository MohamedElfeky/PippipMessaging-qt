/*
 * UDPListener.cpp
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

#include "UDPListener.h"
#include "EntropyStream.h"
#include <coder/Unsigned16.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <memory>

namespace Pippip {

static const short FORTUNAPORT = 16574;
static const char *LOCALHOST = "127.0.0.1";

UDPListener::UDPListener(EntropyStream *str, QObject *parent)
: QObject(parent),
  run (false),
  stream(str) {
}

UDPListener::~UDPListener() {
}

bool UDPListener::createSocket() {

    socket = ::socket(PF_INET, SOCK_DGRAM, 0);
    if (socket < 0) {
        std::ostringstream errstr;
        errstr << "Socket creation error: " << strerror(errno) << std::endl;
        emit error(QString(errstr.str().c_str()),true);
        return false;
    }

    int optval = 1;
    setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &optval , sizeof(int));

    sockaddr_in name;
    name.sin_family = AF_INET;
    name.sin_port = htons(FORTUNAPORT);
    hostent *hostinfo = gethostbyname(LOCALHOST);
    name.sin_addr = *(reinterpret_cast<in_addr*>(hostinfo->h_addr));

    int res = bind(socket, reinterpret_cast<sockaddr*>(&name), sizeof(name));
    if (res < 0) {
        std::ostringstream errstr;
        errstr << "Socket bind error: " << strerror(errno) << std::endl;
        emit error(QString(errstr.str().c_str()),true);
        return false;
    }

    return true;

}

void UDPListener::quit() {

    run = false;

}

void UDPListener::runListener() {

    std::unique_ptr<uint8_t[]> req(new uint8_t[2]);
    if (createSocket()) {
        run = true;
        timeval tv;
        fd_set read_fds;
        sockaddr_in addr;
        socklen_t saLength = sizeof(sockaddr_in);
        while (run) {
            tv.tv_sec = 1;
            tv.tv_usec = 0;
            FD_ZERO(&read_fds);
            FD_SET(socket, &read_fds);
            int selected = select(socket + 1, &read_fds, 0, 0, &tv);
            if (selected < 0) {
                std::ostringstream errstr;
                errstr << "Socket select error: " << strerror(errno) << std::endl;
                emit error(QString(errstr.str().c_str()), true);
                run = false;
            }
            else if (selected > 0 && FD_ISSET(socket, &read_fds)) {
                int bytesRead = recvfrom(socket, req.get(), 2, 0,
                                        reinterpret_cast<sockaddr*>(&addr), &saLength);
                if (bytesRead == 2) {
                    coder::Unsigned16 u16(coder::ByteArray(req.get(), 2), coder::bigendian);
                    // std::cout << "Handling a request for " << u16.getValue() << " bytes" << std::endl;
                    coder::ByteArray rdata;
                    stream->readStream(rdata, u16.getValue());
                    std::unique_ptr<uint8_t[]> outbuf(rdata.asArray());
                    int res = sendto(socket, outbuf.get(), u16.getValue(), 0,
                                                    reinterpret_cast<sockaddr*>(&addr), saLength);
                    if (res < 0) {
                        std::ostringstream errstr;
                        errstr << "Socket send error: " << strerror(errno) << std::endl;
                        emit error(QString(errstr.str().c_str()), false);
                    }
                }
            }
        }
        close(socket);
    }

    emit finished();

}

}
