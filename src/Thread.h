#ifndef UCXX_THREAD_H
#define UCXX_THREAD_H

//
// Platform-specific thread implementation.
//

#ifdef WIN32
#   include <Windows.h>
#else
#   include <pthread.h>
#endif

namespace ucxx {

class Mutex;
class Semaphore;

/**
 * @brief Thread runnable interface.
 */
class IRunnable
{
public:
    virtual void run() = 0;
    virtual ~IRunnable() {}
};

/**
 * @brief This class implements a thread functionality.
 */
class Thread
{
public:

    /**
     * Thread priorities.
     */
    enum Priority {
        Priority_Idle,
        Priority_Lowest,
        Priority_Low,
        Priority_Normal,
        Priority_High,
        Priority_Highest,
        Priority_Realtime
    };

    /**
     * @brief Construct a thread.
     * The thread is constructed in suspended state. Caller has to
     * start it by calling start() method.
     */
    Thread(IRunnable *pRunnable = 0);

    /**
     * @brief Destructor.
     */
    virtual ~Thread();

    /**
     * @brief Start thread execution.
     */
    void start();

    /**
     * Tells whether a thread is running.
     * @return \c true if thread is running.
     */
    bool isRunning() const;

    /**
     * Terminate the thread.
     * This method is pretty dangerous. It will ask the OS
     * to terminate the thread. After termination, running flag
     * may not return to \c false.
     */
    void terminate();

    /**
     * Wait for the thread to terminate its execution.
     */
    void join();

    /**
     * Thread routine.
     */
    virtual void run();

    /**
     * Put thread in sleep for specified time.
     * @param milliseconds Sleep delay in ms.
     */
    static void sleep(unsigned int milliseconds);

    /**
     * Assign priority of the current thread.
     * This function is platform-specific.
     * @param priority Thread priority to be set.
     */
    static void setCurrentThreadPriority(Thread::Priority priority);

    /**
     * Assign current process priority.
     * @param priority Process priority to be set.
     */
    static void setCurrentProcessPriority(Thread::Priority priority);

    /**
     * System thread routine.
     */
#ifdef WIN32
    friend DWORD WINAPI threadRunner(LPVOID lpParam);

#else
    friend void* threadRunner(void *param);

#endif

private:

    /**
     * @brief Assign thread running state.
     * @param value New running state.
     */
    void setRunning(bool value);

    /**
     * System thread reference.
     */
#ifdef WIN32
    HANDLE m_thread;    ///< System thread handle.
    DWORD m_threadId;   ///< System thread id.
#else
    pthread_t m_thread; ///< System thread.
    Semaphore *m_pStartSemaphore;    ///< Thread starting semaphore.
#endif

    bool m_running;     ///< Thread running flag.
    Mutex *m_pMutex;    ///< Protective mutex.
    IRunnable *m_pRunnable;  ///< Pointer to an object implementing IRunnable interface.
};

} // namespace ucxx

#endif // UCXX_THREAD_H
