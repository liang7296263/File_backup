#include "File_send.h"

/*class File_send
{
private:
    FileInformation fileInformation_; // 存放文件信息的结构体
    ThreadPool threadpool_;           // 工作线程池
    std::mutex meeutex_;              // 保护文件发送的锁

public:
    File_send(const char *filename, const char *filesize, int subthreadnum = 3);
    ~File_send();
    void SendStruct(); // 发送结构体
    void SendFile();   // 发送文件
};*/

File_send::File_send(std::string filename, std::string path, std::uint64_t filesize, int sockfd, int subthreadnum)
    : threadpool_(subthreadnum, "WORE"), sockfd_(sockfd), path_(path)
{
    strcpy(fileInformation_.filename, filename.c_str());
    fileInformation_.filesize = filesize; // 文件大小
    
}

File_send::~File_send()
{

}

bool File_send::SendStruct() // 发送结构体
{
    char tmpbuf[4096]; // 临时的buffer，报文头部+报文内容。
    memset(tmpbuf, 0, sizeof(tmpbuf));
    memcpy(tmpbuf, &fileInformation_, sizeof(fileInformation_)); // 拼接报文内容。

    if((send(sockfd_, tmpbuf, sizeof(fileInformation_), 0)) <= 0)
        return false; // 发送结构体

    return true;

}
bool File_send::SendFile() // 发送文件
{
    std::ifstream fin(path_ + "/" + fileInformation_.filename, std::ios::binary); // 创建文件流
    
    if (fin.is_open() == false)
    {
        std::cout << "打开文件失败。 \n";
        return false;
    } // 打开文件

    int onread = 0;     // 打算读取文件的大小（字节数）
    int totalbytes = 0; // 已经读取的字节数
    char buffer[4096];  // 接收文件内容的缓冲区
    // char tmpbuffer[4096]; // 拼接缓冲区
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        // memset(tmpbuffer, 0, sizeof(tmpbuffer));
        //  计算本次应该读取的字节数，如果剩余的数据超过7字节，就读7字节。
        if (fileInformation_.filesize - totalbytes > 4096)
            onread = 4096;
        else
            onread = fileInformation_.filesize - totalbytes;

        // 从文件中读取数据
        fin.read(buffer, onread);
        int len = fin.gcount();
        // std::cout << "len的大小是" << len << std::endl;
        // isFile = false;                     // 设置标志位。
        // memcpy(tmpbuffer, &isFile, 1);      // 拼接报文头部。
        // memcpy(tmpbuffer + 1, buffer, len); // 拼接报文内容。

        if((send(sockfd_, buffer, len, 0)) <= 0)
            return false;

        // 计算文件已读取字节总数， 如果文件读完，跳出循环。
        totalbytes = totalbytes + onread;

        if (totalbytes == fileInformation_.filesize)
            break;
        
    }
    std::cout << "发送完毕" << std::endl;
    return true;
}

bool File_send::tcp_recv(std::string &buffer, size_t maxlen)
{
    buffer.clear();                                         // 清空容器
    buffer.resize(maxlen);                                  // 设置容器大小为maxlen
    int readn = recv(sockfd_, &buffer[0], buffer.size(), 0); // 直接操作buffer的内存
    if (readn <= 0)
    {
        buffer.clear();
        return false;
    }
    buffer.resize(readn); // 重置buffer的实际大小

    return true;
}

char *File_send::data()
{
    return fileInformation_.filename;
}