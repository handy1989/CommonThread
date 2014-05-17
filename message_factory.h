#ifndef _MESSAGE_FACTORY_H_
#define _MESSAGE_FACTORY_H_

#include <glog/logging.h>

#include "message.h"
#include "calculate_sum_message.h"

class MessageFactory
{
    public:
    MessageFactory();
    ~MessageFactory();

    static Message* createMessage(int message_type);
};



#endif
