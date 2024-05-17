#include"ClientSocket.h"
#include "File_send.h"
#include <string>
#include <iostream>
#include <unistd.h>
#include "inotify.h"
#include "Backup.h"
#include <signal.h>

Backup *bac;

void Stop(int sig)
{
    std::cout << "sig=" << sig << std::endl;
    // 调用EchoServer::Stop()停止服务
    bac->Stop();
    std::cout << "eco已停止" << std::endl;
    delete bac;
    std::cout << "delete eco" << std::endl;
    exit(0);
}
int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cout << "Using:./client 服务端的ip 服务端的端口 文件监控路径\n";
    }

    signal(SIGTERM, Stop); // 信号15，系统kill或killall命令默认发送的信号。
    signal(SIGINT, Stop);  // 信号2，按Ctrl+C发送的信号。

    bac = new Backup(argv[1], argv[2], argv[3], 0);

    bac->Start();

    return 0;
}
