#ifndef _LOGSTREAM_H
#define _LOGSTREAM_H
/*
    重载运算符， 和第二层缓冲区
*/

#include "noncopyable.h"
#include <string.h>
#include <string>

const int KSmallBuffer = 4000;
const int KLargeBuffer = 4000 * 1000;

template<int SIZE>
class FixdBuffer : noncopyable
{
public:
    FixdBuffer():cur(data){}
    ~FixdBuffer(){}

    const char* begin() const {return data;}
    size_t avail() {return end() - cur;}
    size_t len() const {return cur - data;}
    void add(int n){cur += n;}
    char* current(){return cur;}

    void append(const char* line, size_t len){
        if(avail() > len){
            memcpy(cur, line, len);
            cur += len;
        }
    }

    void bzero(){
        memset(data, 0, sizeof data);
        cur = data;
    }

    void reset(){
        cur = data;
    }

private:
    char data[SIZE];
    char *cur;

private:
       const char* end()
       {
        char* p = data;
        return p + SIZE;
       }
};

class LogStream : noncopyable
{
    typedef LogStream self;
public:
    typedef FixdBuffer<KSmallBuffer> buffer;
    self& operator<<(unsigned short);
    self& operator<<(short);
    self& operator<<(unsigned int);
    self& operator<<(int);
    self& operator<<(unsigned long);
    self& operator<<(long);
    self& operator<<(unsigned long long);
    self& operator<<(long long);
    self& operator<<(const void*);
    self& operator<<(double);
    self& operator<<(long double);

    self& operator<<(float v)
    {
        *this<<static_cast<double>(v);
        return *this;
    }

    self& operator<<(char c)
    {
        _buf.append(&c, 1);
        return *this;
    }

    self& operator<<(const char* str)
    {
        if(str){
            _buf.append(str, strlen(str));
        }else{
            _buf.append("(null)", 6);
        }
        return *this;
    }

    self& operator<<(const unsigned char* str)
    {
        *this<<reinterpret_cast<const char*>(str);
        return *this;
    }
    
    self& operator<<(const std::string& str)
    {
        _buf.append(str.c_str(), str.size());
        return *this;
    }

    void append(const char* str, size_t len)
    {
        _buf.append(str, len);
    }

    const buffer& get_buf()
    {
        return _buf;
    }

    void reset()
    {
        _buf.reset();
    }
    
private:
    buffer _buf;
    static const int KMaxNumberSize = 32;

private:
    template<typename T>
    void formatInteger(T value);

};

#endif // !_LOGSTREAM_H
