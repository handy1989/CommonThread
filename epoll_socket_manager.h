#ifndef _EPOLL_SOCKET_MANAGER_H_
#define _EPOLL_SOCKET_MANAGER_H_

#include "epoll_socket.h"

class EpollSocketManager
{
public:
    EpollSocketManager();
    ~EpollSocketManager();

    static EpollSocketManager* getInstance();
    
    bool initSocket(netaddres_into_t *netaddr, const int& addr_num);
    int get_conn_socket_count();
    bool putIntoEpoll(int fd, int listen_port, int client_port, int ip_addr);
    int wait_available_socket();
    bool remove(int sock_fd);
    void closeSocket(int sock_fd);

private:
    static EpollSocketManager* m_epoll_socket_manager_instance;
    EpollSocket m_epoll_socket;
};

#endif

