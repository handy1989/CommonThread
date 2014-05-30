#ifndef _CALCULATE_SUM_MESSAGE_H_
#define _CALCULATE_SUM_MESSAGE_H_

#include "message.h"
#include "common_define.h"

class CalculateSumReqMessage : public Message
{
public:
    CalculateSumReqMessage();
    ~CalculateSumReqMessage();
    virtual void setHeaderControl()
    {
    
    }
    virtual bool parseBody();
    virtual bool buildBody();
private:
    double para1;
    double para2;
};

class CalculateSumRspMessage : public Message
{
public:
    CalculateSumRspMessage();
    ~CalculateSumRspMessage();
    virtual void setHeaderControl()
    {
    
    }
    virtual bool parseBody();
    virtual bool buildBody();

private:
    double m_result;
};
#endif
