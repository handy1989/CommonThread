#ifndef _COMMON_DEFINE_H_
#define _COMMON_DEFINE_H_

#define MSG_SIGN 1

#define MESSAGE_CAL_SUM_REQ 100
#define MESSAGE_CAL_SUM_RSP 200
typedef struct _message_header_t
{
    int type;
    int sequence;
}message_header_t;

#endif
