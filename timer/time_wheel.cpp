//
// Created by doper on 11/14/21.
//

#include "time_wheel.h"

tw_timer::tw_timer(int rot, int ts) : prev(nullptr), next(nullptr), rotation(rot), time_slot(ts) {}

time_wheel::time_wheel() : cur_slot(0) {
    for (auto &slot: slots) {
        slot = nullptr;
    }
}

time_wheel::~time_wheel() {
    for (auto &slot: slots) {
        tw_timer *tmp = slot;
        while (tmp) {
            slot = tmp->next;
            delete tmp;
            tmp = slot;
        }
    }
}

bool time_wheel::add_timer(tw_timer* timer) {
    if (!timer) {
        return false;
    }
    int ticks = 0;
    //根据定时器超时时间计算它在时间轮转动多少个ticks后出发，并按ticks计算他要时间轮转动多少圈后会触发
    if (timer->timeout < SI) {
        ticks = 1;
    } else {
        ticks = timer->timeout / SI;
    }
    //计算转动多少圈触发
    int rotation = ticks / N;
    //计算哪个槽
    int ts = (cur_slot + (ticks % N)) % N;
    //创建定时器
    timer->rotation = rotation;
    timer->time_slot = ts;

    //如果该槽为空，设置为头节点
    if (!slots[ts]) {
        printf("add timer, rotation is %d, ts is %d, cur_slot is %d\n", rotation, ts, cur_slot);
        slots[ts] = timer;
    } else {    //头插法
        timer->next = slots[ts];
        slots[ts]->prev = timer;
        slots[ts] = timer;
    }
    return timer;
}

bool time_wheel::del_timer(tw_timer *timer) {
    if (!timer) {
        return false;
    }
    int ts = timer->time_slot;
    //若要删除的是头节点
    if (timer == slots[ts]) {
        slots[ts] = slots[ts]->next;
        if (slots[ts]) {
            slots[ts]->prev = nullptr;
        }
    } else {
        timer->prev->next = timer->next;
        if (timer->next) {
            timer->next->prev = timer->prev;
        }
    }
    delete timer;
    return true;
}

void time_wheel::tick() {
    tw_timer *tmp = slots[cur_slot];
    printf("current slot is %d\n", cur_slot);
    while (tmp) {
        printf("tick the timer once\n");
        //如果此时定时器的rotation大于0,则说明还没到超时
        if (tmp->rotation > 0) {
            tmp->rotation--;
            tmp = tmp->next;
        } else {    //超时
            tmp->cb_func(tmp->user_data);
            tw_timer *tmp2 = tmp->next;
            del_timer(tmp);
            tmp = tmp2;
        }
    }
    cur_slot = ++cur_slot % N;
}

bool time_wheel::adjust_timer(tw_timer *timer) {
    if(!timer) {
        return false;
    }
    int ts = timer->time_slot;
    //若要调整的是头节点
    if (timer == slots[ts]) {
        slots[ts] = slots[ts]->next;
        if (slots[ts]) {
            slots[ts]->prev = nullptr;
        }
    } else {
        timer->prev->next = timer->next;
        if (timer->next) {
            timer->next->prev = timer->prev;
        }
    }
    add_timer(timer);
    return true;
}








