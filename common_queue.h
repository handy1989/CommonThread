#ifndef _COMMON_QUEUE_H_
#define _COMMON_QUEUE_H_

#include <pthread.h>
#include <sys/time.h>
#include <errno.h>
#include <exception>
#include <string>
#include <queue>
#include <stdint.h>
#include <glog/logging.h>

template <typename T> class CommonQueue
{
    int m_capacity;
    int m_max_size;

    std::queue<T> m_queue;
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond_not_full;
    pthread_cond_t m_cond_not_empty;
public:
    CommonQueue()
    {
        m_capacity = 0;
        m_max_size = 0;
    }
    virtual ~CommonQueue()
    {
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_cond_not_full);
        pthread_cond_destroy(&m_cond_not_empty);
    }
    
    bool init(int capacity)
    {
        pthread_mutex_init(&m_mutex, NULL);
        if (pthread_cond_init(&m_cond_not_full, NULL))
        {
            LOG(ERROR) << "CommonQueue init failed!";
            return false;
        }
        if (pthread_cond_init(&m_cond_not_empty, NULL))
        {
            LOG(ERROR) << "CommonQueue init failed!";
            return false; 
        }
        m_capacity = capacity;
        return true;
    }

    bool pop(T& retVal, int wait_ms)
    {
        bool ret = false;
        pthread_mutex_lock(&m_mutex);

        try
        {
            if (m_queue.empty())
            {
                // queue is empty
                if (0 == wait_ms)
                {
                    // wait infinitely
                    while (m_queue.empty())
                    {
                        pthread_cond_wait(&m_cond_not_empty, &m_mutex);
                    }
                }
                else
                {
                    // wait for timeout
                    struct timespec timespot;
                    if (wait_ms > 0)
                    {
                        getAbsTimeout(wait_ms, timespot);
                    }
                    else
                    {
                        timespot.tv_sec = 0;
                        timespot.tv_nsec = 0;
                    }
                    pthread_cond_timedwait(&m_cond_not_empty, &m_mutex, &timespot);
                }
            }
            else
            {
                retVal = m_queue.front();
                m_queue.pop();
                ret = true;
                pthread_mutex_unlock(&m_mutex);
            }
        }
        catch (std::exception&)
        {
            ret = false;
        }
        pthread_mutex_unlock(&m_mutex);
        return ret;
    }

    bool push(const T& element, int wait_ms)
    {
        bool ret = false;
        pthread_mutex_lock(&m_mutex);

        try
        {
            if (m_queue.size() >= m_capacity)
            {
                //queue is full
                if (0 == wait_ms)
                {
                    //wait infinitely
                    while (m_queue.size() >= m_capacity)
                    {
                        pthread_cond_wait(&m_cond_not_full, &m_mutex);
                    }
                }
                else
                {
                    //wait for timeout
                    struct timespec timespot;
                    if (wait_ms > 0)
                    {
                        getAbsTimeout(wait_ms, timespot);
                    }
                    else
                    {
                        timespot.tv_sec = 0;
                        timespot.tv_nsec = 0;
                    }
                    pthread_cond_timedwait(&m_cond_not_full, &m_mutex, &timespot);
                }
            }
            else
            {
                m_queue.push(element);
                if (m_max_size < m_queue.size())
                {
                    m_max_size = m_queue.size();
                }
                pthread_cond_signal(&m_cond_not_empty);
                ret = true;
            }
        }
        catch (std::exception &)
        {
            ret = false;
        }
        pthread_mutex_unlock(&m_mutex);
        return ret;
    }
    
    bool pop(T& retVal)
    {
        return pop(retVal, -1);
    }
    
    bool push(const T& element)
    {
        return push(element, -1);
    }

    int getCapacity()
    {
        return m_capacity;
    }

    int getSize()
    {
        return m_queue.size();
    }

    int getMaxSize()
    {
        return m_max_size;
    }

private:
    void getAbsTimeout(int timeout_ms, struct timespec& timeout)
    {
        struct timeval now;
        gettimeofday(&now, NULL);
        // struct timeval
        //{
        //    tv_sec; // seconds
        //    tv_usec; // microseconds, 10^(-6)seconds
        //}
        //struct timespec
        //{
        //    tv_sec; // seconds
        //    tv_nsec; //nanoseconds, 10^(-9) seconds 
        //}
        int timeout_ns = timeout_ms * 1000 * 1000 + now.tv_usec * 1000; // change to nanoseconds
        timeout.tv_sec = now.tv_sec + timeout_ns / 1000 / 1000 / 1000;
        timeout.tv_nsec = timeout_ns % (1000 * 1000 * 1000);

    }
};

#endif
