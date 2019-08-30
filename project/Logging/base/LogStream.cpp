#include "LogStream.h"
#include <algorithm>
#include <stdint.h>

template<typename T>
size_t convert(char buf[], T value)
{
  T i = value;
  char* p = buf;
  if(i < 0)
		i = i * -1;
  do
  {
	  int val = i % 10;
	  *p++ = val + '0';
	  i /= 10;
  } while (i != 0);

  if (value < 0)
  {
    *p++ = '-';
  }
  *p = '\0';
  std::reverse(buf, p);

  return p - buf;
}

const char digitsHex[] = "0123456789ABCDEF";

size_t convertHex(char buf[], uintptr_t value)
{
    uintptr_t i = value;
    char* p = buf;
    do{
        int lsd = static_cast<int>(i % 16);
        i /= 16;
        *p++ = digitsHex[lsd];
    }while(i != 0);
    *p = '\0';

    std::reverse(buf, p);
    return p - buf;
}

template class FixdBuffer<KLargeBuffer>;
template class FixdBuffer<KSmallBuffer>;

template<typename T>
void LogStream::formatInteger(T value)
{
    if(_buf.avail() > KMaxNumberSize){
        int len = convert(_buf.current(), value);
        _buf.add(len);
    }
}

LogStream& LogStream::operator<<(unsigned short v)
{
    int val = static_cast<unsigned int>(v);
    this->formatInteger(val);
    return *this;
}

LogStream& LogStream::operator<<(short v)
{
    int val = static_cast<int>(v);
    this->formatInteger(val);
    return *this;
}

LogStream& LogStream::operator<<(int v)
{
    this->formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned int v)
{
    this->formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(long v)
{
    this->formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long v)
{
    this->formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long long v)
{
    this->formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(long long v)
{
    this->formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(double v)
{
    if(_buf.avail() > KMaxNumberSize){
        int len = snprintf(_buf.current(), KMaxNumberSize, "%.12g", v);
        _buf.add(len);
    }
    return *this;
}

LogStream& LogStream::operator<<(long double v)
{
    if(_buf.avail() > KMaxNumberSize){
        int len = snprintf(_buf.current(), KMaxNumberSize, "%.12Lg", v);
        _buf.add(len);
    }
    return *this;
}

LogStream& LogStream::operator<<(const void* p)
{
    uintptr_t v = (uintptr_t)(p);
    if(_buf.avail() >= KMaxNumberSize)
    {
        char* buf = const_cast<char*>(_buf.current());
        buf[0] = '0';
        buf[1] = 'x';
        size_t len = convertHex(buf + 2, v);
        _buf.add(len + 2);
    }
    return *this;
}
