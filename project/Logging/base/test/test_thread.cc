#include "Thread.h"
#include <functional>
#include <stdio.h>
#include <unistd.h>

void func()
{
    sleep(5);
    printf("%s\n", "in thread");
}


int main()
{
    Thread* tid[4];
    for(int i = 0; i < 4; ++i)
    {
        tid[i] = new Thread(std::bind(&func), "test func");
    }
    for(int i = 0; i < 4; ++i)
        tid[i]->start();
     for(int i = 0; i < 4; ++i)
        printf("%d\n", tid[i]->tid());   
    return 0;
}