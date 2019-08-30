#ifndef _LOGFILE_H
#define _LOGFILE_H
/*
包装类 实现对缓冲的定时刷新和同步
*/

#include "MutexLock.h"
#include "FileUtil.h"
#include "noncopyable.h"
#include <memory>
#include <string>

class LogFile : noncopyable
{
public:
    LogFile(const std::string& name, int everyn = 1024);
    ~LogFile();

    void append(const char* line, size_t len);
    void flush();

private:
    const std::string basename;
    const int everyN;
    int count;
    std::unique_ptr<MutexLock> mutex_;
    std::unique_ptr<Appendfile> file_;

private:
    void _append(const char* line, size_t len);

};


#endif // !_LOGFILE_H
