#ifndef _CLIENT_RECEIVER_HANDLER_H_
#define _CLIENT_RECEIVER_HANDLER_H_

#include <glog/logging>
#include "common_queue.h"
#include "common_utility.h"
#include "client_connection_handler.h"

class ClientReceiverHandler : public CommonThreadPoll
{
public:
    ClientReceiverHandler(int thread_count);
    virtual ClientReceiverHandler();

    int recvData(
            int sock_fd,
            char *recv_buffer,
            const int need_recv_bytes,
            int& real_recv_len); 
    int recvCmd(SocketInfoManager& socket_info);
    void initQueues(ClientSocketQueue *socket_queue, MessageQueue* message_queue);

protected:
    virtual void work();

private:
    ClientSocketQueue* socket_queue;
    MessageQueue* message_queue;
    
    int m_recv_message_num;
    int m_push_queue_succ;
    int m_push_queue_fail;
};

#endif
