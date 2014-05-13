#include "message_factory.h"


MessageFactory::MessageFactory()
{
}


MessageFactory::~MessageFactory()
{
}

Message* MessageFactory::createMessage(int message_type)
{
    Message* message = NULL;
    LOG(INFO) << "message_type[" << message_type << "]";
    switch(message_type)
    {
        case MESSAGE_CAL_SUM_REQ:
            message = new CalculateSumResMessage();
            break;
        case MESSAGE_CAL_SUM_RSP:
            message = new CalculateSumRspMessage();
            break;
        default:
            LOG(INFO) << "message type is invalid!";
            break;
    }
    return message;
}

