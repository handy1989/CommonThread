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
    return true;
}

bool CalculateSumReqMessage::buildBody()
{
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
    return true;
}

bool CalculateSumRspMessage::buildBody()
{
    return true;
}
