#ifndef _SERVER_SEND_SOCKET_H_
#define _SERVER_SEND_SOCKET_H_

#include "message_factory.h"
#include "epoll_socket_manager.h"
#include "send_socket.h"
class ServerSendSocket
{
public:
    ServerSendSocket();
    ~ServerSendSocket();

    void initSocket(const char * ip, const short& port, const int& send_timeout_sec, const int& send_buf_size);

    bool connectServer(const uint32_t& conn_timeout_ms);
    bool sendData(const Message* message,const int& timeout_ms = 100);

private:
    SendSocket m_send_socket;
};

#endif
