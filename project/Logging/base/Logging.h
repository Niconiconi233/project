#ifndef _LOGGING_H
#define _LOGGING_H

#include "LogStream.h"
#include <string>

  enum LogLevel
  {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    NUM_LOG_LEVELS,
  };

class Logger
{
public:
    Logger(const LogLevel& level,const char* filename, int line);
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    LogStream& stream(){return impl.stream_;}

    static void setLogFileName(std::string& filename)
    {
        logFileName_ = filename;
    }

    static const std::string& getLogFileName()
    {
        return logFileName_;
    }

    static void setLevel(LogLevel level)
    {
        g_LogLevel_ = level;
    }
private:
    class Impl{
        public:
            Impl(const LogLevel& level, const char* filename, int line);
            void doLog();
            void formaitTime();
            LogStream stream_;
            LogLevel level_;
            int line_;
            std::string basename_;
    };
    Impl impl;
    static std::string logFileName_;
    static LogLevel g_LogLevel_;
};

#define LOG_LOG Logger(INFO, __FILE__, __LINE__).stream()
#define LOG_TRACE Logger(TRACE, __FILE__, __LINE__).stream()
#define LOG_DEBUG Logger(DEBUG, __FILE__, __LINE__).stream()
#define LOG_WARN Logger(WARN, __FILE__, __LINE__).stream()
#define LOG_ERROR Logger(ERROR, __FILE__, __LINE__).stream()
#define LOG_FATAL Logger(FATAL, __FILE__, __LINE__).stream()

#endif // !_LOGGING_H
