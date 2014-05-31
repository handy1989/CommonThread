#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <string>
#include <sys/poll.h>

#include "send_socket.h"
#include "common_utility.h"

SendSocket::
	SendSocket()
{
	m_send_socket_fd = -1;
	m_reconnect = 0;
}

SendSocket::
	~SendSocket()
{
}

bool SendSocket::
	isConnected()
{
	return m_send_socket_fd > 0;
}

bool SendSocket::
	isReconnect()
{
	return (m_reconnect == 1);
}

const char* SendSocket::
	get_ip()
{
	return m_ip_string;
}

int SendSocket::
	get_sock_fd()
{
	return m_send_socket_fd;
}

short SendSocket::
	get_port()
{
	return m_port;
}

bool SendSocket::
	init(const char * ip, const short& port, const int& send_timeout_sec, const int& send_buf_size)
{
	strcpy(m_ip_string, ip);
	m_ip = inet_addr(ip);
	m_port = port;
	m_send_timeout_sec = send_timeout_sec;
	m_send_buf_size = send_buf_size;

	return true;
}

bool SendSocket::
	connectTo()
{
	// socket	
	m_send_socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	
	if(m_send_socket_fd == -1)
	{
		return false;
	}
	// socket addr
	struct sockaddr_in addr;	
	addr.sin_family = AF_INET;	
	addr.sin_addr.s_addr = m_ip;
	addr.sin_port = htons(m_port);
	if(connect(m_send_socket_fd, (const struct sockaddr*)&addr, (socklen_t)sizeof(addr)) == -1)
	{	
		close(m_send_socket_fd);
		m_send_socket_fd = -1;
		return false;
	}
	
	if (!setSendTimeout(m_send_timeout_sec))
	{
		close(m_send_socket_fd);
		m_send_socket_fd = -1;
		return false;
	}
/*
	if (!setLinger(1, 0))
	{
		close(m_send_socket_fd);
		m_send_socket_fd = -1;
		return false;
	}
*/
	if (!setSendBuf(m_send_buf_size)) {
		close(m_send_socket_fd);
		m_send_socket_fd = -1;
		return false;
	}	
	
	return true;
}

bool SendSocket::
	reconnectTo()
{
	if (m_send_socket_fd > 0)
	{
		close(m_send_socket_fd);
		m_send_socket_fd = -1;
	}
	
	return connectTo();
}

void SendSocket::
	closeSocket()
{
	if (m_send_socket_fd > 0)
	{
		close(m_send_socket_fd);
		m_send_socket_fd = -1;
	}
}

bool SendSocket::
	setRecvTimeout(int timeout_sec)
{
	if(m_send_socket_fd > 0)
	{
		struct timeval time;
		time.tv_sec = timeout_sec;
		time.tv_usec = 0;

		bool ret_val = (!setsockopt(m_send_socket_fd, SOL_SOCKET, SO_RCVTIMEO, &time, sizeof(time)));
		return ret_val;
	}
	else
	{
		return false;
	}
}

bool SendSocket::
	setSendTimeout(int timeout_sec)
{
	if(m_send_socket_fd > 0)
	{
		struct timeval time;
		time.tv_sec = timeout_sec;
		time.tv_usec = 0;

		bool ret_val = (!setsockopt(m_send_socket_fd, SOL_SOCKET, SO_SNDTIMEO, &time, sizeof(time)));
		return ret_val;
	}
	else
	{
		return false;
	}
}

bool SendSocket::setRecvBuf(uint32_t buf_size)
{
	if (m_send_socket_fd > 0) {
 		return (!setsockopt(m_send_socket_fd, SOL_SOCKET, SO_RCVBUF, &buf_size, sizeof(buf_size)));		
	} else {
		return false;
	}
}

bool SendSocket::setSendBuf(uint32_t buf_size)
{
	if (m_send_socket_fd > 0) {
 		return (!setsockopt(m_send_socket_fd, SOL_SOCKET, SO_SNDBUF, &buf_size, sizeof(buf_size)));		
	} else {
		return false;
	}
}

bool SendSocket::
	setLinger(int onoff, int linger)
{
	if(m_send_socket_fd > 0) {
		struct linger ling;
		ling.l_onoff = onoff;
		ling.l_linger = linger;

		bool ret_val = (!setsockopt(m_send_socket_fd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling)));
		return ret_val;
	} else {
		return false;
	}
}
	
bool SendSocket::
	sendData(const char* data, int data_len)
{
	int send_bytes = 0;
	while(data_len > 0)
	{
		send_bytes = send(m_send_socket_fd, data, data_len, MSG_NOSIGNAL);
		if(send_bytes <= 0)
		{
            LOG(INFO) << "send error:" << errno << "when send data from socket " << m_send_socket_fd;
			return false;
		}

		data += send_bytes;
		data_len -= send_bytes;
	}
	
	return true;
}

bool SendSocket::ssendData(int socket_fd,const char * send_buf, const int& send_bytes, const int& timeout_ms)
{
        struct pollfd poll_fd;
        memset(&poll_fd, 0, sizeof(poll_fd));
        poll_fd.fd = socket_fd;

        int total_send_bytes = 0;
        int left_bytes = send_bytes;
        int left_time_ms = (timeout_ms<=0) ? 0 : timeout_ms;
        const uint32_t calc_time_start = Utility::getCurMilliSeconds();
        do {
                int send_ret = send(socket_fd, send_buf+total_send_bytes, left_bytes, MSG_NOSIGNAL);
                if (send_ret >= 0)
				{
                        total_send_bytes += send_ret;
                        left_bytes -= send_ret;
                }
				else
				{
                    if (send_ret == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
					{
                            
                    } else 
                    {
                        LOG(INFO) << "send error: " << errno << " : " << strerror(errno) << "when send data from socket" << socket_fd;
                        return false;
                    }
                }

                if (left_bytes <= 0 || left_time_ms == 0)
				{
                        break;
                }

                poll_fd.events = (POLLOUT | POLLWRNORM);
                poll_fd.revents = 0;
                LOG(INFO) << "left time ms: " << left_time_ms;
                int poll_ret = poll(&poll_fd, 1, left_time_ms);
                if (poll_ret <= 0)
				{
                    return false;
                } 
				
                uint32_t calc_time_stop = Utility::getCurMilliSeconds();
                if ((calc_time_stop - calc_time_start)>=(uint32_t)timeout_ms)
				{
                        left_bytes = 0;
                } else
                {
                        left_time_ms = timeout_ms - (calc_time_stop - calc_time_start);
                }
        }            while (left_bytes > 0);

        return (total_send_bytes == send_bytes) ? true : false;
}


bool SendSocket::
	sendDataRetry(const char * data, unsigned int data_len)
{
	m_reconnect = 0;
	
	if (data == NULL || data_len < 12)
		return false;
	
	unsigned int time_start = 0;
	unsigned int time_end = 0;
	int ret = 0;
	bool send_successed = false;
	for (int i=0; i<2; i++) // try three times at most
	{
		time_start = Utility::getCurMilliSeconds();
		ret = sendData(data, data_len);
		time_end = Utility::getCurMilliSeconds();
		if (time_end - time_start > 0)
		{
		}
		
		if (ret <= 0)
		{
			m_reconnect = 1;
			
			reconnectTo();
		}
		else
		{
			send_successed = true;			
			break;
		}
	}
	return send_successed;
}

bool SendSocket::
	setBlock()
{
	int flag = 0;
	if ((flag=fcntl(m_send_socket_fd, F_GETFL)) < 0
		|| fcntl(m_send_socket_fd, F_SETFL, flag&(~O_NONBLOCK)) < 0)
	{
		return false;
	}
	return true;
}

bool SendSocket::
	setNonblock()
{
	int flag = 0;
	if ((flag=fcntl(m_send_socket_fd, F_GETFL)) < 0
		|| fcntl(m_send_socket_fd, F_SETFL, flag|(O_NONBLOCK)) < 0)
	{
		return false;
	}
	return true;
}


int SendSocket::
	recvData(int fd_sock, char* buffer, int recv_len,int& state)
{
	int recv_bytes = 0;
	int real_recv = 0;
    LOG(INFO) << "need recv data: " << recv_len;
	while(recv_len > 0)
	{
		recv_bytes = recv(fd_sock, buffer, recv_len, 0);
		if(recv_bytes == 0)
		{
            LOG(INFO) << "client close connection when recv data from socket " << fd_sock;
			return 0;
		}
		else if(recv_bytes < 0)
		{
			int err = errno;
			if(err != EAGAIN && err != EWOULDBLOCK && err != ETIMEDOUT)
			{
                LOG(INFO) << strerror(errno) << "when recv data from socket " << fd_sock;
				state = -1;
			}
			else
			{
                LOG(INFO) << strerror(errno) << "when recv data from socket" << fd_sock;
				state = 0;
			}
			return real_recv;
		}

		buffer += recv_bytes;
		recv_len -= recv_bytes;
		real_recv += recv_bytes;
        LOG(INFO) << "recv data : " << recv_bytes << "leave date: " << recv_len;
	}
	return real_recv;
}


