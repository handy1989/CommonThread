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
    int error_no = errno;
    LOG(INFO) << "recvData sock_fd[" << sock_fd << "]";

    if (0 == recv_return)
    {
        LOG(INFO) << "sock_fd[" << sock_fd << "] closed!";
        return SOCKET_ERROR_PEER_CLOSE;
    }
    else if(recv_return < 0)
    {
        if (error_no == EWOULDBLOCK || error_no == EAGAIN || error_no == ETIMEDOUT)
        {
            LOG(INFO) << "try again. error[" << strerror(errno) << "]";
            return SOCKET_ERROR_WAIT_TRY_AGAIN;
        }
        else
        {
            LOG(INFO) << "error[" << strerror(errno) << "]";
            return SOCKET_ERROR_UNKNOW_OCCUR;
        }
    }
    else
    {
        real_recv_len = recv_return;
        return SOCKET_ERROR_SUCCESS;
    }
}

int ClientReceiverHandler::recvCmd(SocketInfoManager& socket_info)
{
    char *recv_buffer = NULL;
    while (true)
    {
        int need_recv_bytes = socket_info.needRecvBytes();
        string& recv_buffer_string = socket_info.getBuffer();
        if (need_recv_bytes != 0)
        {
            recv_buffer = new char[need_recv_bytes];
        }
        else if (recv_buffer_string.size() == 0)
        {
            recv_buffer = new char[DEFAULT_RECV_LEN];
            socket_info.setNeedRecvBytes(DEFAULT_RECV_LEN);
            need_recv_bytes = DEFAULT_RECV_LEN;
        }
        else
        {
            return SOCKET_ERROR_UNKNOW_OCCUR;
        }

        int recv_return;
        int recv_result = recvData(socket_info.getSocketFd(), recv_buffer, need_recv_bytes, recv_return);
        if (recv_result != SOCKET_ERROR_SUCCESS)
        {
            if (recv_buffer)
            {
                delete []recv_buffer;
                recv_buffer = NULL;
            }
            return recv_result;
        }
        else
        {
            if(recv_buffer)
            {
                recv_buffer_string += string(recv_buffer,recv_return);
                delete [] recv_buffer;
                recv_buffer = NULL;
            }
            else
            {
                return SOCKET_ERROR_UNKNOW_OCCUR;
            }
            need_recv_bytes -= recv_return;
            socket_info.setNeedRecvBytes(need_recv_bytes);
            LOG(INFO) << "recv_buffer_size[" << recv_buffer_string.size() << "] need_recv_bytes[" << need_recv_bytes << "]";
        }
        return SOCKET_ERROR_SUCCESS;
    }
}

void ClientReceiverHandler::work()
{
    int socket_fd;
    int ret;
    LOG(INFO) << "client_receiver_handler work!";
    while (true)
    {
        socket_fd = -1;
        if (!m_socket_queue->pop(socket_fd, 0))
        {
            LOG(INFO) << "ClientSocketQueue pop failed!";
            continue;
        }
        if (socket_fd < 0)
        {
            LOG(INFO) << "ClientSocketQueue pop null!";
            continue;
        }
        LOG(INFO) << "client_receiver_handler pop socket_fd[" << socket_fd << "]";
        SocketInfoManager socket_info = EpollSocketManager::getInstance()->getSocketInfo(socket_fd);
        while (SOCKET_ERROR_SUCCESS == (ret =  recvCmd(socket_info)))
        {
            Message* message = MessageFactory::createMessage(Message::getMessageType(socket_info.getBuffer().c_str(), socket_info.getBuffer().size(), socket_info.taskSign()));
            if (message)
            {
                message->m_buffer = socket_info.getBuffer();
                message->m_ip_addr = socket_info.getAddr();
                message->m_listen_port = socket_info.getListenPort();
                message->m_client_port = socket_info.getPort();
                message->m_socket_fd = socket_info.getSocketFd();
                message->m_task_sign = socket_info.getTaskSign();

                if (!m_message_queue->push(message, -1))
                {
                    delete message;
                    message = NULL;
                    continue;
                }
            }
        }
        if (SOCKET_ERROR_WAIT_TRY_AGAIN == ret)
        {
            if (!EpollSocketManager::getInstance()->putIntoEpoll(socket_info.getSocketFd(), Utility::getCurMilliSeconds(), socket_info.getListenPort(), socket_info.getPort(), socket_info.getAddr(), socket_info.connTimeoutMs(), SOCKET_TYPE_CLIENT))
            {
                EpollSocketManager::getInstance()->closeSocket(socket_info.getSocketFd());
            }
            continue;
        }
        else
        {
            LOG(INFO) << "delete socket_fd[" << socket_info.getSocketFd() << "]";
            EpollSocketManager::getInstance()->closeSocket(socket_info.getSocketFd());
            continue;
        }
    }
}

void ClientReceiverHandler::initQueues(ClientSocketQueue* socket_queue, MessageQueue* message_queue)
{
    m_socket_queue = socket_queue;
    m_message_queue = message_queue;
}
