#ifndef UCXX_MUTEX_H
#define UCXX_MUTEX_H

//
//  System-specific mutex implementation
//

#ifdef WIN32
#   include <Windows.h>
#else
#   include <pthread.h>
#endif

namespace ucxx {

/**
 * @brief Platform-specific mutex implementation.
 * The mutex implemented by this class is recursive, meaning it can
 * be locked multiple times from the same thread.
 */
class Mutex
{
public:

    /**
     * @brief Construct a mutex.
     * Constructed mutex is unlocked by default.
     */
    Mutex();

    /**
     * @brief Destroy the mutex.
     * @note Destroying the mutex does not guarantee its release.
     */
    ~Mutex();

    /**
     * @brief Acquire a lock on this mutex.
     * This method will block if the mutex is already locked by another thread.
     */
    void lock();

    /**
     * @brief Release the mutex.
     */
    void unlock();

    /**
     * @brief Attempt to lock the mutex.
     * @note This method will not block is the mutex is already locked.
     * @return true if mutex lock has been acquired, false if locked by another thread.
     */
    bool tryLock();

private:

    // Disable copying
    Mutex(const Mutex&) {}
    Mutex& operator =(const Mutex&) { return *this; }

    /// Platform-specific mutex implementation.
#ifdef WIN32
    HANDLE m_mutex;
#else
    pthread_mutex_t m_mutex;
#endif
};

/**
 * @brief Mutex lock helper class.
 * This class is used to automatically release a mutex when
 * the locker's instance goes out of scope.
 */
class MutexLocker
{
public:

    /**
     * @brief Construct the locked.
     * @note This will immediately attempt to lock the mutex.
     * @Param pMutex Pointer to the mutex to be locked.
     */
    MutexLocker(Mutex *pMutex);

    /**
     * @brief Unlock the mutex.
     */
    ~MutexLocker();

private:

    // Disable copying and default constructor.
    MutexLocker() {}
    MutexLocker(const MutexLocker&) {}
    MutexLocker& operator =(const MutexLocker&) { return *this; }

    /// Mutex object to be handled by this locker.
    Mutex *m_pMutex;
};

} // namespace ucxx

#endif // UCXX_MUTEX_H
