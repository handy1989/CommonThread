#ifndef _CLIENT_CONNECTION_HANDLER_H_
#define _CLIENT_CONNECTION_HANDLER_H_

#include "common_queue.h"
#include "common_thread_pool.h"
#include "epoll_socket_manager.h"
#include "common_utility.h"

class ClientConnectionHandler : public CommonThreadPool
{
public:
    ClientConnectionHandler(int check_internal);
    virtual ~ClientConnectionHandler();
    void initQueues(ClientSocketQueue*);

protected:
    virtual void work();

private:
    ClientSocketQueue* m_recv_sock_q;
    int m_check_internal;
    int m_cur_time;
    int m_last_check_point;

    void checkTimeout();
    void processConnection(
            int new_fd,
            int listen_port,
            int ip_addr,
            int client_port,
            int conn_timeout_ms); 
};

#endif
