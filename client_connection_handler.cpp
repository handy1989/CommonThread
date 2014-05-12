#include <time.h>
#include <errno.h>
#include <unistd.h>
#include "client_connection_handler.h"

ClientConnectionHandler::ClientConnectionHandler(int check_interval) : CommonThreadPool(1)
{
    m_check_internal = check_interval;
    m_last_check_point = 0;
    m_recv_sock_q = NULL; 
}

ClientConnectionHandler::~ClientConnectionHandler()
{

}

void ClientConnectionHandler::initQueues(ClientSocketQueue* recv_sock_q)
{
    m_recv_sock_q = recv_sock_q;
}

void ClientConnectionHandler::checkTimeout()
{
    // 这个条件表达式没明白
    if (m_cur_time < m_last_check_point || m_cur_time > (m_last_check_point + m_check_internal))
    {
        m_last_check_point = m_cur_time;
        int total_fd_count = EpollSocketManager::getInstance()->count();

        LOG(INFO) << "begin to checkTimeout! total_fd_count[" << total_fd_count << "]";

        int i = 0;
        SocketInfoManager it_end = EpollSocketManager::getInstance()->socketInfoEnd();
        for (SocketInfoManager it = EpollSocketManager::getInstance()->socketInfoBegin(); it != it_end; ++it)
        {
            if (i >= total_fd_count)
            {
                break;
            }
            if (it.inUsed())
            {
                ++i;
                if (!it.isListener())
                {
                    LOG(INFO) << "check socket[" << it.getSocketFd() << "] port[" << it.getPort() << "]";
                    if (it.timeout(m_cur_time))
                    {
                        LOG(INFO) << "socket[" << it.getSocketFd() << "] port[" << it.getPort() << "] timeout. Close it!";
                        EpollSocketManager::getInstance()->closeSocket(it.getSocketFd());
                    }
                    else
                    {
                        LOG(INFO) << "socket[" << it.getSocketFd() << "] port[" << it.getPort() << "] not timeout!";
                    }
                }
            }
        }
    }
}

void ClientConnectionHandler::work()
{
    int accept_socket_fd;
    int available_fd_count;
    
    struct sockaddr_in client_addr;
    while (true)
    {
        available_fd_count = EpollSocketManager::getInstance()->waitAvailableSocket();
        m_cur_time = Utility::getCurMileSecond();
        
        LOG(INFO) << "available socket num: " << available_fd_count;

        if (available_fd_count <= 0)
        {
            continue;
        }
        SocketInfoManager it_end = EpollSocketManager::getInstance()->socketAvaiEnd();
        for (SocketInfoManager it = EpollSocketManager::getInstance()->socketAvaiBegin(); it != it_end; ++it)
        {
            if (it.isListener())
            {
                LOG(INFO) << "listener available!";
                while (true)
                {
                    accept_socket_fd = it.acceptSocket(&client_addr);
                    if (accept_socket_fd > 0)
                    {
                        LOG(INFO) << "accept new fd[" << accept_socket_fd << "]";
                        struct linger ling = {1, 0};
                        setsockopt(accept_socket_fd, SOL_SOCKET, SO_LINGER, &linger, sizeof(linger));
                        if (EpollSocketManager::getInstance()->putIntoEpoll(accept_socket_fd, cur_time, it.listenPort(), client_addr.sin_port, client_addr.sin_addr.s_addr, it.conn_timeout_ms()))
                        {
                            LOG(INFO) << "sockfd[" < accept_socket_fd << "] put into epoll success!";
                        }
                        else
                        {
                            LOG(INFO) << "sockfd[" < accept_socket_fd << "] put into epoll failed, now close it!";
                            EpollSocketManager::getInstance()->closeSocket(accept_socket_fd);
                        }
                    }
                    else
                    {
                        LOG(INFO) << "accetp new fd failed!";
                        break;
                    }
                }
                else
                {
                    LOG(INFO) << "client available!";
                    if (it.inUsed())
                    {
                        if (!EpollSocketManager::getInstance()->removeSocket(it.getSocketFd()))
                        {
                            LOG(INFO) << "remove fd[" << it.getSocketFd() << "] from epoll failed!";
                            EpollSocketManager::getInstance()->closeSocket(it.getSocketFd());
                        }
                        else
                        {
                            processConnection(it.getSocketFd(), it.listenPort(), it.getAddr(), it.getPort(), it.conn_timeout_ms());
                        }
                    }
                    else
                    {
                        LOG(INFO) << "sockfd[" << it.getSocketFd() << "] not OK!";
                    }
                }
            }
        }
    }
}

void ClientConnectionHandler::processConnection(int new_fd, int listen_port, int ip_addr, int client_port, int conn_timeout_ms)
{
    LOG(INFO) << "processConnection fd[" << new_fd << "] listen_port[" << listen_port << "] ip_addr[" << ip_addr << "] client_port[" << client_port << "] conn_timeout_ms[" << conn_timeout_ms << "]";
    if (m_recv_sock_q->push(new_fd), -1)
    {
        LOG(INFO) << "push to queue success!";
    }
    else
    {
        EpollSocketManager::getInstance()->closeSocket(new_fd);
        LOG(INFO) << "push to queue failed!";
    }
}
