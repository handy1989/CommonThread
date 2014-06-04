#ifndef _CALCULATE_SUM_MESSAGE_H_
#define _CALCULATE_SUM_MESSAGE_H_

#include <glog/logging.h>
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

    double getPara1()
    {
        return para1;
    }
    double getPara2()
    {
        return para2;
    }
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
    void setResult(double para1, double para2)
    {
        m_result = para1 + para2;
    }
    double getResult()
    {
        return m_result;
    }

private:
    double m_result;
};
#endif
