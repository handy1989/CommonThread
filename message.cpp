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
    return true; 
}

bool Message::parseMessage()
{
    return (parseHeader() && parseBody());
}

bool Message::buildMessage(const Message* message)
{
    LOG(INFO) << "Message::buildMessage()";
    return true; 
}

int Message::getMessageType(const char* buffer,int buffer_size,int sign)
{
    message_header_t* message_header = (message_header_t*) buffer;
    return message_header->type;
}
