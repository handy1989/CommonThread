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

enum SocketType
{
    SOCKET_TYPE_UNKNOWN = 0,
    SOCKET_TYPE_SERVER,
    SOCKET_TUPE_CLIENT
};

typedef struct _sock_info_t
{
    int m_listen_port;
    int m_port;
    int m_addr;
    int m_time_ms;
    int m_conn_timeout_ms;
    int m_type;
    bool m_closed;
    int m_need_recv_bytes;
    std::string m_recv_buf;

}sock_info_t;

typedef struct _netaddres_info_t
{
    std::string m_ip_addr;
    int m_port;
    int m_backlog;

}netaddres_info_t;

class SocketAvaiManager;
class SocketInfoManager;

class EpollSocket
{
    static const int EPOLL_SIZE = 6000;
    static const int EVENTS_SIZE = EPOLL_SIZE;
public:
    friend class SocketAvaiManager;
    friend class SocketInfoManager;
    EpollSocket();
    EpollSocket(netaddres_info_t *netaddr, const int& addr_num, const int& wait_time_ms);
    ~EpollSocket();
    void init(netaddres_info_t *netaddr, const int& addr_num, const int& wait_time_ms);
    int waitAvailableSocket();
    int getSocketCount();
    bool removeSocket(int sock_fd);
    bool add(int sock_fd, int cur_time, int listen_port, int port, int sock_addr, int sock_type);
    bool bindPort();
    bool listenPort();
    int count();
    bool setNonblock(int& sock_fd);
    int getTime(int socket_fd);

    SocketAvaiManager socketAvaiBegin();
    SocketAvaiManager socketAvaiEnd();
    SocketInfoManager socketInfoBegin();
    SocketInfoManager socketInfoEnd();
    SocketInfoManager socketInfo(int sock_fd);
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

class SocketAvaiManager
{
public:
    SocketAvaiManager(int index, EpollSocket* epoll_socket_ptr) : m_fd_index(index), m_epoll_socket_ptr(epoll_socket_ptr)
    {
        m_socket_fd = m_epoll_socket_ptr->m_events[m_fd_index].data.fd;
    }
    int getSocketFd()
    {
        return m_socket_fd;
    }
    int getAddr()
    {
        return m_epoll_socket_ptr->m_socket_info[m_socket_fd].m_addr;
    }
    int getPort()
    {
        return m_epoll_socket_ptr->m_socket_info[m_socket_fd].m_port;
    }
    int getListenPort()
    {
        return m_epoll_socket_ptr->m_socket_info[m_socket_fd].m_listen_port;
    }
    bool available()
    {
        return (m_epoll_socket_ptr->m_events[m_fd_index].events & EPOLLIN);
    }
    bool operator != (SocketAvaiManager socket_avai_manager)
    {
        return m_fd_index != socket_avai_manager.m_fd_index;
    }
    SocketAvaiManager& operator ++ ()
    {
        ++m_fd_index;
        m_socket_fd = m_epoll_socket_ptr->m_events[m_fd_index].data.fd;
        return *this;
    }
    int acceptSocket(struct sockaddr_in* client_addr)
    {
        socklen_t sock_len = sizeof(sockaddr_in);
        return accept(m_socket_fd, (struct sockaddr*)client_addr, &sock_len);
    }
    bool isListener()
    {
        return (m_epoll_socket_ptr->m_socket_info[m_socket_fd].m_type == SOCKET_TYPE_SERVER);
    }
    bool isWritable()
    {
        return (m_epoll_socket_ptr->m_events[m_fd_index].events & EPOLLOUT);
    }
    void close()
    {
        m_epoll_socket_ptr->m_socket_info[m_socket_fd].m_time_ms = 0;
        m_epoll_socket_ptr->m_socket_info[m_socket_fd].m_closed = true;
        LOG(INFO) << "close fd events[" << m_fd_index << "] = " << m_socket_fd;

    }
    bool isOk()
    {
        return m_epoll_socket_ptr->m_socket_info[m_socket_fd].m_closed == false;
    }
    bool inUsed()
    {
        return m_epoll_socket_ptr->m_socket_info[m_socket_fd].m_time_ms != 0;
    }
private:
    int m_fd_index;
    int m_socket_fd;
    EpollSocket* m_epoll_socket_ptr;
};

class SocketInfoManager
{
public:
    SocketInfoManager(int index, EpollSocket* epoll_socket_ptr) : m_fd_index(index), m_epoll_socket_ptr(epoll_socket_ptr)
    {
    
    }
    std::string dump()
    {
        char buffer[1024];
        snprintf(buffer, sizeof(buffer), "fd[%d] port[%d] listen_port[%d] time[%d] type[%d]", getSocketFd(), getPort(), getListenPort(), m_epoll_socket_ptr->m_socket_info[m_fd_index].m_time_ms, m_epoll_socket_ptr->m_socket_info[m_fd_index].m_type);
        return buffer;
    }
    int getSocketFd()
    {
        return m_fd_index;
    }
    int getPort()
    {
        return m_epoll_socket_ptr->m_socket_info[m_fd_index].m_port;
    }
    int getListenPort()
    {
        return m_epoll_socket_ptr->m_socket_info[m_fd_index].m_listen_port;
    }
    int getAddr()
    {
        return m_epoll_socket_ptr->m_socket_info[m_fd_index].m_addr;
    }
    bool inUsed()
    {
        return m_epoll_socket_ptr->m_socket_info[m_fd_index].m_time_ms != 0;
    }
    bool isListener()
    {
        return m_epoll_socket_ptr->m_socket_info[m_fd_index].m_type == SOCKET_TYPE_SERVER;
    }
    bool operator != (SocketInfoManager socket_info_manager)
    {
        return m_fd_index != socket_info_manager.m_fd_index;
    }
    SocketInfoManager& operator ++ ()
    {
        ++m_fd_index;
        return *this;
    }
    void close()
    {
        m_epoll_socket_ptr->m_socket_info[m_fd_index].m_time_ms = 0;
        m_epoll_socket_ptr->m_socket_info[m_fd_index].m_closed = true;
        m_epoll_socket_ptr->m_socket_info[m_fd_index].m_recv_buf = "";
        m_epoll_socket_ptr->m_socket_info[m_fd_index].m_need_recv_bytes = 0;
        LOG(INFO) << "closed fd[" << m_fd_index << "]";
    }
    bool isOk()
    {
        return m_epoll_socket_ptr->m_socket_info[m_fd_index].m_closed == false;    
    }
    std::string& getBuffer()
    {
        return m_epoll_socket_ptr->m_socket_info[m_fd_index].m_recv_buf;
    }
    int needRecvBytes()
    {
        return m_epoll_socket_ptr->m_socket_info[m_fd_index].m_need_recv_bytes;
    }
    void setNeedRecvBytes(int need_bytes)
    {
        m_epoll_socket_ptr->m_socket_info[m_fd_index].m_need_recv_bytes = need_bytes;
    }
    void flushBuffer()
    {
        m_epoll_socket_ptr->m_socket_info[m_fd_index].m_recv_buf = "";
    }
private:
    int m_fd_index;
    EpollSocket* m_epoll_socket_ptr;
};

#endif
