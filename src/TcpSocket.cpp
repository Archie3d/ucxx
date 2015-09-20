#ifndef WIN32
#   include <unistd.h>
#   include <netdb.h>
#   include <netinet/in.h>
#   include <sys/ioctl.h>
#   include <sys/socket.h>
#   include <arpa/inet.h>

#define SD_BOTH     SHUT_RDWR
#define SD_RECEIVE  SHUT_RD
#define SD_SEND     SHUT_WR

#endif

#include <iostream>
#include <assert.h>
#include "TcpSocket.h"

namespace ucxx {

TcpSocket::TcpSocket()
    : Socket(Socket::Protocol_Tcp)
{
#ifdef WIN32
    m_socket = INVALID_SOCKET;
#else
        m_socket = -1;
#endif
}

TcpSocket::TcpSocket(SOCKET_TYPE s)
    : Socket(Socket::Protocol_Tcp)
{
    m_socket = s;


    // Configure timeout on socket recv operation.
    // If not, the socket will block the receiver thread
    // when waiting for incoming data.
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval));
}

TcpSocket::~TcpSocket()
{
    TcpSocket::close();
#ifdef WIN32
    closesocket(m_socket);
#else
    ::close(m_socket);
#endif
}

SOCKET_TYPE TcpSocket::nativeSocket() const
{
    MutexLocker locker(&m_mutex);
    return m_socket;
}

size_t TcpSocket::readBuffer(char *pBuffer, size_t size)
{
    if (!isConnected()) {
        return 0;
    }

    SOCKET_TYPE sock = nativeSocket();
    int s = recv(sock, pBuffer, size, 0);
    if (s < 0) {
        // Socket error
        setError("Unable to read data");
        return 0;
    }
    return (size_t)s;
}

size_t TcpSocket::writeBuffer(const char *pData, size_t size)
{
    if (!isConnected()) {
        return 0;
    }

    SOCKET_TYPE sock = nativeSocket();
    int s = send(sock, pData, size, 0);
    if (s < 0) {
        // Error sending data
        setError("Unable to send data");
        return 0;
    }
    return (size_t)s;
}

size_t TcpSocket::available()
{
    if (!isConnected()) {
        return 0;
    }

    SOCKET_TYPE sock = nativeSocket();
    u_long bytes = 0;
#ifdef WIN32
    ioctlsocket(sock, FIONREAD, &bytes);
#else
    ioctl(sock, FIONREAD, &bytes);
#endif
    return (size_t)bytes;
}

void TcpSocket::close(Stream stream)
{
    SOCKET_TYPE sock = nativeSocket();
    int s = SD_BOTH;
    switch (stream) {
    case Stream_Incoming:
        s = SD_RECEIVE;
        break;
    case Stream_Outgoing:
        s = SD_SEND;
        break;
    default:
        break;
    }
    shutdown(sock, s);
#ifdef WIN32
    closesocket(sock);
#else
    ::close(sock);
#endif
}

bool TcpSocket::isConnected() const
{
    MutexLocker locker(&m_mutex);
    return m_socket > 0;
}

bool TcpSocket::connectToHost(const std::string &hostName, unsigned short port)
{
    if (isConnected()) {
        close();
    }

    SOCKET_TYPE sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        setError("Unable to create a socket");
        return false;
    }

    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(hostName.c_str());
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) >= 0) {
        MutexLocker locker(&m_mutex);
        m_socket = sock;
        return true;
    }

    return false;
}

std::string TcpSocket::peerAddr() const
{
    MutexLocker locker(&m_mutex);
    return m_peerAddr;
}

unsigned short TcpSocket::peerPort() const
{
    MutexLocker locker(&m_mutex);
    return m_peerPort;
}

std::string TcpSocket::readLine(size_t size)
{
    std::string line;
    if (!isConnected()) {
        return line;
    }

    SOCKET_TYPE sock = nativeSocket();
    size_t length = 0;
    char c = '\0';
    do {
        int read = recv(sock, &c, 1, 0);
        if (read < 0) {
            setError("Unable to read data");
            return line;
        } else if (read == 1 && c != '\n' && c != '\r') {
            line += c;
            length += 1;
        }
    } while (c != '\n' && (length < size));

    return line;
}

std::string TcpSocket::hostNameToIp(const std::string &hostName)
{
    struct hostent *he;
    struct in_addr **addr_list;

    size_t length = hostName.length();
    char *host = new char[length + 1];
    memcpy(host, hostName.c_str(), length);
    host[length] = '\0';

    he = gethostbyname(host);

    delete[] host;

    std::string ip;
    if (he != NULL) {
        addr_list = (struct in_addr**)he->h_addr_list;
        for (int i = 0; addr_list[i] != NULL; i++) {
            ip += inet_ntoa(*addr_list[i]);
            return ip;
        }
    }

    return ip;
}

void TcpSocket::setPeerAddr(const std::string &addr)
{
    MutexLocker locker(&m_mutex);
    m_peerAddr = addr;
}

void TcpSocket::setPeerPort(unsigned short port)
{
    MutexLocker locker(&m_mutex);
    m_peerPort = port;
}

} // namespace ucxx
