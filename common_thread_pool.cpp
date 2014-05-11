#include <exception>
#include <stdlib.h>
#include "common_thread_pool.h"

void CommonThreadPool::destroyThreadId()
{
    if (m_thread_ids)
    {
        delete []m_thread_ids;
        m_thread_ids = NULL;
    }
}

bool CommonThreadPoll::startThreads()
{
    if (m_pool_size > 0)
    {
        destroyThreadId();
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_getstacksize(&attr, STACK_SIZE);
        
        m_thread_ids = new pthread_t[m_pool_size];
        for (int i = 0; i < m_pool_size; ++i)
        {
            if (pthread_create(m_thread_ids[i], &attr, threadProc, (void*)this))
            {
                return false;
            }
        }
        pthread_attr_destroy(&attr);
    }
    return true;
}

void CommonThreadPool::waitThreadsTermination()
{
    if (m_thread_ids)
    {
        for (int i = 0; i < m_pool_size; ++i)
        {
            pthread_join(m_thread_ids[i], NULL);
        }
        destroyThreadId();
    }
}

void CommonThreadPool::detachThreads()
{
    if (m_thread_ids)
    {
        for (int i = 0; i < m_pool_size; ++i)
        {
            pthread_detach(m_thread_ids[i]);
        }
    }
}

void *CommonThreadPool::threadProc(void *para)
{
    CommonThreadPool* common_thread_pool_ptr = (CommonThreadPool*)para;
    try
    {
        common_thread_pool_ptr->work();
    }
    catch(std::exception &e)
    {
        LOG(INFO) << "threadProc error: " << e.what();
        abort();
    }
    catch(...)
    {
        LOG(INFO) << "threadProc unknown error!";
        abort();
    }
    return NULL;
}
