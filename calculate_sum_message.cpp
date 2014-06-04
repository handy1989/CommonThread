#include "calculate_sum_message.h"

CalculateSumReqMessage::CalculateSumReqMessage()
{
    m_message_header.type = MESSAGE_CAL_SUM_REQ;   
}

CalculateSumReqMessage::~CalculateSumReqMessage()
{
    
}

bool CalculateSumReqMessage::parseBody()
{
    LOG(INFO) << "CalculateSumReqMessage::parseBody!";
    return true;
}

bool CalculateSumReqMessage::buildBody()
{
    LOG(INFO) << "CalculateSumReqMessage::buildBody!";
    return true;
}

CalculateSumRspMessage::CalculateSumRspMessage()
{
    m_message_header.type = MESSAGE_CAL_SUM_RSP;
}

CalculateSumRspMessage::~CalculateSumRspMessage()
{

}

bool CalculateSumRspMessage::parseBody()
{
    LOG(INFO) << "CalculateSumRspMessage::parseBody!";
    return true;
}

bool CalculateSumRspMessage::buildBody()
{
    LOG(INFO) << "CalculateSumReqMessage::buildBody!"; 
    return true;
}
