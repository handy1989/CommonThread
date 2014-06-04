#include "message.h"

Message::Message()
{
    m_task_sign = MSG_SIGN;
}

Message::~Message()
{

}

int Message::version()
{
    LOG(INFO) << "Message::version()";
    return 0;
}

int Message::messageType()
{
    LOG(INFO) << "Message:messageType()";
    return m_message_header.type;
}

int Message::messageSequence()
{
    LOG(INFO) << "Message::message_queue()";
    return m_message_header.sequence;
}

bool Message::parseHeader()
{
    LOG(INFO) << "Message::parseHeader()";
    const char* msg_buffer = m_buffer.c_str();
    message_header_t* message_header = (message_header_t*) msg_buffer;
    m_message_header = *message_header;
    msg_buffer += message_header->header_size;
    m_msg_body_str.assign(msg_buffer,message_header->payload_size);
    return true; 
}

void Message::copyCommonParameter(const Message* message)    
{
    m_listen_port = message->m_listen_port;
    m_socket_fd = message->m_socket_fd;
    m_client_port = message->m_client_port;
    m_ip_addr = message->m_ip_addr;
    m_message_header.sequence = message->m_message_header.sequence;
}

bool Message::parseMessage()
{
    return (parseHeader() && parseBody());
}

bool Message::buildMessage(const Message* message)
{
    LOG(INFO) << "Message::buildMessage()";
    if(message)
    {
        copyCommonParameter(message);
    }

    uint32_t msg_head_size = sizeof(message_header_t);
    if(m_message_header.status == btq_status_success)
    {
        if(!buildBody())
        {
            return false;
        }
    }
    uint32_t head_buffer_size = msg_head_size;
    char* head_buffer = new char[head_buffer_size];

    char* msg_head_buffer = head_buffer;

    message_header_t* message_header = (message_header_t*)msg_head_buffer;

    *message_header = m_message_header;

    message_header->header_size = msg_head_size;
    message_header->payload_size = m_msg_body_str.size();

    m_buffer.assign(head_buffer,head_buffer_size);
    delete [] head_buffer;
    m_buffer += m_msg_body_str;
    return true;
}

int Message::getMessageType(const char* buffer,int buffer_size,int sign)
{
    message_header_t* message_header = (message_header_t*) buffer;
    return message_header->type;
}
