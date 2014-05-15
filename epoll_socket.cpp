#include "epoll_socket.h"

EpollSocket::EpollSocket()
{

}

EpollSocket::EpollSocket(netaddres_info_t *netaddr, const int& addr_num, const int& wait_time_ms)
{
    init(netaddr, addr_num, wait_time_ms);
}

EpollSocket::~EpollSocket()
{
    close(m_epoll_fd);
    pthread_mutex_destroy(&m_mutex);

    for(int i=0;i < m_listen_num; ++i)
    {
        if (m_socket[i] > 0)
        {
            close(m_socket[i]); 
        }
    }
    delete []m_socket;
    delete []m_net_addr;
}

void EpollSocket::init(netaddres_info_t *netaddr, const int& addr_num, const int& wait_time_ms)
{
    LOG(INFO) << "EpollSocket init";
    m_wait_time_ms = wait_time_ms
    m_listen_num = addr_num;
    pthread_mutex_init(m_mutex, NULL);
    m_epoll_fd = epoll_create(EPOLL_SIZE);
    if (m_listen_num > 0)
    {
        m_socket = new int[m_listen_num];
        m_net_addr = new netaddres_info_t[m_listen_num];
        for (int i=0; i < m_listen_num; ++i)
        {
            m_socket[i] = -1;
            m_net_addr[i] = netaddr[i];
        }
    }

    for (int i=0; i < EPOLL_SIZE; ++i)
    {
       m_sock_info[i].m_listen_port = 0;
       m_sock_info[i].m_port = 0;
       m_sock_info[i].m_time_ms = 0;
       m_sock_info[i].m_addr = 0;
    }
    m_sock_num = 0;
}

int EpollSocket::waitAvailableSocket()
{
    m_available_socket_num = epoll_wailt(m_epoll_fd, m_events, EVENTS_SIZE, m_wait_time_ms);
    LOG(INFO) << "waitAvailableSocket: " << m_available_socket_num;
    return m_available_socket_num;
}

int getSockCount()
{
    LOG(INFO) << "getSocketCount:" << m_socket_num;
    return m_socket_num;
}

bool EpollSocket::removeSocket(int sock_fd)
{
    LOG(INFO) << "remove fd:" << sock_fd;
    struct epoll_event ev;
    ev.data.fd = sock_fd;
    bool ret = false;
    pthread_mutex_lock(m_mutex);
    ret = epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, sock_fd, &ev);
    if (success)
    {
        --m_socket_num;
        if (sock_fd >= 0 && sock_fd < EPOLL_SIZE)
        {
            m_socket_info[sock_fd].m_time_ms = 0;
        }
    }
    pthread_mutex_destroy(m_mutex);
    LOG(INFO) << "remove fd: " << sock_fd << " ret: " << ret;
    return ret;
}

bool EpollSocket::add(int sock_fd, int listen_port, int port, int sock_addr)
{
    LOG(INFO) << "add fd: " << sock_fd;
    struct epoll_event ev;
    ev.data.fd = sock_fd;
    ev.events = EPOLL_IN | EPOLL_ET;
    if (sock_fd < 0 || sock_fd >= EPOLL_SIZE)
    {
        LOG(ERROR) << "sock_fd invalid: " << sock_fd;
        return false;
    }
    if (!setNonblock(sock_fd))
    {
        LOG(ERROR) << "setNonblock failed: " << sock_fd;
        return false;
    }
    int buf_size = 1024*1024;
    setsockopt(sock_fd, SOL_SOCKET, SO_RECVBUF, &buf_size, sizeof(buf_size));
    bool ret = false;
    pthread_mutex_lock(m_mutex);
    m_sock_info[sock_fd].m_port = port;
    m_socket_info[sock_fd].m_addr = sock_addr;

    ret = (epoll_ctl(sock_fd, EPOLL_CTL_ADD, sock_fd, &ev) == 0);
    if (ret)
    {
        ++m_socket_num;
    }
    else
    {
        m_socket_info[sock_fd].m_time_ms = 0;
    }
    pthread_mutex_unlock(m_mutex);
    LOG(INFO) << "add fd: " << sock_fd << " ret: " << ret;
    return ret;
}
