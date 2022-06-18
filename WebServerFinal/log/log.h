#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <iostream>
#include <string>
#include <stdarg.h>
#include <pthread.h>
#include "../locker.h"
using namespace std;

template <class T>
class block_queue
{
public:
    block_queue(int max_size = 1000)
    {
        q_max_size = max_size;
        q_data = new T[max_size];
        q_size = 0;
        q_front = -1;
        q_back = -1;
    }

    void clear()
    {
        q_mutex.lock();
        q_size = 0;
        q_front = -1;
        q_back = -1;
        q_mutex.unlock();
    }

    ~block_queue()
    {
        q_mutex.lock();
        if (q_data != NULL)
            delete [] q_data;

        q_mutex.unlock();
    }
    
    bool full() 
    {
        q_mutex.lock();
        if (q_size >= q_max_size)
        {

            q_mutex.unlock();
            return true;
        }
        q_mutex.unlock();
        return false;
    }
   
    bool empty() 
    {
        q_mutex.lock();
        if (!q_size)
        {
            q_mutex.unlock();
            return true;
        }
        q_mutex.unlock();
        return false;
    }
    
    bool front(T &value) 
    {
        q_mutex.lock();
        if (!q_size)
        {
            q_mutex.unlock();
            return false;
        }
        value = q_data[q_front];
        q_mutex.unlock();
        return true;
    }
    
    bool back(T &value) 
    {
        q_mutex.lock();
        if (!q_size)
        {
            q_mutex.unlock();
            return false;
        }
        value = q_data[q_back];
        q_mutex.unlock();
        return true;
    }

    int size() 
    {

        return q_size;
    }

    int max_size()
    {

        return q_max_size;

    }

    bool push(const T &item)
    {

        q_mutex.lock();
        if (q_size - q_max_size >= 0)
        {

            q_cond.broadcast();
            q_mutex.unlock();
            return false;
        }

        q_back = (q_back + 1) % q_max_size;
        q_data[q_back] = item;

        q_size++;

        q_cond.broadcast();
        q_mutex.unlock();
        return true;
    }

    bool pop(T &item)
    {
        q_mutex.lock();
        while (q_size <= 0)
        { 
            if (q_cond.wait(q_mutex.get()) != 0)
            {
                q_mutex.unlock();
                return false;
            }
        }

        q_front = (q_front + 1) % q_max_size;
        item = q_data[q_front];
        q_size--;
        q_mutex.unlock();
        return true;
    }




private:
    locker q_mutex;
    cond q_cond;

    T *q_data;
    int q_size;
    int q_max_size;
    int q_front;
    int q_back;
};

class Log
{
public:

    static Log *get_instance()
    {
        static Log instance;
        return &instance;
    }

    static void *flush_log_thread(void *args)
    {
        Log::get_instance()->async_write_log();
    }
    //初始化
    bool init(const char *file_name, int log_buf_size = 8192, int split_lines = 100000);

    void write_log(int level, const char *format, ...);

    void flush(void);

private:
    Log();
    virtual ~Log();
    void *async_write_log()
    {
        string single_log;
        
        while (log_q->pop(single_log))
        {
            m_mutex.lock();
            fputs(single_log.c_str(), m_fp);
            m_mutex.unlock();
        }
        //return;
    }

private:
    char log_dir[128]; //路径名
    char log_file[128]; //log文件名
    int m_split_lines;  //日志最大行数
    int m_log_buf_size; //日志缓冲区大小
    long long m_count;  //日志行数记录
    int m_today;        //当前日期
    FILE *m_fp;         //日志文件指针
    char *m_buf;
    block_queue<string> *log_q; //阻塞队列
    locker m_mutex;
};


#define LOG_DEBUG(format, ...) Log::get_instance()->write_log(0, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) Log::get_instance()->write_log(1, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...) Log::get_instance()->write_log(2, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) Log::get_instance()->write_log(3, format, ##__VA_ARGS__)

#endif
