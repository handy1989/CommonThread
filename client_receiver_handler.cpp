#include "client_receiver_handler.h"

ClientReceiverHandler::ClientReceiverHandler(int thread_count) : CommonThreadPool(thread_count)
{
    m_recv_message_num = 0;
    m_push_queue_succ = 0;
    m_push_queue_fail = 0;
}

ClientReceiverHandler::~ClientReceiverHandler()
{
    
}

int ClientReceiverHandler::recvData(int sock_fd, char *recv_buffer, const int need_recv_bytes, int& real_recv_len)
{
    int recv_return = recv(sock_fd, recv_buffer, need_recv_bytes, 0);
    int errno_no = errno;
    LOG(INFO) << "recvData sock_fd[" << sock_fd << "]";

    if (0 == recv_return)
    {
        LOG(INFO) << "sock_fd[" << sock_fd << "] closed!";
        return SOCK_ERROR_PEER_CLOSED;
    }
    else if(recv_return < 0)
    {
        if (error_no == EWOULDBLOCK || error_no == EAGAIN || error_no == ETIMEDOUT)
        {
            LOG(INFO) << "try again. error[" << strerrno(errno) << "]";
            return SOCKET_ERROR_WAIT_TRY_AGAIN;
        }
        else
        {
            LOG(INFO) << "error[" << strerrno(errno) << "]";
            return SOCKET_ERROR_UNKNOW_OCCUR;
        }
    }
    else
    {
        real_recv_len = recv_return;
        return SOCKET_ERROR_SUCCESS;
    }
}
