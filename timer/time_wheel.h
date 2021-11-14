#ifndef WEBSERVER_TIME_WHEEL_H
#define WEBSERVER_TIME_WHEEL_H

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

class tw_timer;

//连接资源数据
struct _client_data {
    sockaddr_in address;        //地址
    int sockfd;                 //sock文件描述符
    tw_timer *timer;            //它指向的定时器,指针
};

//一个定时器类
class tw_timer {
public:
    explicit tw_timer(int time_out);
    ~tw_timer() = default;

private:
    int rotation{};   //记录定时器在时间轮转多少圈后生效
    int time_slot{};  //记录定时器属于时间轮哪个槽
    friend class time_wheel;

public:
    int timeout{};						//超时时间

    void (*cb_func)(_client_data *){};    //函数指针，回调函数
    _client_data *user_data{};                //连接的资源
    tw_timer *prev;                    //前向指针
    tw_timer *next;                    //后向指针
};

class time_wheel final{
public:
    time_wheel();
    ~time_wheel();

    /**
     * 往时间轮上添加一个定时器 ,插入合适的槽
     * @param timeout 定时器的超时时间
     * @return 成功插入的定时器
     */
    bool add_timer(tw_timer* timer);

    /**
     * 删除目标定时器
     * @param timer
     * @return 是否成功
     */
    bool del_timer(tw_timer* timer);

    /**
     * 设置新的超时时间，调用此函数重新调整一个计时器的位置
     * @param timer
     * @return 是否成功
     */
    bool adjust_timer(tw_timer* timer);

    /**
     * SI时间到后，调用该函数，时间轮往前滚动一个槽
     */
    void tick();


private:
    static const int N = 60;    //时间轮上的槽的数目
    static const int SI = 1;    //每ST时间，时间轮就转动一次
    tw_timer *slots[N]{};     //时间轮上的槽
    int cur_slot;   //时间轮当前指向的槽

};


#endif //WEBSERVER_TIME_WHEEL_H
