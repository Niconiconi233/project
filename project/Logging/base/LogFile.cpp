#include "MutexLock.h"
#include "FileUtil.h"
#include "LogFile.h"
#include <unistd.h>
#include <string>

LogFile::LogFile(const std::string& name, int everyn)
    : basename(name),
    everyN(everyn),
    count(0)
{
    file_.reset(new Appendfile(name));
    mutex_.reset(new MutexLock());
}

LogFile::~LogFile()
{

}

void LogFile::flush()
{
    file_->flush();
}

void LogFile::append(const char* line, size_t len)
{
    MutexLockGurard lock(*mutex_);
    _append(line, len);
}

void LogFile::_append(const char* line, size_t len)
{
    file_->append(line, len);
    ++count;
    if(count >= everyN){
        file_->flush();
        count = 0;
    }
}
