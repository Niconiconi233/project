#ifndef FILESERV_FILE_H
#define FILESERV_FILE_H
#include <sys/stat.h>
#include <string>
#include <cerrno>
#include <cassert>
#include <fcntl.h>
#include <unistd.h>

#include "../Logging/base/Logging.h"

class File{
public: 
    File(const std::string filename)
        :fileName_(filename),
        fd_(-1)
    {
        init();
    }

    File()
        :fileName_(),
        fd_(-1)
    {

    }

    ~File()
    {
        if(fd_ != -1)
            ::close(fd_);
    }

    void setPath(const std::string filename)
    {
        fileName_ = std::move(filename);
        init();
    }

    void init()
    {
        int ret = stat(fileName_.c_str(), &filestat_);
        if(ret != 0){
            LOG_ERROR << "File::file " << strerror(errno);
            have_file = false;
        }
        have_file = true;

    }

    size_t getFileSize()
    {
        if(have_file)
            return filestat_.st_size;
        else
            return 0;
    }

    bool is_file()
    {
        if(have_file)
            return S_ISREG(filestat_.st_mode);
        else
            return false;
    }

    int get_fd()
    {
        if(have_file)
            if(fd_ == -1)
            {
                fd_ = ::open(fileName_.c_str(), O_RDONLY | O_CLOEXEC | O_CREAT);
                if(fd_ < 0)
                {
                    LOG_ERROR << "get_fd failed";
                }
                return fd_;
            }
        return fd_;

    }

    bool can_read()
    {
        if(have_file)
            return ::access(fileName_.c_str(), R_OK) == 0 ? true : false;
        else
            return false;
    }

    bool can_write()
    {
        if(have_file)
            return ::access(fileName_.c_str(), W_OK) == 0 ? true : false;
        else
            return false;
    }

    std::string get_file_data()
    {
        if(have_file && can_read())
        {
            char buf[1024 * 8];
            std::string res;
            do{
                int n = ::read(get_fd(), buf, sizeof buf);
                res.append(buf, n);
                if(n == 0)
                    break;
            }while(true);
            return res;
        }else
        {
            LOG_FATAL << "no such file " << fileName_;
            return std::string{};
        }
    }

    void close()
    {
        if(fd_ != -1)
        {
            ::close(fd_);
            fd_ = -1;
        }
    }

    bool ensure()
    {
        return have_file;
    }

private: 
    std::string fileName_;
    struct stat filestat_;
    int fd_;
    bool have_file{false};
};

#endif // !FILESERV_FILE_H
