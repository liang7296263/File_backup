#include"ClientSocket.h"

/*class ClientSocket
{
private:
    sockaddr_in addr_; // 存放服务端ip地址和端口的结构体
    int sockfd_;       // 客户端sockfd
public:
    ClientSocket(const std::string &in_ip, const unsigned short in_port);
    ~ClientSocket();

    bool tcp_connect(); // 建立连接
    int sockfd();       // 返回sockfd
};*/

ClientSocket::ClientSocket(const std::string &in_ip, unsigned short in_port) : s_ip_(in_ip), in_port_(in_port), sockfd_(-1)
{
    memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(in_port_);
    addr_.sin_addr.s_addr = inet_addr(in_ip.c_str());
}

ClientSocket::~ClientSocket()
{
    ::close(sockfd_);
}

bool ClientSocket::tcp_connect() // 建立连接
{
    if (sockfd_ != -1)
        return false; // 如果socket已连接，返回失败
                      // 第一步：创建客户端的socket
    if ((sockfd_ = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return false;
    //std::cout << "创建套接字成功，fd是： " << sockfd_ << std::endl;
    //std::cout << "ip 是： " << addr_.sin_addr.s_addr << std::endl;
    //std::cout << "端口是： " << addr_.sin_port << std::endl;
    // 第二步：向服务器发起连接请求


    struct hostent *h;                                // 用于存放服务端ip的数据结构
    if ((h = gethostbyname(s_ip_.c_str())) == nullptr) // 指定服务端ip地址
    {
        close(sockfd_);
        sockfd_ = -1;
        return false;
    }

    memcpy(&addr_.sin_addr, h->h_addr, h->h_length);

    if (connect(sockfd_, (struct sockaddr *)&addr_, sizeof(addr_)) != 0) // 向服务器发起连接请求
    {
        close(sockfd_);
        sockfd_ = -1;
        return false;
    }
    return true;
}

int ClientSocket::sockfd() // 返回sockfd
{
    return sockfd_;
}

std::string ClientSocket::s_ip()
{
    return s_ip_;
}