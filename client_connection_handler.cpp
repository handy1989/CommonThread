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

    while (true)
    {
        available_fd_count = EpollSocketManager::getInstance()->waitAvailableSocket();
        
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
