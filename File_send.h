#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include "File.h"
#include "ThreadPool.h"
#include <memory>
#include <mutex>

class File_send
{
private:
    FileInformation fileInformation_;    // 存放文件信息的结构体
    ThreadPool threadpool_;              // 工作线程池
    std::mutex meeutex_;                 // 保护文件发送的锁
    int sockfd_;                         // 客户端套接字
    std::string path_;                   // 文件路径

public:
    File_send(std::string filename, std::string path, std::uint64_t filesize, int sockfd, int subthreadnum = 3);
    ~File_send();
    bool SendStruct();                      //发送结构体
    bool SendFile();                        // 发送文件
    bool tcp_recv(std::string &buffer, size_t maxlen); // 接收报文的成员函数

    char * data();
};

