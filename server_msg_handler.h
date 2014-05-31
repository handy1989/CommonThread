#ifndef _SERVER_MSG_HANDLER_H_
#define _SERVER_MSG_HANDLER_H_

#include <unistd.h>
#include "common_thread_pool.h"
#include "message_factory.h"

#include "network_manager.h"

class ServerMsgHandler : public CommonThreadPool
{
    public:
    ServerMsgHandler(int thread_count);
    virtual ~ServerMsgHandler();

    protected:
    virtual void work();

    private:

    int calculateSum(Message* message,Message*& rsp_message);
};


#endif
