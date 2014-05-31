#include "server_send_socket.h"

ServerSendSocket::ServerSendSocket()
{
}

ServerSendSocket::~ServerSendSocket()
{
}

void ServerSendSocket::initSocket(const char * ip,const short& port,const int& send_timeout_sec,const int& send_buf_size)
{
	m_send_socket.init(ip,port,send_timeout_sec,send_buf_size);
}

bool ServerSendSocket::connectServer(const uint32_t& conn_timeout_ms)
{
	if(m_send_socket.connectTo())
	{
        LOG(INFO) << "connect succ!";
		if(!EpollSocketManager::getInstance()->putIntoEpoll(m_send_socket.m_send_socket_fd, Utility::getCurMilliSeconds(), 2222, m_send_socket.m_port, m_send_socket.m_ip, conn_timeout_ms,SOCKET_TYPE_CLIENT))
		{
			EpollSocketManager::getInstance()->closeSocket(m_send_socket.m_send_socket_fd);
			m_send_socket.m_send_socket_fd = -1;
            LOG(INFO) << "connect fail!";
			return false;
		}
	}
	else
	{
        LOG(INFO) << "reconnect fail!";
		return false;
	}
	return true;
}

bool ServerSendSocket::sendData(const Message* message,const int& timeout_ms)
{
	if(m_send_socket.isConnected())
	{
		SocketInfoManager socket_info = EpollSocketManager::getInstance()->getSocketInfo(m_send_socket.m_send_socket_fd);
		if(socket_info.isOk() && 
			socket_info.getAddr() == m_send_socket.m_ip &&
			socket_info.getPort() == m_send_socket.m_port)
		{
		}
		else
		{
            LOG(INFO) << "send socket is error, fd[" << m_send_socket.m_send_socket_fd << "]";
			if(!connectServer(timeout_ms*3))
			{
				return false;
			}
		}
	}
	else
	{
		if(!connectServer(timeout_ms*3))
		{
			return false;
		}
	}


	if(!SendSocket::ssendData(m_send_socket.get_sock_fd(),message->m_buffer.c_str(),message->m_buffer.size(),timeout_ms))
	{
        LOG(INFO) <<  "send data (len="<<message->m_buffer.size()<<") to rootserver("<<m_send_socket.get_ip()<<":"<<m_send_socket.get_port()<<") FAIL,socket:"<<m_send_socket.get_sock_fd();
		if(!connectServer(timeout_ms*3))
		{
			return false;
		}
		return false;
	}
	else
	{
        LOG(INFO) << "send data (len="<<message->m_buffer.size()<<") to rootserver("<<m_send_socket.get_ip()<<":"<<m_send_socket.get_port()<<") SUCCESS,socket:"<<m_send_socket.get_sock_fd();
	}
	
	return true;
}


