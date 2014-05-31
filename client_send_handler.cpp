#include "client_send_handler.h"

#include <sys/poll.h>
#include "send_socket.h"

ClientSendHandler::ClientSendHandler(int pool_size) : CommonThreadPool(pool_size)
{
    m_revc_message = 0;
    m_send_succ = 0;
    m_send_fail = 0;
}


ClientSendHandler::~ClientSendHandler()
{
}

void ClientSendHandler::initQueues(MessageQueue* message_send_queue)
{
    m_message_send_queue = message_send_queue;
}

bool ClientSendHandler::sendData(int socket_fd,const char* data, int data_len)
{
    int send_bytes = 0;
    while(data_len > 0)
    {
        send_bytes = send(socket_fd, data, data_len, MSG_NOSIGNAL);
        if(send_bytes <= 0)
        {
            LOG(INFO) << "send error[" << errno << "] : " << strerror(errno) << " when send data from socket[" << socket_fd << "]";
            return false;
        }

        data += send_bytes;
        data_len -= send_bytes;
    }
    return true;
}

void ClientSendHandler::work()
{
    Message* message;

    while (true)
    {
        message = NULL;
        if  (!m_message_send_queue->pop(message, 0)) 
        {
            LOG(ERROR) << "MessageQueue::pop() FAILED!"; 
            continue;
        }
        if (message == NULL) 
        {
            LOG(ERROR) << "MessageQueue::pop() NULL!";
            continue;
        }

        m_revc_message++;
        LOG(INFO) << "get message, fd[" << message->m_socket_fd << "]";

        SocketInfoManager socket_info = EpollSocketManager::getInstance()->getSocketInfo(message->m_socket_fd);
        if(socket_info.isOk() 
                && socket_info.getAddr() == message->m_ip_addr 
                && socket_info.getPort() == message->m_client_port)
        {
            if(SendSocket::ssendData(socket_info.getSocketFd(),message->m_buffer.c_str(),message->m_buffer.size()))
            {
                m_send_succ++;
                LOG(INFO) << "send data succ!";
            }
            else
            {
                //不删除socket，让接收线程进行关闭，如果发送线程关闭socket
                //会导致接收线程异常
                m_send_fail++;
                LOG(INFO) << "send failed!";
            }
        }
        else
        {
            m_send_fail++;
            LOG(INFO) << "socket_info.addr[" << socket_info.getAddr() << "] message->m_ip_addr[" << message->m_ip_addr << "]";
            LOG(INFO) << "socket_info.port[" << socket_info.getPort() << "] message->m_client_port[" << message->m_client_port << "]";
        }
        delete message;
    }
}

