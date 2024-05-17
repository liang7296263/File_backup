#include "inotify.h"

/*class inotify
{
private:
    int inotifyfd_;           // inotify 实例的文件描述符
    std::string path_;        // 需要监控的文件路径
    inotify_event inotifyev_; // 事件信息结构体

public:
    inotify(const char *path);
    ~inotify();

    int inotifyfd();           // 返回inotify 实例的文件描述符
    std::string path();        // 返回需要监控的文件路径
    inotify_event inotifyev(); // 返回事件信息结构体

    void Star(); // 开启监控
};*/

inotify::inotify(const std::string &path) : path_(path)
{
    inotifyfd_  = inotify_init();
}

inotify::~inotify()
{
    ::close(inotifyfd_);
}

int inotify::inotifyfd()  // 返回inotify 实例的文件描述符
{
    return inotifyfd_;
}
/*inotify_event inotify::inotifyev() // 返回事件信息结构体
{
    return inotifyev_;
}*/

bool inotify::add_watch()
{
    int wd = inotify_add_watch(inotifyfd_, path_.c_str(), IN_CLOSE_WRITE | IN_DELETE);
    if (wd < 0)
    {
        std::cerr << "Error adding watch to directory" << std::endl;
        close(inotifyfd_);
        return false;
    }
    return true;
}

bool inotify::Star() // 开启监控
{
    char buffer[BUF_LEN];
    int num_bytes = read(inotifyfd_, buffer, BUF_LEN);
    if(num_bytes == -1) 
        return false;
    else
    {
        for (int i = 0; i < num_bytes;)
        {
            struct inotify_event *event = (struct inotify_event *)&buffer[i];
            if (event->len)
            {
                if (event->mask & IN_CLOSE_WRITE)
                {
                    std::cout << "File modify: " << event->name << std::endl;
                    std::string filename = event->name;
                    FileModifycallback_(filename);
                }
                else if (event->mask & IN_DELETE)
                {
                    std::cout << "File deleted: " << event->name << std::endl;
                }
            }
            i += EVENT_SIZE + event->len;
        }
    return true;
    }
   
}

void inotify::setFileCreatedcallback(std::function<void(std::string &)> fn) // 设置文件创建事件回调。
{
    FileCreatedcallback_ = fn;
}
void inotify::setFileModifycallback(std::function<void(std::string &)> fn)  // 设置文件修改事件回调。
{
    FileModifycallback_ = fn;
}
void inotify::setFileDeletecallback(std::function<void(std::string &)> fn)  // 设置文件修改事件回调。
{
    FileDeletecallback_ = fn;
}