#ifdef WIN32
#   include <Windows.h>
#   include <stdarg.h>
#   include "Mutex.h"
#else
#   ifdef __APPLE__
#      include <sys/time.h>
#   endif
#   include <time.h>
#   include <errno.h>
#   include <stdlib.h>
#endif
#include <assert.h>
#include "Semaphore.h"

namespace ucxx {

Semaphore::Semaphore(int value)
{
#ifdef WIN32
    m_semaphore = CreateSemaphore(0, value, MAXLONG, 0);
    assert(m_semaphore);
    m_count = value;
    m_pMutex = new Mutex();
#elif defined __APPLE__
    pthread_mutex_init(&m_countMutex, 0);
    pthread_mutex_init(&m_waitMutex, 0);
    pthread_cond_init(&m_waitCond, 0);
    m_count = value;
#else
    memset(&m_semaphore, 0, sizeof(sem_t));
    sem_init(&m_semaphore, 0, value);
#endif
}

Semaphore::~Semaphore()
{
#ifdef WIN32
    CloseHandle(m_semaphore);
    delete m_pMutex;
#elif defined __APPLE__
    pthread_mutex_destroy(&m_countMutex);
    pthread_mutex_destroy(&m_waitMutex);
    pthread_cond_destroy(&m_waitCond);
#else
    sem_destroy(&m_semaphore);
#endif
}

bool Semaphore::wait(unsigned int milliseconds)
{
#ifdef WIN32
    DWORD dwWaitResult = WaitForSingleObject(m_semaphore, (milliseconds == 0) ? INFINITE : milliseconds);
    if (dwWaitResult == WAIT_OBJECT_0) {
        // Semaphore acquired
        m_pMutex->lock();
        m_count--;
        m_pMutex->unlock();
        return true;
    }

    return false;

#elif defined __APPLE__

    pthread_mutex_lock(&m_countMutex);
    if (m_count > 0) {
        m_count--;
        pthread_mutex_unlock(&m_countMutex);
        return true;
    }
    pthread_mutex_unlock(&m_countMutex);

    pthread_mutex_lock(&m_waitMutex);
    int cres;
    if (milliseconds == 0)
    cres = pthread_cond_wait(&m_waitCond, &m_waitMutex);
    else {
        struct timespec ts;
        struct timeval tv;
        // Darwin does not have clock_gettime function.
        gettimeofday(&tv, 0);
        ts.tv_sec = tv.tv_sec + milliseconds / 1000;
        ts.tv_nsec = tv.tv_usec * 1000 + (milliseconds % 1000) * 1000;
        ts.tv_sec += ts.tv_nsec / 1000000000;
        ts.tv_nsec %= 1000000000;

        cres = pthread_cond_timedwait(&m_waitCond, &m_waitMutex, &ts);
    }
    pthread_mutex_unlock(&m_waitMutex);

    if (cres == 0) {
        pthread_mutex_lock(&m_countMutex);
        if (m_count > 0) {
            m_count--;
            pthread_mutex_unlock(&m_countMutex);
            return true;
        }
        pthread_mutex_unlock(&m_countMutex);
        return false;
    }
    return false;

#else

    if (milliseconds == 0) {
        if (sem_wait(&m_semaphore) == 0) {
            return true;
        }
        return false;
    }

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += milliseconds / 1000;
    ts.tv_nsec += (milliseconds % 1000) * 1000000;
    ts.tv_sec += ts.tv_nsec / 1000000000;
    ts.tv_nsec %= 1000000000;

    int s;
    while ((s = sem_timedwait(&m_semaphore, &ts)) == -1 && errno == EINTR) {
        continue; // Restart when interrupted
    }

    if (s == -1) {
        if (errno == ETIMEDOUT) {
            return false; // Semaphore wait timedout
        }
        assert(!"sem_timedwait");
        return false;
    }

    return true; // Semaphore acquired
#endif
}

int Semaphore::value()
{
    int cnt = 0;
#ifdef WIN32
    m_pMutex->lock();
    cnt = m_count;
    m_pMutex->unlock();
#elif defined __APPLE__
    pthread_mutex_lock(&m_countMutex);
    cnt = m_count;
    pthread_mutex_unlock(&m_countMutex);
#else
    sem_getvalue(&m_semaphore, &cnt);
#endif
    return cnt;
}

void Semaphore::notify(int increment)
{
#ifdef WIN32
    LONG cnt;
    m_pMutex->lock();
    ReleaseSemaphore(m_semaphore, increment, &cnt);
    m_count = cnt + increment;
    m_pMutex->unlock();
#else
    // Is there more efficient way to do this?
    for (int i = 0; i < increment; i++) {
#   ifdef __APPLE__
        pthread_mutex_lock(&m_countMutex);
        m_count++;
        pthread_mutex_unlock(&m_countMutex);
        pthread_cond_signal(&m_waitCond);
#   else
        sem_post(&m_semaphore);
#   endif
    }
#endif
}

} // namespace ucxx
