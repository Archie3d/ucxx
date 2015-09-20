#ifndef UCXX_SOCKET_H
#define UCXX_SOCKET_H

//
// Abstract socket implementation (platform-specific)
//

#include <stdlib.h>
#include <string>
#include "ByteArray.h"

namespace ucxx {

/**
 * @brief Abstract socket implementation.
 */
class Socket
{
public:

    /// Socket protocol.
    enum Protocol {
        Protocol_Tcp,
        Protocol_Udp
    };

    /// Socket data stream direction.
    enum Stream {
        Stream_Incoming = 1,
        Stream_Outgoing = 2,
        Stream_All = Stream_Incoming | Stream_Outgoing
    };

    Socket(Protocol protocol);
    virtual ~Socket();
    virtual size_t writeBuffer(const char *pData, size_t size) = 0;
    virtual size_t readBuffer(char *pBuffer, size_t size) = 0;
    virtual size_t available() = 0;
    virtual void close(Stream stream = Stream_All) = 0;
    virtual bool isConnected() const = 0;

    // Read/write methods accepting ByteArray and
    // delegating the I/O to readBuffer/writeBuffer methods.
    virtual size_t write(const ByteArray &byteArray);
    virtual ByteArray read(size_t size);

    Protocol protocol() const { return m_protocol; }
    bool isError() const { return m_error; }
    std::string errorText() const { return m_errorText; }

    static void initialize();
    static void cleanup();

protected:

    void setError(const std::string &text);

private:

    Socket() {}
    Socket(const Socket &s) {}
    Socket& operator =(const Socket &s) { return *this; }

    Protocol m_protocol;
    bool m_error;               ///< Socket error flag.
    std::string m_errorText;    ///< Socket error text.
#ifdef WIN32
    static bool s_winSockInitialized;
#endif
};

} // namespace ucxx

#endif // UCXX_SOCKET_H
