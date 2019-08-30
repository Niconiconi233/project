#include "Logging.h"
#include "AsyncLogging.h"
#include "CurrentThread.h"
#include <iostream>
#include <sys/time.h>
#include <pthread.h>

static pthread_once_t once_control_ = PTHREAD_ONCE_INIT;
static AsyncLogging* AsyncLogger_;

std::string Logger::logFileName_ = "web_server.log";
LogLevel Logger::g_LogLevel_ = LogLevel::DEBUG;

void once_init()
{
    AsyncLogger_ = new AsyncLogging(Logger::getLogFileName());
    AsyncLogger_->start();
}

void output(const char* msg, int len)
{
    pthread_once(&once_control_, once_init);
    AsyncLogger_->append(msg, len);
}

const char* LogLevelName[NUM_LOG_LEVELS] =
{
  "TRACE ",
  "DEBUG ",
  "INFO  ",
  "WARN  ",
  "ERROR ",
  "FATAL ",
};

Logger::Impl::Impl(const LogLevel& level, const char* filename, int line)
    : stream_(),
    level_(level),
    line_(line),
    basename_(filename)
{
    formaitTime();
}

void Logger::Impl::formaitTime()
{
    struct timeval tv;
    time_t time;
    char str_t[36] = {0};
    gettimeofday(&tv, NULL);
    time = tv.tv_sec;
    struct tm* p_time = localtime(&time);
    strftime(str_t, sizeof str_t, "%Y-%m-%d %H:%M:%S.", p_time);
    char buf[10];
    snprintf(buf, sizeof buf, "%06ld ", tv.tv_usec);
    strcat(str_t, buf);
    stream_<<str_t<<CurrentThread::tid()<<" "<<LogLevelName[level_]<<" ";
}

Logger::Logger(const LogLevel& level, const char* filename, int len)
    : impl(level, filename, len)
{

}

Logger::~Logger()
{
    impl.stream_ << "--" << impl.basename_ << "--" << impl.line_ << "\n";
    if(impl.level_ >= g_LogLevel_){//判断日志等级 否则直接丢弃
        const LogStream::buffer& buf(stream().get_buf());
        output(buf.begin(), buf.len());//save to AsyncLogging
    }
}
