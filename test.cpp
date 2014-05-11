#include <stdio.h>
#include "epoll_socket.h"

int main(int argc, char *argv[])
{
    google::InitGoogleLogging(argv[0]);
    FLAGS_log_dir = "./log";

    netaddres_info_t netaddr[1];
    netaddr[0].m_port = 6666;
    netaddr[0].m_ip_addr = "127.0.0.1";
    netaddr[0].m_backlog = 100;

    EpollSocket *epoll_socket = new EpollSocket(netaddr, 1, 10000);
    epoll_socket->bindPort();
    epoll_socket->listenPort();
    return 0;
}

