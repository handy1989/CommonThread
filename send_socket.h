#ifndef _SEND_SOCKET_H_
#define _SEND_SOCKET_H_

#include <stdint.h>
#include <glog/logging.h>

class SendSocket
{
    public:
    SendSocket();
    ~SendSocket();

    bool init(const char* ip, const short& port, const int& send_timeout_sec, const int& send_buf_size);
    bool connectTo();
    bool reconnectTo();
    void closeSocket();
    bool setRecvTimeout(int timeout_sec);
    bool setSendTimeout(int timeout_sec);
    bool setLinger(int onoff, int linger);
    bool setRecvBuf(uint32_t buf_size);
    bool setSendBuf(uint32_t buf_size);
    bool setBlock();
    bool setNonblock();
    bool isConnected();
    bool isReconnect();

    const char* get_ip();
    int get_sock_fd();
    short get_port();

    bool sendData(const char* data, int data_len);
    static bool ssendData(int socket_fd,const char * send_buf, const int& send_bytes, const int& timeout_ms = 100);
    int get_fd(){ return m_send_socket_fd; };
    bool sendDataRetry(const char* data, unsigned int data_len);

    static int recvData(int fd_sock, char* buffer, int recv_len, int& state);

    public:
    int m_send_timeout_sec;
    int m_send_buf_size;
    char m_ip_string[16];
    uint32_t m_ip;
    short m_port;
    short m_reconnect;
    int m_send_socket_fd;
    
};


#endif
