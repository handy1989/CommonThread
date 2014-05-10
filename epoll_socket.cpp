#include "epoll_socket.h"

EpollSocket::EpollSocket()
{

}

EpollSocket::EpollSocket(netaddres_info_t *netaddr, const int& addr_num)
{
    init(netaddr, addr_num);
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

void EpollSocket::init(netaddres_info_t *netaddr, const int& addr_num)
{
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
    }
    m_sock_num = 0;
}

