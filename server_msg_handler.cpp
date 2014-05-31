#include "server_msg_handler.h"

ServerMsgHandler::ServerMsgHandler(int thread_count) : CommonThreadPool(thread_count)
{
}

ServerMsgHandler::~ServerMsgHandler()
{
}

int ServerMsgHandler::calculateSum(Message* message,Message*& rsp_message)
{
    LOG(INFO) << "begin calculate sum!";
    CalculateSumReqMessage* cal_sum_message = (CalculateSumReqMessage*)message;
    if(!cal_sum_message->parseBody())
    {
        LOG(INFO) << "parse cal_sum_message failed!";
        return ERROR_PARSE_MESSAGE_ERROR;
    }
    LOG(INFO) << "CalculateSumReqMessage para1[" << cal_sum_message->getPara1() << "] para2[" << cal_sum_message->getPara2() << "]";

    CalculateSumRspMessage* cal_sum_rsp_message = new CalculateSumRspMessage();
    cal_sum_rsp_message->setResult(cal_sum_message->getPara1(), cal_sum_message->getPara2());

    rsp_message = cal_sum_rsp_message;
    return ERROR_SUCCESS;

}


void ServerMsgHandler::work()
{
    Message* message;
    Message* rsp_message;

    while (true)
    {
        message = NULL;
        rsp_message = NULL;
        if(!NetworkManager::messagePop(message, 0))
        {
            LOG(INFO) << "MessageQueue::pop() FAILED!";
            sleep(5);
            continue;
        }
        if (message == NULL) 
        {
            LOG(INFO) << "MessageQueue::pop NULL!";
            continue;
        }

        if(!message->parseHeader())
        {
            delete message;
            message = NULL;
            continue;
        }
        
        LOG(INFO) << "get message, socket: " << message->m_socket_fd << "message type: " << message->messageType();
        int ret = -1;
        
        switch(message->messageType())
        {
            case MESSAGE_CAL_SUM_REQ :
                ret = calculateSum(message,rsp_message);
                break;
            default:
                break;

        }
        LOG(INFO) << "ret[" << ret << "]";

        if(ret == ERROR_SUCCESS)
        {
            if(rsp_message != NULL)
            {
                LOG(INFO) << "cmd success, while rsp message!";

                if(!rsp_message->buildMessage(message))
                {
                    delete rsp_message;
                    rsp_message = NULL;
                }
                else
                {
                    LOG(INFO) << "seq: " << rsp_message->messageSequence();
                    LOG(INFO) << "message_type: " << rsp_message->messageType(); 
                    LOG(INFO) << "m_listen_port: " << rsp_message->m_listen_port;

                    if(rsp_message->m_listen_port == 2222)
                    {
                        if(!NetworkManager::clientLongConnectPush(rsp_message, -1))
                        {
                            LOG(INFO) << "ClientLongConnectionSendManager::push() FAILED!";
                            delete rsp_message;
                        }
                        else
                        {
                            LOG(INFO) << "ClientLongConnectionSendManager::push() SUCC!";
                        }
                    }
                    else
                    {

                    }
                    rsp_message = NULL;
                }
            }
        }

        delete message;
    }
}

