#include "Backup.h"

//获取文件大小的函数
std::uint64_t getFileSize(const std::string &filename)
{
    std::uint64_t filesize;
    struct stat st;
    if (stat(filename.c_str(), &st) == -1)
    {
        std::cerr << "Error: " << strerror(errno) << " (" << errno << ")" << std::endl;
        return 0;
    }
    // 将 st.st_size 转换为 uint64_t 类型
    return static_cast<std::uint64_t>(st.st_size);
}
// 检查是否以~开头或以tmp结尾
bool shouldFilter(const std::string &str)
{
    // 检查是否以~开头或以tmp结尾
    return (str.front() == '.') || (str.substr(str.length() - 3) == "tmp");
}


Backup::Backup(const std::string &ip, const std::string &port, const std::string &path, int worktheadnum)
    : ip_(ip), port_(std::stoi(port)), path_(path), threadpool_(worktheadnum, "WORKS")
{
    inot_ = new inotify(path_);
    inot_->setFileCreatedcallback(std::bind(&Backup::FileCreated, this, std::placeholders::_1));
    inot_->setFileModifycallback(std::bind(&Backup::FileModify, this, std::placeholders::_1));
    inot_->setFileDeletecallback(std::bind(&Backup::HandlFileDelete, this, std::placeholders::_1));
}

Backup::~Backup()
{
    delete inot_;
}

void Backup::Start() // 开始任务
{
    inot_->add_watch();
    while (true)
    {
        inot_->Star();
    }
    
}
void Backup::Stop()  // 结束任务
{
    // 停止工作线程
    threadpool_.stop();
}

void Backup::HandlFileCreated(std::string &filename) // 处理文件创建事件。
{
    if (threadpool_.size() == 0) // 如果没有工作线程，直接调用处理函数
    {
        FileCreated(filename);
    }
    else
    {
        std::lock_guard<std::mutex> lock(meutex_); // 加锁
        threadpool_.addtask(std::bind(&Backup::FileCreated, this, filename));
    }
}
void Backup::HandlFileModify(std::string &filename) // 处理文件修改事件。
{
    if (threadpool_.size() == 0) // 如果没有工作线程，直接调用处理函数
    {
        FileModify(filename);
    }
    else
    {
        std::lock_guard<std::mutex> lock(meutex_); // 加锁
        threadpool_.addtask(std::bind(&Backup::FileModify, this, filename));
    }
}
void Backup::HandlFileDelete(std::string &filename) // 处理文件删除事件。
{
    if (threadpool_.size() == 0) // 如果没有工作线程，直接调用处理函数
    {
        FileDelete(filename);
    }
    else
    {
        std::lock_guard<std::mutex> lock(meutex_); // 加锁
        threadpool_.addtask(std::bind(&Backup::FileDelete, this, filename));
    }
}

void Backup::FileCreated(std::string &filename) // 处理文件创建事件
{
    auto filesize = getFileSize(path_ + filename);

    //std::cout << filesize << std::endl;

    // 创建socket类
    ClientSocket socket(ip_, port_);

    /*第一步：调用tcp_client类的成员函数tcp_connect发起连接*/
    if (socket.tcp_connect() == false)
    {
        perror("tcp_connect");
        return;
    }

    int sockfd = socket.sockfd();

    // 创建File_send类
    File_send filesend(filename, path_, filesize, sockfd, 0);

    // 把文件信息的结构体发送给服务端
    if (filesend.SendStruct() == false)
    {
        perror("send");
        return;
    }

    /*第三步：等待服务端的确认报文（文件名和文件的大小的确认）*/
    std::string buffer;
    if (filesend.tcp_recv(buffer, 2) == false)
    {
        perror("recv()");
        return;
    }
    if (buffer != "ok")
    {
        std::cout << "服务端没有回复ok。 \n";
        return;
    }

    /*第四步：发送文件内容*/
    if (filesend.SendFile() == false)
    {
        perror("sendfile()");
        return;
    }

    // 第五步：等待服务端的确认报文（服务端已接收完文件）
    if (filesend.tcp_recv(buffer, 2) == false)
    {
        perror("recv()");
        return;
    }
    if (buffer != "ok")
    {
        std::cout << "发送文件内容失败。 \n";
        return;
    }

    std::cout << "文件接收成功" << std::endl;
}

void Backup::FileModify(std::string &filename) // 处理文件修改事件
{

    std::cout << "进入文件处理函数" << std::endl;
    if (shouldFilter(filename) == true)
        return;
    else
    {
        std::cout << "非临时文件，开始发送文件" << std::endl;
        auto filesize = getFileSize(path_ + filename);

        // std::cout << filesize << std::endl;

        // 创建socket类
        ClientSocket socket(ip_, port_);

        /*第一步：调用tcp_client类的成员函数tcp_connect发起连接*/
        if (socket.tcp_connect() == false)
        {
            perror("tcp_connect");
            return;
        }

        int sockfd = socket.sockfd();

        // 创建File_send类
        File_send filesend(filename, path_, filesize, sockfd, 0);

        // 把文件信息的结构体发送给服务端
        if (filesend.SendStruct() == false)
        {
            perror("send");
            return;
        }

        /*第三步：等待服务端的确认报文（文件名和文件的大小的确认）*/
        std::string buffer;
        if (filesend.tcp_recv(buffer, 2) == false)
        {
            perror("recv()");
            return;
        }
        if (buffer != "ok")
        {
            std::cout << "服务端没有回复ok。 \n";
            return;
        }

        /*第四步：发送文件内容*/
        if (filesend.SendFile() == false)
        {
            perror("sendfile()");
            return;
        }

        // 第五步：等待服务端的确认报文（服务端已接收完文件）
        if (filesend.tcp_recv(buffer, 2) == false)
        {
            perror("recv()");
            return;
        }
        if (buffer != "ok")
        {
            std::cout << "发送文件内容失败。 \n";
            return;
        }

        std::cout << "文件接收成功" << std::endl;
    }
    
}
void Backup::FileDelete(std::string &filename) // 处理文件删除事件
{
    //同步文件夹时启用
}