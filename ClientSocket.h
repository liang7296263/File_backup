#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <string>
#include <netdb.h>
#include <iostream>
#include <arpa/inet.h>

class ClientSocket
{
private:
    sockaddr_in addr_;  // 存放服务端ip地址和端口的结构体
    int sockfd_;        // 客户端sockfd
    std::string s_ip_;
    unsigned short in_port_;
public:
    ClientSocket(const std::string &in_ip, unsigned short in_port);
    ~ClientSocket();

    bool tcp_connect(); // 建立连接
    int sockfd();        // 返回sockfd
    std::string s_ip();
};

