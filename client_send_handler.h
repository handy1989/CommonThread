#ifndef _CLIENT_SEND_HANDLER_H_
#define _CLIENT_SEND_HANDLER_H_

#include <glog/logging.h>

#include "message_factory.h"
#include "epoll_socket.h"
#include "epoll_socket_manager.h"
#include "common_thread_pool.h"

class ClientSendHandler : public CommonThreadPool
{
    public:
    ClientSendHandler(int pool_size);
    virtual ~ClientSendHandler();

    void initQueues(MessageQueue* message_send_queue);
    bool sendData(int socket_fd,const char* data, int data_len);

    protected:
    virtual void work();

    private:
    MessageQueue* m_message_send_queue;

    uint32_t m_revc_message;
    uint32_t m_send_succ;
    uint32_t m_send_fail;
};

#endif
