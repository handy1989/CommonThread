#include "epoll_socket_manager.h"

EpollSocketManager* EpollSocketManager::m_epoll_socket_manager_instance = new EpollSocketManager();

EpollSocketManager::EpollSocketManager()
{

}

EpollSocketManager::~EpollSocketManager()
{

}

EpollSocketManager* EpollSocketManager::getInstance()
{
    return m_epoll_socket_manager_instance;
}

bool EpollSocketManager::initSocket(netaddres_info_t *netaddr, const int& addr_num, const int& wait_time_ms)
{
    LOG(INFO) << "EpollSocketManager init begin!";
    m_epoll_socket.init(netaddr, addr_num, wait_time_ms);
    if (!m_epoll_socket.bindPort())
    {
        return false;
    }
    if (!m_epoll_socket.listenPort())
    {
        return false;
    }
    LOG(INFO) << "EpollSocketManager init success!";
    return true;
}


bool EpollSocketManager::putIntoEpoll(int fd, int cur_time,  int listen_port, int client_port, int ip_addr, int sock_type)
{
    if (!m_epoll_socket.add(fd, cur_time, listen_port, client_port, ip_addr, sock_type))
    {
        LOG(ERROR) << "add fd[" << fd << "] to epoll failed. error info: " << strerror(errno);
        return false;
    }
    else
    {
        LOG(INFO) << "add fd[" << fd << "] to epoll success! now count is : " << m_epoll_socket.count();
        return true;
    }
}

int EpollSocketManager::waitAvailableSocket()
{
    return m_epoll_socket.waitAvailableSocket();
}

SocketInfoManager EpollSocketManager::getSocketInfo(int sock_fd)
{
    return m_epoll_socket.socketInfo(sock_fd);
}

SocketAvaiManager EpollSocketManager::socketAvaiBegin()
{
    return m_epoll_socket.socketAvaiBegin();
}

SocketAvaiManager EpollSocketManager::socketAvaiEnd()
{
    return m_epoll_socket.socketAvaiEnd();
}

SocketInfoManager EpollSocketManager::socketInfoBegin()
{
    return m_epoll_socket.socketInfoBegin();
}

SocketInfoManager EpollSocketManager::socketInfoEnd()
{
    return m_epoll_socket.socketInfoEnd();
}

bool EpollSocketManager::removeSocket(int sock_fd)
{
    return m_epoll_socket.removeSocket(sock_fd);
}

int EpollSocketManager::count()
{
    LOG(INFO) << "EpollSocketManager::count called!";
    return m_epoll_socket.count();
}

void EpollSocketManager::closeSocket(int sock_fd)
{
    if (sock_fd < 0)
    {
        LOG(ERROR) << "invalid sock_fd: " << sock_fd;
    }
    else
    {
        SocketInfoManager sock_info = m_epoll_socket.socketInfo(sock_fd);
        close(sock_fd);
        sock_info.close();
        LOG(INFO) << "close sock_fd[" << sock_fd << "]";
    }
}

