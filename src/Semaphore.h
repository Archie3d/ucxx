#ifndef UCXX_SEMAPHORE_H
#define UCXX_SEMAPHORE_H

//
// Platform-specific semaphore implementation.
//

#ifdef WIN32
#   include <Windows.h>
#elif defined __APPLE__
#   include <pthread.h>
#else
#   include <semaphore.h>
#endif

namespace ucxx {

/*
 * POSIX semaphores are not fully supported by Mac OS, so we have to use
 * custom implementation here via pthread condition notification.
 */

#ifdef WIN32
    class Mutex;
#endif

/**
 * @brief Platform-specific semaphore implementation.
 * This class implements a counting semaphore.
 */
class Semaphore
{
public:

    /**
     * @brief Construct a semaphore.
     * @param value Initial counter value.
     */
    Semaphore(int value = 0);

    /**
     * @brief Destroy this semaphore.
     */
    ~Semaphore();

    /**
     * @brief Wait on semaphore.
     * This method will decrement the semaphore's counter by one.
     * @note This method will block if semaphore's counter is zero.
     * @param milliseconds Waiting timeout.
     * @return true if semaphore has been released, false on timeout.
     */
    bool wait(unsigned int milliseconds = 0);

    /**
     * @brief Returns semaphore's counter value.
     * @return Semaphore's counter.
     */
    int value();

    /**
     * @brief Release the semaphore.
     * @param increment Semaphore counter increment.
     */
    void notify(int increment = 1);

private:

    // Disable copying
    Semaphore(const Semaphore&) {}
    Semaphore& operator =(const Semaphore&) { return *this; }

#ifdef WIN32
    HANDLE m_semaphore; ///< System semaphore.
    int m_count;        ///< Semaphore counter.
    Mutex *m_pMutex;    ///< Protective mutex.
#elif defined __APPLE__
    // OS X does not support timed-out semaphore,
    // so we have to use timeout on condition as workaround.
    pthread_mutex_t m_countMutex;   ///< Mutex to protect semaphore counter.
    pthread_mutex_t m_waitMutex;    ///< Mutex for conditional wait.
    pthread_cond_t m_waitCond;      ///< Conditional wait.
    int m_count;
#else
    sem_t m_semaphore; ///< System semaphore.
#endif
};

} // namespace ucss

#endif // UCXX_SEMAPHORE_H
