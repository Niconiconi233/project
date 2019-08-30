#ifndef _NONCOPYABLE_H
#define _NONCOPYABLE_H

class noncopyable
{
protected:
    noncopyable() {}
    ~noncopyable() {}
private:
    noncopyable(const noncopyable&);
    const noncopyable& operator=(const noncopyable&);
};

#endif // _NONCOPYABLE_H
