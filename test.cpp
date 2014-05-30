#include <stdio.h>
#include "epoll_socket_manager.h"
#include "network_manager.h"

int main(int argc, char *argv[])
{
    google::InitGoogleLogging(argv[0]);
    FLAGS_log_dir = "./log";

    netaddres_info_t netaddr[1];
    netaddr[0].m_port = 6666;
    netaddr[0].m_ip_addr = "127.0.0.1";
    netaddr[0].m_backlog = 100;

//    EpollSocketManager::getInstance()->initSocket(netaddr, 1, 0);
    //LOG(INFO) << "socket num: " << EpollSocketManager::getInstance()->count();
    NetworkManager::getInstance()->initSocket(netaddr,1,100);
    NetworkManager::getInstance()->setQueueSize(100);
    NetworkManager::getInstance()->initClientRecv(10,10);
    NetworkManager::getInstance()->start();
    NetworkManager::getInstance()->waitThreadsTermination();
    return 0;
}

