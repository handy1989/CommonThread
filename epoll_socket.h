#ifndef _EPOLL_SOCKET_H_
#define _EPOLL_SOCKET_H_

#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <string>
#include <glog/logging.h>

typedef struct _sock_info_t
{
    int m_listen_port;
    int m_port;
    int m_addr;
    int m_time_ms;
    std::string m_recv_buf;

}sock_info_t;

typedef struct _netaddres_info_t
{
    std::string m_ip_addr;
    int m_port;
    int m_backlog;

}netaddres_info_t;

class EpollSocket
{
    static const int EPOLL_SIZE = 6000;
    static const int EVENTS_SIZE = EPOLL_SIZE;
public:
    EpollSocket();
    EpollSocket(netaddres_info_t *netaddr, const int& addr_num, const int& wait_time_ms);
    ~EpollSocket();
    void init(netaddres_info_t *netaddr, const int& addr_num, const int& wait_time_ms);
    int waitAvailableSocket();
    int getSocketCount();
    bool removeSocket(int sock_fd);
    bool add(int sock_fd, int listen_port, int port, int sock_addr);
    bool bindPort();
    bool listenPort();
    bool setNonblock(int& sock_fd);
protected:
    int getPort(int sock_fd);
    int getAddr(int sock_fd);

private:
    int m_listen_num;
    int m_epoll_fd;
    int m_socket_num;
    int m_available_socket_num;
    int m_wait_time_ms;
    int *m_socket;
    netaddres_info_t *m_net_addr;
    pthread_mutex_t m_mutex;
    sock_info_t m_socket_info[EPOLL_SIZE];
    struct epoll_event m_events[EVENTS_SIZE];
};

#endif
