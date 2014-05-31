#include "server_send_handler.h"

//数据发送线程，主动连接服务器
ServerSendHandler::ServerSendHandler(int pool_size) : CommonThreadPool(pool_size)
{
}

ServerSendHandler::~ServerSendHandler()
{
}

void ServerSendHandler::initSocket(const char * ip, short port, int send_timeout_sec, int send_buf_size)
{
	m_ip = ip;
	m_port = port;
	m_send_timeout_sec = send_timeout_sec;	
	m_send_buf_size = send_buf_size;
}

void ServerSendHandler::initQueues(MessageQueue* message_send_queue)
{
	m_message_send_queue = message_send_queue;
}


//服务数据发送线程的执行函数
//当数据发送失败时，应主动再次连接服务器
//逻辑待优化
void ServerSendHandler::work()
{

	Message* message;

	ServerSendSocket server_send_socket;
	server_send_socket.initSocket(m_ip.c_str(), m_port, m_send_timeout_sec, m_send_buf_size);

	while (true)
	{
		message = NULL;
		if  (!m_message_send_queue->pop(message, 0)) {
            LOG(INFO) << "MessageQueue::pop() FAILED!";
			continue;
		}
		if (message == NULL) {
            LOG(INFO) << "MessageQueue::pop() NULL!";
			continue;
		}
		

		for(int i = 0;i != 2 ; ++i) //if fail,try again;
		{
			if(!server_send_socket.sendData(message))
			{
                LOG(INFO) << "send response (len=" << message->m_buffer.size() << ") to rootserver(" <<m_ip << ":" <<m_port<< ") FAIL" ;
			}
			else
			{
                LOG(INFO) << "send response (len="<<message->m_buffer.size()<<") to rootserver("<<m_ip<<":"<<m_port<<") SUCCESS" ;
				break;
			}
		}
		delete message;
		message = NULL;
	}
}


