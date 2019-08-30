#include "FileUtil.h"
#include <string>
#include <stdio.h>

Appendfile::Appendfile(const std::string& name)
    : fp(fopen(name.c_str(), "ae"))
{
    setbuffer(fp, buf, sizeof buf);
}

Appendfile::~Appendfile()
{
    fflush(fp);
    fclose(fp);
}

void Appendfile::flush()
{
    fflush(fp);
}

void Appendfile::append(const char* line, size_t size)
{
    int n = this->write(line, size);
    int nleft = size - n;
    while(nleft > 0){
        int nwrite = this->write(line + n, nleft);
        if(nwrite <= 0){
            fprintf(stderr, "append error");
            break;
        }
        nleft -= nwrite;
        n += nwrite;
    }
}

size_t Appendfile::write(const char* line, size_t n)
{
    int size = fwrite_unlocked(line, 1, n, fp);
    return size;
}

