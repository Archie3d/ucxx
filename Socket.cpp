#ifdef WIN32
#   include <winsock2.h>
#endif // WIN32

#include "Socket.h"

#if defined(WIN32) && defined(_MSC_VER)
#   pragma comment(lib,"ws2_32.lib")
#endif

namespace ucxx {

#ifdef WIN32
bool Socket::s_winSockInitialized = false;

bool initializeWinSock()
{
    WSADATA wsa;
    return WSAStartup(MAKEWORD(2, 2), &wsa) != 0;
}

//----------------------------------------------------------
// WSA static initialized
//----------------------------------------------------------

class WSAInitializer {
public:
    WSAInitializer() {
        Socket::initialize();
    }

    ~WSAInitializer() {
        Socket::cleanup();
    }
};

// Make sure WSA is started up and cleaned automatically
static WSAInitializer s_wsaInitializer;

#endif // WIN32


//----------------------------------------------------------
// class Socket implementation
//----------------------------------------------------------

Socket::Socket(Socket::Protocol protocol)
    : m_protocol(protocol),
      m_error(false)
{
    Socket::initialize();
}

Socket::~Socket()
{
}

size_t Socket::write(const ByteArray &byteArray)
{
    return writeBuffer(byteArray.constData(), byteArray.size());
}

ByteArray Socket::read(size_t size)
{
    char *pBuffer = new char[size];
    size_t bytes = readBuffer(pBuffer, size);
    ByteArray ba(pBuffer, bytes);
    delete[] pBuffer;
    return ba;
}

void Socket::setError(const std::string &text)
{
    m_error = true;
    m_errorText = text;
}

void Socket::initialize()
{
#ifdef WIN32
    if (!s_winSockInitialized) {
        s_winSockInitialized = initializeWinSock();
    }
#endif // WIN32
}

void Socket::cleanup()
{
#ifdef WIN32
    if (s_winSockInitialized) {
        WSACleanup();
    }
#endif // WIN32
}

} // namespace ucxx
