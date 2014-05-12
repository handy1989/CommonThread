#ifndef _EPOLL_SOCKET_MANAGER_H_
#define _EPOLL_SOCKET_MANAGER_H_

#include "common_queue.h"
#include "epoll_socket.h"

class EpollSocketManager
{
public:
    EpollSocketManager();
    ~EpollSocketManager();

    static EpollSocketManager* getInstance();
    
    bool initSocket(netaddres_info_t *netaddr, const int& addr_num, const int& wait_time_ms);
    bool putIntoEpoll(int fd, int cur_time, int listen_port, int client_port, int ip_addr, int sock_type);
    int waitAvailableSocket();
    int count();
    SocketAvaiManager socketAvaiBegin();
    SocketAvaiManager socketAvaiEnd();
    SocketInfoManager socketInfoBegin();
    SocketInfoManager socketInfoEnd();
    SocketInfoManager getSocketInfo(int sock_fd);
    
    bool timeout(int cur_time_ms);
    bool removeSocket(int sock_fd);
    void closeSocket(int sock_fd);

private:
    static EpollSocketManager* m_epoll_socket_manager_instance;
    EpollSocket m_epoll_socket;
};

typedef CommonQueue<int> ClientSocketQueue;

#endif

