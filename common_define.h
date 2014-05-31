#ifndef _COMMON_DEFINE_H_
#define _COMMON_DEFINE_H_

#define MSG_SIGN 1

#define MESSAGE_CAL_SUM_REQ 100
#define MESSAGE_CAL_SUM_RSP 200


const int32_t ERROR_SUCCESS = 0;
const int32_t ERROR_GENERAL_ERROR = 1;
const int32_t ERROR_DECRYPT_ERROR = 2;
const int32_t ERROR_ENCRYPT_ERROR = 3;
const int32_t ERROR_TIMEOUT_ERROR = 4;
const int32_t ERROR_WATERMARK_ERROR = 5;

const int32_t ERROR_PARSE_MESSAGE_ERROR = 1001;
const int32_t ERROR_BUILD_MESSAGE_ERROR = 1002;

typedef struct _message_header_t
{
    int type;
    int sequence;
}message_header_t;

#endif
