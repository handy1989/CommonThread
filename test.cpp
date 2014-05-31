#include <stdio.h>
#include "epoll_socket_manager.h"
#include "network_manager.h"
#include "server_msg_handler.h"

ServerMsgHandler* m_cmd_handler;

int main(int argc, char *argv[])
{
    google::InitGoogleLogging(argv[0]);
    FLAGS_log_dir = "./log";

    netaddres_info_t netaddr[1];
    netaddr[0].m_port = 2222;
    netaddr[0].m_ip_addr = "127.0.0.1";
    netaddr[0].m_backlog = 100;

//    EpollSocketManager::getInstance()->initSocket(netaddr, 1, 0);
    //LOG(INFO) << "socket num: " << EpollSocketManager::getInstance()->count();
    NetworkManager::getInstance()->initSocket(netaddr,1,100);
    NetworkManager::getInstance()->setQueueSize(100);
    NetworkManager::getInstance()->initClientRecv(10,10);

    m_cmd_handler = new ServerMsgHandler(1);

    NetworkManager::getInstance()->start();
    m_cmd_handler->startThreads();

    NetworkManager::getInstance()->waitThreadsTermination();
    return 0;
}

