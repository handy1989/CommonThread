#ifndef _NETWORK_MANAGER_H_
#define _NETWORK_MANAGER_H_

#include <glog/logging.h>
#include "epoll_socket.h"
#include "client_connection_handler.h"
#include "client_receiver_handler.h"
#include "client_send_handler.h"
#include "server_send_handler.h"

class NetworkManager
{
public:
    NetworkManager();
    ~NetworkManager();

    void setQueueSize(int queue_size);
    bool initSocket(netaddres_info_t *netaddr, const int& addr_num, const int& wait_time_ms);
    void initClientRecv(int check_internal, int recv_thread_count);
    void initClientLongConnectSend(uint32_t client_long_connect_send_thread_count);
    void initServerLongConnectSend(
            uint32_t server_long_connect_send_thread_count,
            const char * server_ip,
            short server_port,
            int send_timeout_sec,
            int send_buf_size);   
    
    static NetworkManager* getInstance();
    bool start();
    void waitThreadsTermination();

    MessageQueue* getClientSendQueue(int num);
    MessageQueue* getServerSendQueue();
    MessageQueue* getMessageQueue();

    static bool clientLongConnectPush(Message* message, int waitMs);
    static bool messagePop(Message*& message, int waitMs);

private:
    static NetworkManager* m_instance;
    int m_queue_size;
    int m_client_receive_thread_count;
    uint32_t m_client_long_connect_send_thread_count;
    uint32_t m_server_long_connect_send_thread_count;

    ClientConnectionHandler* m_client_connection_handler;
    ClientSocketQueue* m_sock_queue;

    ClientReceiverHandler* m_client_receiver_handler;
    MessageQueue* m_message_queue;
    
    ClientSendHandler** m_client_long_connect_send_handlers;
    MessageQueue** m_client_long_connect_message_send_queues; 

    ServerSendHandler* m_server_long_connect_send_handler;
    MessageQueue* m_server_long_connect_message_send_queue;
};

#endif
