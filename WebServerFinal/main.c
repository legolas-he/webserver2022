#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <sys/epoll.h>
#include "my_timer.h"
#include "locker.h"
#include "./threadpool/threadpool.h"
#include "./http/http_conn.h"
#include "./log/log.h"
#include "./sql/sql.h"

#define CLICK 5             //时钟超时单位
#define MAX_FD 65536           //最大文件描述符
#define MAX_EPOLL_E_NUM 10000 //最大事件数

extern int addfd(int epollfd, int fd, bool one_shot);
extern int remove(int epollfd, int fd);
extern int setnonblocking(int fd);

static int pipefd[2];
static sort_timer_lst timer_lst;
static int epollfd = 0;

//信号处理
void sig_handler(int sig)
{
    
    int save_errno = errno;
    int msg = sig;
    send(pipefd[1], (char *)&msg, 1, 0);
    errno = save_errno;
}
void addsig(int sig, void(handler)(int), bool restart = true)
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = handler;
    if (restart)
        sa.sa_flags |= SA_RESTART;
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig, &sa, NULL) != -1);
}

void cb_func(client_data *user_data)
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, user_data->sockfd, 0);
    assert(user_data);
    close(user_data->sockfd);
    http_conn::m_user_count--;
    LOG_INFO("close fd %d", user_data->sockfd);
    Log::get_instance()->flush();
}

int main(int argc, char *argv[])
{
    //创建mysql连接池
    connection_pool *connPool = connection_pool::GetInstance();
    //此处记得修改成自己的用户名密码
    connPool->init("localhost", "debian-sys-maint", "Lb06NGIJtyUMsGXx", "yourdb", 3306, 8);

    //创建线程池
    threadpool<http_conn> *pool = NULL;
    try
    {
        pool = new threadpool<http_conn>(connPool);
    }
    catch (...)
    {
        return 1;
    }

    //创建日志
    Log::get_instance()->init("ServerLog", 2000, 800000); 
    addsig(SIGPIPE, SIG_IGN);
    

    http_conn *users = new http_conn[MAX_FD];
    assert(users);

    
    users->initmysql_result(connPool);

    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);

    int port = atoi(argv[1]);
    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    address.sin_port = htons(port);

    int flag = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    ret = bind(listenfd, (struct sockaddr *)&address, sizeof(address));
    assert(ret >= 0);
    ret = listen(listenfd, 5);
    assert(ret >= 0);

    //epoll
    epoll_event events[MAX_EPOLL_E_NUM];
    epollfd = epoll_create(5);
    assert(epollfd != -1);

    addfd(epollfd, listenfd, false);
    http_conn::m_epollfd = epollfd;

    ret = socketpair(PF_UNIX, SOCK_STREAM, 0, pipefd);
    assert(ret != -1);
    setnonblocking(pipefd[1]);
    addfd(epollfd, pipefd[0], false);

    addsig(SIGALRM, sig_handler, false);
    addsig(SIGTERM, sig_handler, false);
    bool stop_server = false;

    client_data *users_timer = new client_data[MAX_FD];

    bool timeout = false;
    alarm(CLICK);

    printf("The server running successfully!\n");
    while (!stop_server)
    {

        int number = epoll_wait(epollfd, events, MAX_EPOLL_E_NUM, -1);
        if (number < 0 && errno != EINTR)
        {
            LOG_ERROR("%s", "epoll failed! ");
            break;
        }

        for (int i = 0; i < number; i++)
        {
            int sockfd = events[i].data.fd;

            if (sockfd == listenfd)
            {
                struct sockaddr_in client_address;
                socklen_t client_addrlength = sizeof(client_address);

                int connfd = accept(listenfd, (struct sockaddr *)&client_address, &client_addrlength);
                if (connfd < 0)
                {
                    LOG_ERROR("%s:errno is:%d", "accept error", errno);
                    continue;
                }
                if (http_conn::m_user_count >= MAX_FD)
                {
                   
                    LOG_ERROR("%s", "Webserver busy");
                    continue;
                }
                users[connfd].init(connfd, client_address);

                users_timer[connfd].address = client_address;
                users_timer[connfd].sockfd = connfd;
                util_timer *timer = new util_timer;
                timer->user_data = &users_timer[connfd];
                timer->cb_func = cb_func;
                time_t cur = time(NULL);
                timer->expire = cur + 3 * CLICK;
                users_timer[connfd].timer = timer;
                timer_lst.add_timer(timer);


            }

            else if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
            {
                util_timer *timer = users_timer[sockfd].timer;
                timer->cb_func(&users_timer[sockfd]);

                if (timer)
                {
                    timer_lst.del_timer(timer);
                }
            }

            //处理信号
            else if ((sockfd == pipefd[0]) && (events[i].events & EPOLLIN))
            {
                int sig;
                char signals[1024];
                ret = recv(pipefd[0], signals, sizeof(signals), 0);
                if (ret == -1 || ret==0)
                {
                    continue;
                }
                else
                {
                    for (int i = 0; i < ret; ++i)
                    {
                        switch (signals[i])
                        {
                        case SIGALRM:
                        {
                            timeout = true;
                            break;
                        }
                        case SIGTERM:
                        {
                            stop_server = true;
                        }
                        }
                    }
                }
            }

            else if (events[i].events & EPOLLIN)
            {
                util_timer *timer = users_timer[sockfd].timer;
                if (users[sockfd].read_once())
                {
                    LOG_INFO("deal with user(%s)", inet_ntoa(users[sockfd].get_address()->sin_addr));
                    Log::get_instance()->flush();
                    pool->append(users + sockfd);

                    if (timer)
                    {
                        time_t cur_timer = time(NULL);
                        timer->expire = cur_timer + 3 * CLICK;
                        timer_lst.adjust_timer(timer);
                    }
                }
                else
                {
                    timer->cb_func(&users_timer[sockfd]);
                    if (timer)
                    {
                        timer_lst.del_timer(timer);
                    }
                }
            }
            else if (events[i].events & EPOLLOUT)
            {
                util_timer *timer = users_timer[sockfd].timer;
                if (users[sockfd].write())
                {
                    LOG_INFO("send data to user(%s)", inet_ntoa(users[sockfd].get_address()->sin_addr));
                    Log::get_instance()->flush();

                    if (timer)
                    {
                        time_t cur_timer = time(NULL);
                        timer->expire = cur_timer + 3 * CLICK;
                        timer_lst.adjust_timer(timer);
                    }
                }
                else
                {
                    timer->cb_func(&users_timer[sockfd]);
                    if (timer)
                    {
                        timer_lst.del_timer(timer);
                    }
                }
            }
        }
        if (timeout)
        {
            timer_lst.tick();
            alarm(CLICK);
            timeout = false;
        }
    }
    close(epollfd);
    close(listenfd);
    close(pipefd[1]);
    close(pipefd[0]);
    delete[] users;
    delete[] users_timer;
    delete pool;
    return 0;
}
