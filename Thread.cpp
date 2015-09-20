#ifndef WIN32
#   ifdef __APPLE__
#       include <sys/time.h>
#   endif
#   include <time.h>
#endif

#include <assert.h>
#include "Mutex.h"
#include "Semaphore.h"
#include "Thread.h"

namespace ucxx {

/* Thread running functions */

#ifdef WIN32
DWORD WINAPI threadRunner(LPVOID lpParam)
{
    assert(lpParam);
    Thread *pThread = static_cast<Thread*>(lpParam);
    pThread->setRunning(true);
    pThread->run();
    pThread->setRunning(false);
    return 0;
}
#else
void* threadRunner(void *param)
{
    assert(param);
    Thread *pThread = static_cast<Thread*> (param);

    // Thread will block here waiting for starting semaphore
    pThread->m_pStartSemaphore->wait();
    pThread->setRunning(true);
    pThread->run();
    pThread->setRunning(false);
    return 0;
}
#endif

/* Thread class implementation */

Thread::Thread(IRunnable *pRunnable)
{
    m_running = false;
    m_pMutex = new Mutex();
    m_pRunnable = pRunnable;

#ifdef WIN32
    m_thread = CreateThread(
            0, // Default security attributes
            0, // Default stack size
            (LPTHREAD_START_ROUTINE)threadRunner, // Thread routine
            this, // Argument to thread routine
            CREATE_SUSPENDED, // Do not start the thread
            &m_threadId // Returned thread id
    );
    assert(m_thread);
#else
    m_pStartSemaphore = new Semaphore();

    int res = pthread_create(&m_thread, // Thread reference
            0, // Attributes
            &threadRunner, // Thread routine
            this // Argument to the thread routine
            );
    assert(!res);
#endif
}

Thread::~Thread()
{
#ifdef WIN32
    CloseHandle(m_thread);
#else
    delete m_pStartSemaphore;
#endif
    delete m_pMutex;
}

void Thread::setRunning(bool value)
{
    MutexLocker locker(m_pMutex);
    m_running = value;
}

void Thread::start()
{
#ifdef WIN32
    ResumeThread(m_thread);
#else
    m_pStartSemaphore->notify();
#endif
}

void Thread::run()
{
    // Run IRunnable object if assigned
    if (m_pRunnable) {
        m_pRunnable->run();
    }
}

bool Thread::isRunning() const
{
    MutexLocker locker(m_pMutex);
    return m_running;
}

void Thread::terminate()
{
#ifdef WIN32
    TerminateThread(m_thread, 0);
#else
    pthread_cancel(m_thread);
#endif
}

void Thread::join()
{
#ifdef WIN32
    WaitForSingleObject(m_thread, INFINITE);
#else
    pthread_join(m_thread, 0);
#endif
}

void Thread::sleep(unsigned int milliseconds)
{
#ifdef WIN32
    Sleep(milliseconds);
#else
    struct timespec ts;
    pthread_mutex_t sleepMutex;
    pthread_cond_t sleepCond;

    pthread_mutex_init(&sleepMutex, 0);
    pthread_cond_init(&sleepCond, 0);
#ifdef __APPLE__
    struct timeval tv;
    // Darwin does not have clock_gettime function.
    gettimeofday(&tv, 0);
    ts.tv_sec = tv.tv_sec + milliseconds / 1000;
    ts.tv_nsec = tv.tv_usec * 1000 + (milliseconds % 1000) * 1000000;
#else
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += milliseconds / 1000;
    ts.tv_nsec += (milliseconds % 1000) * 1000000;
#endif

    ts.tv_sec += ts.tv_nsec / 1000000000;
    ts.tv_nsec %= 1000000000;

    pthread_mutex_lock(&sleepMutex);
    pthread_cond_timedwait(&sleepCond, &sleepMutex, &ts);
    pthread_mutex_unlock(&sleepMutex);

    pthread_cond_destroy(&sleepCond);
    pthread_mutex_destroy(&sleepMutex);
#endif
}

void Thread::setCurrentThreadPriority(Thread::Priority priority)
{
#ifdef WIN32
    HANDLE threadHandle = GetCurrentThread();
    int p = THREAD_PRIORITY_NORMAL;
    switch (priority) {
    case Priority_Idle:
        p = THREAD_PRIORITY_IDLE;
        break;
    case Priority_Lowest:
        p = THREAD_PRIORITY_LOWEST;
        break;
    case Priority_Low:
        p = THREAD_PRIORITY_BELOW_NORMAL;
        break;
    case Priority_Normal:
        p = THREAD_PRIORITY_NORMAL;
        break;
    case Priority_High:
        p = THREAD_PRIORITY_ABOVE_NORMAL;
        break;
    case Priority_Highest:
        p = THREAD_PRIORITY_HIGHEST;
        break;
    case Priority_Realtime:
        p = THREAD_PRIORITY_TIME_CRITICAL;
        break;
    default:
        break;
    }
    BOOL res = SetThreadPriority(threadHandle, p);
    if (!res) {
        //DWORD err = GetLastError();
    }
#endif
}

void Thread::setCurrentProcessPriority(Thread::Priority priority)
{
#ifdef WIN32
    HANDLE processHandle = GetCurrentProcess();
    int p = NORMAL_PRIORITY_CLASS;
    switch (priority) {
    case Priority_Idle:
        p = IDLE_PRIORITY_CLASS;
        break;
    case Priority_Lowest:
    case Priority_Low:
        p = BELOW_NORMAL_PRIORITY_CLASS;
        break;
    case Priority_Normal:
        p = NORMAL_PRIORITY_CLASS;
        break;
    case Priority_High:
        p = ABOVE_NORMAL_PRIORITY_CLASS;
        break;
    case Priority_Highest:
        p = HIGH_PRIORITY_CLASS;
        break;
    case Priority_Realtime:
        p = REALTIME_PRIORITY_CLASS;
        break;
    default:
        break;
    }
    BOOL res = SetPriorityClass(processHandle, p);
    if (!res) {
        //DWORD err = GetLastError();
    }
#endif
}

} // namespace ucxx
