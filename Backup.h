#pragma once
#include "ClientSocket.h"
#include "File_send.h"
#include <string>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "inotify.h"
#include <sys/stat.h>
#include <fstream>
#include <cstdint>
#include <filesystem>
#include <chrono>
#include <thread>


//主业务类
class Backup
{
private:
    inotify *inot_;             // inotify类
    ClientSocket *socket_;       // ClientSocket类
    File_send *filesend_;        // File_send类
    std::string ip_;            // ip地址
    unsigned short port_;       // 服务端端口
    std::string path_;          // 监控文件路径
    ThreadPool threadpool_;     // 工作线程池
    std::mutex meutex_;         // 保护锁

    

public:
    Backup(const std::string &ip, const std::string &port, const std::string &path, int worktheadnum = 5);
    ~Backup();

    void Start();                                     // 开始任务
    void Stop();                                      // 结束任务

    void HandlFileCreated(std::string &filename);     // 处理文件创建事件，在inotify类中回调此函数。
    void HandlFileModify(std::string &filename);      // 处理文件修改事件，在inotify类中回调此函数。
    void HandlFileDelete(std::string &filename);      // 处理文件删除事件，在inotify类中回调此函数。

    void FileCreated(std::string &filename); // 处理文件创建事件
    void FileModify(std::string &filename);  // 处理文件修改事件
    void FileDelete(std::string &filename);  // 处理文件删除事件
};


