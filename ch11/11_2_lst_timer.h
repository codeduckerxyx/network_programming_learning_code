#ifndef LST_TIMER
#define LST_TIMER

#include <time.h>
#define BUFFER_SIZE 64
class util_timer;   /* 前向声明 */

/* 用户数据结构：客户端socket地址，socket文件描述符，读缓存和定时器 */
struct client_data{
    sockaddr_in address;
    int sockfd;
    char buf[ BUFFER_SIZE ];
    util_timer* timer;
};

/* 定时器类 */
class util_timer{
public:
    util_timer() : prev(NULL),next(NULL){}
public:
    time_t expire;  /* 任务的超过时间，这里使用绝对时间 */
    void (*cb_func)( client_data* );    /* 任务回调函数 */
    /* 回调函数处理的客户数据，由定时器的执行者传递给回调函数 */
    client_data* prev;  /* 指向前一个定时器 */
    client_data* next;  /* 指向下一个定时器 */
};

/* 定时器链表。它是一个升序，双向链表，且带有头节点和尾节点 */
class sort_timer_lst{
public:
    sort_timer_lst():head(NULL),tail(NULL){}
    /* 链表被销毁时，删除其中所有的定时器 */
    ~sort_timer_lst(){
        util_timer* tmp = head;
        while(tmp){
            head = tmp->next;
            delete tmp;
            tmp = head;
        }
    }

    /* 将目标定时器timer添加到链表中 */
    void add_timer( util_timer* timer ){
        if( !timer ){
            return;
        }
        if( !head ){
            head = tail = timer;
            return;
        }
        /* 如果目标定时器的超时时间小于当前链表中所有定时器的超时时间，则把该定时器插入链表头部，作为链表新的头节点。
        否则就需要调用重载函数add_timer(util_timer* timer,util_timer* lst_head),把他作为链表中合适的位置，以保证
        链表的升序特性 */
        if( timer->expire < head->expire ){
            timer->next = head;
            head->prev = timer;
            head = timer;
            return;
        }
        add_timer( timer,head );
    }

    /* 当某个定时任务发送变化时，调整对应的定时器在链表中的位置。这个函数只考虑被调整的定时器的超过时间延长的情况，
    即该定时器在链表中的位置。这个函数只考虑被调整的定时器的超时时间延长的情况，即该定时器需要往链表尾部移动 */
    void adjust_timer( util_timer* timer ){
        if( !timer ){
            return;
        }
        util_timer* tmp = timer->next;
        /* 如果被调整的目标定时器处在链表尾部，或者该定时器新的超时值仍然小于其下一个定时器的超时值，则不用调整 */
        if( !tmp || ( timer->expire < tmp->expire ) ){
            return;
        }
        /* 如果目标定时器是链表的头节点，则将定时器从链表中取出并重新插入链表 */
        if( timer==head ){
            head = head->next;
            head->prev = NULL;
            timer->next = NULL;
            add_timer( timer,head );
        }
        /* 如果目标定时器不是链表的头节点。则将该定时器从链表中取出，然后插入其原来所在位置之后的部分链表中 */
        else{
            timer->prev->next = timer->next;
            timer->next->prev = timer->prev;
            add_timer( timer,timer->next );
        }
    }
}