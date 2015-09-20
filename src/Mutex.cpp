#ifdef WIN32
#   include <stdarg.h>
#   include <windef.h>
#   include <winbase.h>
#else
#   include <pthread.h>
#endif // WIN32

#include <assert.h>
#include "Mutex.h"

namespace ucxx {

Mutex::Mutex()
{
#ifdef WIN32
    m_mutex = 0;
    m_mutex = CreateMutex(0, FALSE, 0);
    assert(m_mutex);
    assert(m_mutex != INVALID_HANDLE_VALUE);

#else

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
#if defined(__CYGWIN__) || defined(__APPLE__)
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
#else
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
#endif
    pthread_mutex_init(&m_mutex, &attr);
    pthread_mutexattr_destroy(&attr);
#endif
}

Mutex::~Mutex()
{
#ifdef WIN32
    CloseHandle(m_mutex);
#else
    pthread_mutex_destroy(&m_mutex);
#endif
}

void Mutex::lock()
{
#ifdef WIN32
    DWORD dwWaitResult;
    dwWaitResult = WaitForSingleObject(m_mutex, INFINITE);
    assert(dwWaitResult == WAIT_OBJECT_0);
#else
    pthread_mutex_lock(&m_mutex);
#endif
}

void Mutex::unlock()
{
#ifdef WIN32
    ReleaseMutex(m_mutex);
#else
    pthread_mutex_unlock(&m_mutex);
#endif
}

bool Mutex::tryLock()
{
#ifdef WIN32
    DWORD dwWaitResult = WaitForSingleObject(m_mutex, 0);
    if (dwWaitResult == WAIT_OBJECT_0) {
        // The state of the mutex is signaled (owned by current thread now)
        return true;
    }
#else
    if (pthread_mutex_trylock(&m_mutex) == 0) {
        // Mutex is now locked by this thread
        return true;
    }
#endif
    return false;
}

//----------------------------------------------------------
// class MutexLocker implementation
//----------------------------------------------------------

MutexLocker::MutexLocker(Mutex *pMutex)
    : m_pMutex(pMutex)
{
    if (m_pMutex) {
        m_pMutex->lock();
    }
}

MutexLocker::~MutexLocker()
{
    if (m_pMutex) {
        m_pMutex->unlock();
    }
}

} // namespace ucxx
