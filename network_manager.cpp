#include "network_manager.h"

NetworkManager* NetworkManager::m_instance = new NetworkManager();

NetworkManager::NetworkManager()
{
    m_queue_size = 0;
    m_client_receive_thread_count = 0;
    m_client_connection_handler = NULL;
    m_sock_queue = NULL;
    
    m_client_long_connect_send_thread_count = 0;

    m_client_connection_handler = NULL;
    m_sock_queue = NULL;
    m_client_receiver_handler = NULL;
    m_message_queue = NULL;

    m_client_long_connect_send_handlers = NULL;
    m_client_long_connect_message_send_queues = NULL;

    m_server_long_connect_send_handler = NULL;
    m_server_long_connect_message_send_queue = NULL;
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

    if(m_client_receiver_handler)
    {
        delete m_client_receiver_handler;
        m_client_receiver_handler = NULL;
    }
    if(m_message_queue)
    {
        delete m_message_queue;
        m_message_queue = NULL;
    }

    for(uint32_t i=0;i != m_client_long_connect_send_thread_count;++i)
    {
        if(m_client_long_connect_send_handlers[i])
        {
            delete m_client_long_connect_send_handlers[i];
            m_client_long_connect_send_handlers[i] = NULL;
        }
    }
    if(m_client_long_connect_send_handlers)
    {
        delete [] m_client_long_connect_send_handlers;
        m_client_long_connect_send_handlers = NULL;
    }

    for(uint32_t i=0;i != m_client_long_connect_send_thread_count;++i)
    {
        if(m_client_long_connect_message_send_queues[i])
        {
            delete m_client_long_connect_message_send_queues[i];
            m_client_long_connect_message_send_queues[i] = NULL;
        }
    }
    if(m_client_long_connect_message_send_queues)
    {
        delete [] m_client_long_connect_message_send_queues;
        m_client_long_connect_message_send_queues = NULL;
    }

    if(m_server_long_connect_send_handler)
    {
        delete m_server_long_connect_send_handler;
        m_server_long_connect_send_handler = NULL;
    }

    if(m_server_long_connect_message_send_queue)
    {
        delete m_server_long_connect_message_send_queue;
        m_server_long_connect_message_send_queue = NULL;
    }
}

NetworkManager* NetworkManager::getInstance()
{
    return m_instance;
}

void NetworkManager::setQueueSize(int queue_size)
{
    LOG(INFO) << "NetworkManager::setQueueSize(), queue_size[" << queue_size << "]";  
    m_queue_size = queue_size;
}

bool NetworkManager::initSocket(netaddres_info_t* netaddr, const int& addr_num, const int& wait_time_ms)
{
    LOG(INFO) << "NetworkManager::initSocket()!";
    return EpollSocketManager::getInstance()->initSocket(netaddr, addr_num, wait_time_ms);
}

void NetworkManager::initClientRecv(int check_internal, int recv_thread_count)
{
    LOG(INFO) << "NetworkManager::initClientRecv()!";
    m_sock_queue = new ClientSocketQueue();
    m_sock_queue->init(m_queue_size);
    
    m_client_connection_handler = new ClientConnectionHandler(check_internal);
    m_client_connection_handler->initQueues(m_sock_queue);

    m_client_receiver_handler = new ClientReceiverHandler(recv_thread_count);
    m_client_receiver_handler->initQueues(m_sock_queue, m_message_queue);
}

void NetworkManager::initClientLongConnectSend(
        uint32_t client_long_connect_send_thread_count)
{
    m_client_long_connect_send_thread_count = client_long_connect_send_thread_count;

    m_client_long_connect_send_handlers = new ClientSendHandler*[m_client_long_connect_send_thread_count];
    m_client_long_connect_message_send_queues = new MessageQueue*[m_client_long_connect_send_thread_count];

    for(uint32_t i = 0;i != m_client_long_connect_send_thread_count;++i)
    {
        m_client_long_connect_message_send_queues[i] = new MessageQueue();
        m_client_long_connect_message_send_queues[i]->init(m_queue_size);
        m_client_long_connect_send_handlers[i] = new ClientSendHandler(1);
        m_client_long_connect_send_handlers[i]->initQueues(m_client_long_connect_message_send_queues[i]);
    }
}

void NetworkManager::initServerLongConnectSend(
        uint32_t server_long_connect_send_thread_count,
        const char * server_ip,
        short server_port,
        int send_timeout_sec,
        int send_buf_size)
{
    m_server_long_connect_send_thread_count = server_long_connect_send_thread_count;

    m_server_long_connect_message_send_queue= new MessageQueue();
    m_server_long_connect_message_send_queue->init(m_queue_size);
    m_server_long_connect_send_handler = new ServerSendHandler(m_server_long_connect_send_thread_count);
    m_server_long_connect_send_handler->initQueues(m_server_long_connect_message_send_queue);
    m_server_long_connect_send_handler->initSocket(server_ip,server_port,send_timeout_sec,send_buf_size);

}

MessageQueue* NetworkManager::getClientSendQueue(int num)
{

    if(m_client_long_connect_message_send_queues == NULL)
    {
        return NULL;
    }

    uint32_t index = num % m_client_long_connect_send_thread_count;

    return m_client_long_connect_message_send_queues[index];
}


MessageQueue* NetworkManager::getMessageQueue()
{
    return m_message_queue;
}

bool NetworkManager::clientLongConnectPush(Message* message, int waitMs)
{
    MessageQueue* message_queue = m_instance->getClientSendQueue(message->m_socket_fd);
    if(message_queue == NULL)
        return false;

    return message_queue->push(message,waitMs);
}

bool NetworkManager::messagePop(Message*& message, int waitMs)
{
    MessageQueue* message_queue = m_instance->getMessageQueue();

    if(message_queue == NULL)
        return false;

    return message_queue->pop(message,waitMs);
}

bool NetworkManager::start()
{
    LOG(INFO) << "NetworkManager::start()!";
    if (m_client_connection_handler)
    {
        LOG(INFO) << "client_connection_handler starts";
        if (!m_client_connection_handler->startThreads())
        {
            LOG(ERROR) << "client_connection_handler starts failed!";
            return false;
        }
    }
    if (m_client_receiver_handler)
    {
        LOG(INFO) << "client_receiver_handler starts";
        if (!m_client_receiver_handler->startThreads())
        {
            LOG(ERROR) << "client_reveiver_handler starts failed!";
            return false;
        }
    }
    for(uint32_t i = 0;i != m_client_long_connect_send_thread_count;++i)
    {
        if(!m_client_long_connect_send_handlers[i]->startThreads())
        {
            return false;
        }
    }

    if(m_server_long_connect_send_handler)
    {
        if(!m_server_long_connect_send_handler->startThreads())
        {
            return false;
        }
    }
    return true;
}

void NetworkManager::waitThreadsTermination()
{
    LOG(INFO) << "NetworkManager::waitThreadsTermination()!";
    m_client_connection_handler->waitThreadsTermination();
    m_client_receiver_handler->waitThreadsTermination();
}
