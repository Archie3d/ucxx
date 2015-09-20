#ifndef UCXX_QUEUE_H
#define UCXX_QUEUE_H

//
// Thread-safe queue
//

namespace ucxx {

#ifndef QUEUE_H
#define QUEUE_H

#include <queue>
#include "Mutex.h"
#include "Semaphore.h"

/**
 * @brief Thread safe queue
 */
template <typename T>
class Queue
{
public:
    Queue() {}
    ~Queue() {}

    /**
     * @brief Dequeue a single item from the queue.
     * @note This method will block if the queue is empty.
     * @return Item dequeued.
     */
    T dequeue()
    {
        // Wait for data in the queue
        m_semaphore.wait();
        MutexLocker locker(&m_mutex);
        T element = m_queue.front();
        m_queue.pop();
        return element;
    }

    /**
     * @brief Enqueue an item.
     * @note This will notify on queue's semaphore.
     * @param t Item to be enqueued.
     */
    void enqueue(const T &t)
    {
        MutexLocker locker(&m_mutex);
        m_queue.push(t);
        m_semaphore.notify(1);
    }

    /**
     * @brief Returns the first item without dequeuing it.
     * @return Queue first item.
     */
    T head() const
    {
        MutexLocker locker(&m_mutex);
        return m_queue.front();
    }

    /**
     * @brief Returns number of items in the queue.
     * @return Number of items.
     */
    int count() const
    {
        MutexLocker locker(&m_mutex);
        return m_queue.size();
    }

    /**
     * @brief Clear the queue.
     */
    void clear()
    {
        MutexLocker locker(&m_mutex);
        for (int i = 0; i < m_queue.size(); ++i) {
            // Synchronize with the semaphore counter.
            m_semaphore.wait();
        }
        m_queue = std::queue<T>();
    }

private:

    mutable Mutex m_mutex;
    Semaphore m_semaphore;
    std::queue<T> m_queue;
};

} // ucxx

#endif // UCXX_QUEUE_H
