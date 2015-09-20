#ifndef UCXX_TCPSOCKET_H
#define UCXX_TCPSOCKET_H

//
// Platform-specific TCP socket implementation
//

#ifdef WIN32
#   include <winsock2.h>

#   define SOCKET_TYPE  SOCKET
#else
#   define SOCKET_TYPE  int
#endif

#include <string>
#include "Mutex.h"
#include "Socket.h"

namespace ucxx {

/**
 * @brief TCP-socket.
 * This implements a generic TCP socket of client-type.
 */
class TcpSocket : public Socket
{
    friend class TcpServer;
public:

    TcpSocket();
    ~TcpSocket();

    size_t readBuffer(char *pBuffer, size_t size);
    size_t writeBuffer(const char *pData, size_t size);
    size_t available();
    void close(Stream stream = Stream_All);
    bool isConnected() const;

    bool connectToHost(const std::string &hostName, unsigned short port);

    std::string peerAddr() const;
    unsigned short peerPort() const;

    /**
     * Read from socket till EOL or socket error.
     * @return Line read.
     */
    std::string readLine(size_t size = 256);

    static std::string hostNameToIp(const std::string &hostName);

private:
    TcpSocket(SOCKET_TYPE s);
    SOCKET_TYPE nativeSocket() const;
    void setPeerAddr(const std::string &addr);
    void setPeerPort(unsigned short p);

    SOCKET_TYPE m_socket;
    std::string m_peerAddr;
    unsigned short m_peerPort;
    mutable Mutex m_mutex;  ///< Protective mutex.
};

} // namespace ucxx

#endif // UCXX_TCPSOCKET_H
