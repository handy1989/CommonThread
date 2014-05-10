#ifndef _EPOLL_SOCKET_H_
#define _EPOLL_SOCKET_H_

typedef _sock_info_t
{
    int m_insten_port;
    int m_port;
    string m_recv_buf;

}sock_info_t;

typedef _netaddres_info_t
{
    string m_ip_addr;
    int m_port;

}netaddres_info_t;

class EpollSocket
{
    static const int EPOLL_SIZE = 6000;
    static const int EVENTS_SIZE = EPOLL_SIZE;
public:
    EpollSocket();
    EpollSocket(netaddres_info_t *netaddr, const int& addr_num);
    void init(netaddres_info_t *netaddr, const int& addr_num);
    int wait_available_socket();
    bool remove_socket(int sock_fd);
    bool add(int sock_fd, int listen_port, int port, int sock_addr);
    bool bind_port();
    bool listen_port();
    bool set_nonblock();
protected:
    int get_port(int sock_fd);
    int get_addr(int sock_fd);

private:
    int m_listen_num;
    int m_epoll_fd;
    int *m_socket;
    netaddres_info_t *m_net_addr;
    pthread_mutex_t m_mutex;
    sock_info_t m_sock_info[EPOLL_SIZE];
    struct epoll_event m_events[EVENTS_SIZE];
};

#endif
