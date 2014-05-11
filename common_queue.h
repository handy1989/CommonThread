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
        pthread_mutex_init(&m_mutex);
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
        pthread_mutex_lock(m_mutex);
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
        timeout.tv_nsec = timeout_ns % (1000 * 1000 * 1000)

    }
};

#endif
