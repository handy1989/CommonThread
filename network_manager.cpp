#include "network_manager.h"

NetworkManager::NetworkManager()
{
    m_queue_size = 0;
    m_client_receive_thread_count = 0;
    m_client_connection_handler = NULL;
    m_sock_queue = NULL;
}

NetworkManager::~NetworkManager()
{
    if (m_client_connection_handler)
    {
        delete m_client_connection_handler;
        m_client_connection_handler = NULL;
    }
    if (m_sock_queue)
    {
        delete m_sock_queue;
        m_sock_queue = NULL;
    }
}

NetworkManager* NetworkManager::getInstance()
{
    return m_instance;
}

void NetworkManager::setQueueSize(int queue_size)
{
    m_queue_size = queue_size;
}

bool NetworkManager::initSocket(netaddres_info_t* netaddr, const int& addr_num, const int& wait_time_ms)
{
    return EpollSocketManager::getInstance()->initSocket(netaddr, addr_num, wait_time_ms);
}

void NetworkManager::initClientRecv(int check_internal, int recv_thread_count)
{
    m_sock_queue = new ClientSocketQueue();
    m_sock_queue->init(m_queue_size);
    
    m_client_connection_handler = new ClientConnectionHandler(check_internal);
    m_client_connection_handler->initQueues(m_sock_queue);

    m_client_receiver_handler = new ClientReceiverHandler(recv_thread_count);
    m_client_receiver_handler->initQueues(m_sock_queue, m_message_queue);
}

bool NetworkManager::start()
{
    if (m_client_connection_handler)
    {
        if (!m_client_connection_handler->startThreads())
        {
            LOG(ERROR) << "client_connection_handler starts failed!";
            return false;
        }
    }
    if (m_client_receiver_handler)
    {
        if (!m_client_receiver_handler->startThreads())
        {
            LOG(ERROR) << "client_reveiver_handler starts failed!";
            return false;
        }
    }
    return true;
}

void NetworkManager::waitThreadTermination()
{
    m_client_connection_handler->waitThreadsTermination();
    m_client_receiver_handler->waitThreadsTermination();
}
