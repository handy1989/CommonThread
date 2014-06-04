#include <iostream>

#include "../message_factory.h"
#include "../send_socket.h"
#include "../calculate_sum_message.h"

#include <pthread.h>
#include <glog/logging.h>

using namespace std;

uint32_t send_num = 0;

uint32_t connect_succ = 0;
uint32_t connect_fail = 0;

uint32_t recv_succ = 0;

uint32_t begin_time = time(NULL);

void* recvthreadProc(void* para)
{
	while(true)
	{
		char* buffer = new char[1024];
		int len = 1024;
		int state = 0;

		int send_socket = *((int*)para);
        LOG(INFO) << "send_socket2: " << send_socket;
		int real_len = SendSocket::recvData(send_socket,buffer,len,state);

		if(real_len == 0)
		{
			break;
		}

	/*	QuerySystemTimeRspMessage query_system_time_rsp_message;
		query_system_time_rsp_message.m_buffer = buffer;
		query_system_time_rsp_message.m_buffer_size = real_len;

		if(query_system_time_rsp_message.parseMessage())
		{
			cout << "system time:" << query_system_time_rsp_message.getSystemTime() << endl;
		}*/
	}
}

bool recv_threads(int fd)
{
	int* send_fd = new int(fd);
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	//set stack size
	pthread_attr_setstacksize(&attr, 1024*1024);

	pthread_t thread_id;
	if(pthread_create(&thread_id, &attr, recvthreadProc, (void*)send_fd))
	{
		return false;
	}

	pthread_attr_destroy(&attr);
	return true;
}


void* sendthreadProc(void* para)
{
	CalculateSumReqMessage* calculate_sum_message = (CalculateSumReqMessage*)para;
	SendSocket send_socket;
	send_socket.init("127.0.0.1",2222,1,1024*1024);

	while(true)
	{
		send_socket.reconnectTo();

		send_socket.sendData(calculate_sum_message->m_buffer.c_str(),calculate_sum_message->m_buffer.size());
		send_num++;
        LOG(INFO) << "send succ: " << send_num << " in " << time(NULL) - begin_time << "s" << endl;

		char* buffer = new char[1024];
		int len = 1024;
		int state = 0;
		int real_len = SendSocket::recvData(send_socket.m_send_socket_fd,buffer,len,state);

		if(real_len == 0)
		{
            LOG(INFO) << "recv failed!";
		}

		CalculateSumRspMessage calculate_sum_rsp_message;
		calculate_sum_rsp_message.m_buffer.assign(buffer,real_len);

		if(calculate_sum_rsp_message.parseMessage())
		{
            LOG(INFO) << "result: " << calculate_sum_rsp_message.getResult() << endl;
			recv_succ++;
            LOG(INFO) << "recv succ: " << recv_succ << " in " << time(NULL) - begin_time << "s" << endl; 
		}
	//	usleep(1000);
	}
}

bool send_threads(uint32_t pool_size,CalculateSumReqMessage* calculate_sum_message)
{
	if(pool_size > 0)
	{
		pthread_attr_t attr;
		pthread_attr_init(&attr);

		//set stack size
		pthread_attr_setstacksize(&attr, 1024*1024);

		pthread_t* thread_ids = new pthread_t[pool_size];
		for(uint32_t i = 0; i < pool_size; i++)
		{
			if(pthread_create((thread_ids + i), &attr, sendthreadProc, (void*)calculate_sum_message))
			{
				return false;
			}
		}

		pthread_attr_destroy(&attr);
	}
	return true;
}


int main()
{
	CalculateSumReqMessage calculate_sum_message;

	if(calculate_sum_message.buildMessage())
	{
		send_threads(5,&calculate_sum_message);
	}

	while(true)
		;

	
}
