#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <stdint.h>
#include <cstring>
#include <string>

#include "common_define.h"
#include "common_utility.h"
#include "common_queue.h"


class Message
{
public:
    Message();
    virtual ~Message();

    int version();
    int messageType();
    int messageSequence();

    virtual void setHeaderControl() = 0;
    virtual bool parseBody() = 0;
    virtual bool buildBody() = 0;

    bool parseHeader();
    bool parseMessage();
    bool buildMessage(const Message* message = NULL);
    static int getMessageType(const char* buffer,int buffer_size,int sign);
    
    int m_socket_fd;
    string m_buffer;
    int m_listen_port;
    int m_client_port;
    int m_ip_addr;
    int m_task_sign;
    message_header_t m_message_header;
    string m_msg_body_str;

private:
    void copyCommonParameter(const Message* message);
};
typedef CommonQueue<Message*> MessageQueue;
#endif
