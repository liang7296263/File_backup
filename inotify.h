#pragma once
#include <iostream>
#include <sys/inotify.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <functional>

#define MAX_EVENTS 10
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (MAX_EVENTS * (EVENT_SIZE + 16))

class inotify
{
private:
    int inotifyfd_;              // inotify 实例的文件描述符
    const std::string path_; // 需要监控的文件路径

    std::function<void(std::string &)> FileCreatedcallback_; // 处理文件创建事件， 在Backup中回调。
    std::function<void(std::string &)> FileModifycallback_; // 处理文件修改事件， 在Backup中回调。
    std::function<void(std::string &)> FileDeletecallback_; // 处理文件删除事件， 在Backup中回调。
public:
    inotify(const std::string &path);
    ~inotify();

    int inotifyfd();             // 返回inotify 实例的文件描述符
    std::string path();          // 返回需要监控的文件路径
    //inotify_event inotifyev();   // 返回事件信息结构体

    bool add_watch();            // 用于向inotify实例中添加要监视的文件或目录，并指定要监视的事件类型。
    bool Star();          // 开启监控

    void setFileCreatedcallback(std::function<void(std::string &)> fn); // 设置文件创建事件回调。
    void setFileModifycallback(std::function<void(std::string &)> fn); // 设置文件修改事件回调。
    void setFileDeletecallback(std::function<void(std::string &)> fn);     // 设置文件修改事件回调。
};
