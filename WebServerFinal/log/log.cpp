#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>
#include "log.h"
#include <pthread.h>
using namespace std;

Log::Log()
{
    m_count = 0;
}

Log::~Log()
{
    fclose(m_fp);
    
}
bool Log::init(const char *file_name, int log_buf_size, int split_lines)
{

    m_log_buf_size = log_buf_size;
    m_buf = new char[m_log_buf_size];
    memset(m_buf, '\0', m_log_buf_size);
    m_split_lines = split_lines;

    time_t t = time(NULL);
    struct tm *time_sys = localtime(&t);
    struct tm today_date = *time_sys;

 
    const char *p = strrchr(file_name, '/');
    char log_full_name[256] = {0};

    if (p == NULL)
    {
        snprintf(log_full_name, 255, "%d_%02d_%02d_%s", today_date.tm_year + 1900, today_date.tm_mon + 1, today_date.tm_mday, file_name);
    }
    else
    {
        strcpy(log_file, p + 1);
        strncpy(log_dir, file_name, p - file_name + 1);
        snprintf(log_full_name, 255, "%s%d_%02d_%02d_%s", log_dir, today_date.tm_year + 1900, today_date.tm_mon + 1, today_date.tm_mday, log_file);
    }

    m_today = today_date.tm_mday;

    m_fp = fopen(log_full_name, "a");
    if (m_fp == NULL)
    {
        return false;
    }

    return true;
}

void Log::write_log(int level, const char *format, ...)
{
    struct timeval now = {0, 0};
    gettimeofday(&now, NULL);
    time_t t = now.tv_sec;
    struct tm *time_sys = localtime(&t);
    struct tm today_date = *time_sys;
    char s[16] = {0};
    if(level==0)
        strcpy(s, "[DEBUG]:");
    else if(level==1)
        strcpy(s, "[INFNOTE]:");
    else if(level==2)
        strcpy(s, "[WARNING]:");
    else if(level==3)
        strcpy(s, "[ERROR]:");
    else 
        strcpy(s, "[NOTE]:");

    m_mutex.lock();

    m_count++;
    if (m_today != today_date.tm_mday || m_count % m_split_lines == 0) 
    {
        char new_log[256] = {0};
        fflush(m_fp);
        fclose(m_fp);
        char tail[16] = {0};
       
        snprintf(tail, 16, "%d_%02d_%02d_", today_date.tm_year + 1900, today_date.tm_mon + 1, today_date.tm_mday);
       
        if (m_today != today_date.tm_mday)
        {
            snprintf(new_log, 255, "%s%s%s", log_dir, tail, log_file);
            m_today = today_date.tm_mday;
            m_count = 0;
        }
        else
        {
            snprintf(new_log, 255, "%s%s%s.%lld", log_dir, tail, log_file, m_count / m_split_lines);
        }
        m_fp = fopen(new_log, "a");
    }
 
    m_mutex.unlock();

    va_list valst;
    va_start(valst, format);

    string log_str;
    m_mutex.lock();

    int n = snprintf(m_buf, 48, "%d-%02d-%02d %02d:%02d:%02d.%06ld %s ",
                     today_date.tm_year + 1900, today_date.tm_mon + 1, today_date.tm_mday,
                     today_date.tm_hour, today_date.tm_min, today_date.tm_sec, now.tv_usec, s);
    
    int m = vsnprintf(m_buf + n, m_log_buf_size - 1, format, valst);
    m_buf[n + m] = '\n';
    m_buf[n + m + 1] = '\0';
    log_str = m_buf;

    m_mutex.unlock();

    m_mutex.lock();
    fputs(log_str.c_str(), m_fp);
    m_mutex.unlock();
    

    va_end(valst);
    return ;
}

void Log::flush(void)
{
    m_mutex.lock();
    fflush(m_fp);
    m_mutex.unlock();
    return;
}
