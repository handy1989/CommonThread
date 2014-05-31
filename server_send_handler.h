#ifndef _SERVER_SEND_HANDLER_H_
#define _SERVER_SEND_HANDLER_H_

#include "common_thread_pool.h"

#include "server_send_socket.h"

class ServerSendHandler : public CommonThreadPool
{
public:
    ServerSendHandler(int pool_size);
    ~ServerSendHandler();

    void initSocket(const char* ip, short port, int send_timeout_sec, int send_buf_size);

    void initQueues(MessageQueue* message_send_queue);
    
protected:
    virtual void work();

private:
    MessageQueue* m_message_send_queue;

    std::string m_ip;
	short m_port;
	int m_send_timeout_sec;
	int m_send_buf_size;
};

#endif
