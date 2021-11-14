#ifndef LST_TIMER
#define LST_TIMER

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/uio.h>

#include <time.h>
#include "../log/log.h"
#include "time_wheel.h"

class util_timer;

//连接资源数据
struct client_data {
    sockaddr_in address;        //地址
    int sockfd;                    //sock文件描述符
    util_timer *timer;            //它指向的定时器,指针
};

//一个定时器类
class util_timer {
public:
    util_timer() : prev(NULL), next(NULL) {}

public:
    time_t expire;                        //超时时间

    void (*cb_func)(client_data *);    //函数指针，回调函数
    client_data *user_data;                //连接的资源
    util_timer *prev;                    //前向指针
    util_timer *next;                    //后向指针
};

//升序计时器链表，以超时时间作为排序标准
class sort_timer_lst {
public:
    sort_timer_lst();

    ~sort_timer_lst();

    void add_timer(util_timer *timer);                                //添加计时器
    void adjust_timer(util_timer *timer);                            //
    void del_timer(util_timer *timer);

    void tick();

private:
    void add_timer(util_timer *timer, util_timer *lst_head);

    util_timer *head;
    util_timer *tail;
};

//工具类
class Utils {
public:
    Utils() {}

    ~Utils() {}

    void init(int timeslot);

    //对文件描述符设置非阻塞
    int setnonblocking(int fd);

    //将内核事件表注册读事件，ET模式，选择开启EPOLLONESHOT
    void addfd(int epollfd, int fd, bool one_shot, int TRIGMode);

    //信号处理函数
    static void sig_handler(int sig);

    //设置信号函数
    void addsig(int sig, void(handler)(int), bool restart = true);

    //定时处理任务，重新定时以不断触发SIGALRM信号
    void timer_handler();

    void show_error(int connfd, const char *info);

public:
    static int *u_pipefd;            //信号管道
    sort_timer_lst m_timer_lst;
    time_wheel m_time_wheel;

    static int u_epollfd;
    int m_TIMESLOT;
};

//回调函数
void cb_func(_client_data *user_data);

#endif
