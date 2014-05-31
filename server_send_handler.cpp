#include "server_send_handler.h"

//���ݷ����̣߳��������ӷ�����
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


//�������ݷ����̵߳�ִ�к���
//�����ݷ���ʧ��ʱ��Ӧ�����ٴ����ӷ�����
//�߼����Ż�
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


