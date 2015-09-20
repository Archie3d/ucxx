#ifndef WIN32
#   include <unistd.h>
#   include <netdb.h>
#   include <netinet/in.h>
#   include <sys/ioctl.h>
#   include <sys/socket.h>
#   include <arpa/inet.h>

#else
    typedef int socklen_t;
#endif

#include <stdlib.h>
#include <assert.h>
#include "TcpServer.h"

namespace ucxx {

TcpServer::TcpServer()
{
    // Be sure the sockets library is initialized.
    Socket::initialize();

    m_pServerSocket = 0;
    m_maxPendingConnections = 0;
}

TcpServer::~TcpServer()
{
    delete m_pServerSocket;
}

bool TcpServer::listen(unsigned short port)
{
    return listen(std::string(), port);
}

bool TcpServer::listen(const std::string &hostName, unsigned short port)
{
    if (isListening()) {
        // Already listening
        return false;
    }

    SOCKET_TYPE sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return false;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = (hostName.empty() ? INADDR_ANY : inet_addr(hostName.c_str()));
    server.sin_port = htons(port);
    if (::bind(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        // Unable to bind
#ifdef WIN32
        closesocket(sock);
#else
        ::close(sock);
#endif
        return false;
    }

    int backlog = m_maxPendingConnections > 0 ? m_maxPendingConnections : SOMAXCONN;
    if (::listen(sock, backlog) != 0) {
        // Error listening
#ifdef WIN32
        closesocket(sock);
#else
        ::close(sock);
#endif
        return false;
    }

    MutexLocker locker(&m_mutex);
    m_pServerSocket = new TcpSocket(sock);
    return true;
}

TcpSocket* TcpServer::accept()
{
    TcpSocket *pSocket = 0;
    if (!isListening()) {
        // Not listening, cannot accept connections
        return pSocket;
    }

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    fd_set fdset;

    SOCKET_TYPE sock = nativeSocket();
    if (sock < 0) {
        return 0;
    }

    FD_ZERO(&fdset);
    FD_SET(sock, &fdset);

    int n = select(sock + 1, &fdset, NULL, NULL, &timeout);
    if (n <= 0) {
        return 0;
    }

    struct sockaddr_in client;
    socklen_t c = sizeof(struct sockaddr_in);
    SOCKET_TYPE clientSock = ::accept(sock,
                                      (struct sockaddr*)&client,
                                      &c);
    if (clientSock < 0) {
        // Accept failed with error WSAGetLastError();
        return pSocket;
    }

    char *clientIp = inet_ntoa(client.sin_addr);
    unsigned short clientPort = ntohs(client.sin_port);

    pSocket = new TcpSocket(clientSock);
    pSocket->setPeerAddr(std::string(clientIp));
    pSocket->setPeerPort(clientPort);

    return pSocket;
}

void TcpServer::close()
{
    MutexLocker locker(&m_mutex);
    if (m_pServerSocket != 0) {
        m_pServerSocket->close();
        delete m_pServerSocket;
        m_pServerSocket = 0;
    }
}

bool TcpServer::isListening() const
{
    MutexLocker locker(&m_mutex);
    return m_pServerSocket != 0;
}

SOCKET_TYPE TcpServer::nativeSocket() const
{
    MutexLocker locker(&m_mutex);
    if (!m_pServerSocket) {
        return -1;
    }
    return m_pServerSocket->nativeSocket();
}

} // namespace ucxx
