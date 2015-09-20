#ifndef UCXX_TCPSERVER_H
#define UCXX_TCPSERVER_H

//
// Platform-specific TCP server socket
//

#include "Mutex.h"
#include "TcpSocket.h"

namespace ucxx {

/**
 * @brief TCP server.
 * TCP server listens on a given ports and accepts incoming connections.
 */
class TcpServer
{
public:

    TcpServer();
    virtual ~TcpServer();
    bool listen(unsigned short port);
    bool listen(const std::string &hostName, unsigned short port);

    /**
     * Accept an incoming connection.
     * This method may block waiting for the next connection.
     * @return Pointer to an accepted client socket, or null no connections or error detected.
     */
    TcpSocket* accept();
    void close();

    void setMaxPendingConnections(int c) { m_maxPendingConnections = c; }
    int maxPendingConnections() const { return m_maxPendingConnections; }
    bool isListening() const;

private:

    SOCKET_TYPE nativeSocket() const;

    mutable Mutex m_mutex; ///< Protective mutex.
    TcpSocket *m_pServerSocket;
    int m_maxPendingConnections;
};

} // namesapce ucxx

#endif // UCXX_TCPSERVER_H
