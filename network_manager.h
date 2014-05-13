#ifndef _NETWORK_MANAGER_H_
#define _NETWORK_MANAGER_H_

#include <glog/logging.h>
#include "epoll_socket.h"
#include "client_connection_handler.h"
#include "client_receiver_handler.h"

class NetworkManager
{
public:
    NetworkManager();
    ~NetworkManager();

    void setQueueSize(int queue_size);
    bool initSocket(netaddres_info_t *netaddr, const int& addr_num, const int& wait_time_ms);
    void initClientRecv(int check_internal, int recv_thread_count);

    static NetworkManager* getInstance();
    bool start();
    void waitThreadTermination();

private:
    static NetworkManager* m_instance;
    int m_queue_size;
    int m_client_receive_thread_count;

    ClientConnectionHandler* m_client_connection_handler;
    ClientSocketQueue* m_sock_queue;

    ClientReceiverHandler* m_client_receiver_handler;
    MessageQueue* m_message_queue;

 
};

#endif
