#ifndef _COMMON_THREAD_POOL_H_
#define _COMMON_THREAD_POOL_H_

#include <pthread.h>
#include <glog/logging.h>

class CommonThreadPool
{
    const static int STACK_SIZE = (256*1024);
public:
    CommonThreadPool(int pool_size) : m_pool_size(pool_size)
    {
        m_thread_ids = NULL;
    }
    virtual ~CommonThreadPool()
    {
        destroyThreadId();
    }
    void destroyThreadId();
    inline int getPoolSize()
    {
        return m_pool_size;
    }
    bool startThreads();
    void waitThreadsTermination();
    void detachThreads();

protected:
    virtual void work() = 0;
private:
    int m_pool_size;
    static void* threadProc(void* para);
    pthread_t *m_thread_ids;
};

#endif
