#ifndef _FILEUTIL_H
#define _FILEUTIL_H

/*
Loger的实际写入类
*/

#include "noncopyable.h"
#include <string>
#include <cstddef>

class Appendfile : noncopyable
{
public:
    Appendfile(const std::string& name);
    ~Appendfile();
    void append(const char* line, size_t size);
    void flush();

private:
    FILE* fp;
    char buf[64 * 1024];
    size_t write(const char*, size_t);
    
};

#endif // !_FILE_UTIL_H
