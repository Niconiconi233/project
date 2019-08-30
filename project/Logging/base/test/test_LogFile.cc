#include "../Logging.h"
#include "../Thread.h"
#include <string>
#include <unistd.h>
#include <vector>
#include <memory>
#include <iostream>
using namespace std;

void threadFunc()
{
    for (int i = 0; i < 100000; ++i)
    {
	LOG_ERROR << "fddsa" << 'c' << 0 << 3.666 << string("This is a string");
	LOG_LOG << "fddsa" << 'c' << 0 << 3.666 << string("This is a string");
	LOG_DEBUG << "fddsa" << 'c' << 0 << 3.666 << string("This is a string");
	LOG_WARN << "fddsa" << 'c' << 0 << 3.666 << string("This is a string");
	LOG_FATAL << "fddsa" << 'c' << 0 << 3.666 << string("This is a string");
    }
}

void type_test()
{
    // 13 lines
    cout << "----------type test-----------" << endl;
    LOG_ERROR << 0;
    LOG_ERROR << 1234567890123;
    LOG_ERROR << 1.0f;
    LOG_ERROR << 3.1415926;
    LOG_ERROR << (short) 1;
    LOG_ERROR << (long long) 1;
    LOG_ERROR << (unsigned int) 1;
    LOG_ERROR << (unsigned long) 1;
    LOG_ERROR << (long double) 1.6555556;
    LOG_ERROR << (unsigned long long) 1;
    LOG_ERROR << 'c';
    LOG_ERROR << "abcdefg";
    LOG_ERROR << string("This is a string");
}

void stressing_single_thread()
{
    // 100000 lines
    cout << "----------stressing test single thread-----------" << endl;
    for (int i = 0; i < 100000; ++i)
    {
        LOG_ERROR << i;
    }
}

void stressing_multi_threads(int threadNum = 4)
{
    // threadNum * 100000 lines
    cout << "----------stressing test multi thread-----------" << endl;
    vector<shared_ptr<Thread>> vsp;
    for (int i = 0; i < threadNum; ++i)
    {
        shared_ptr<Thread> tmp(new Thread(threadFunc, "testFunc"));
        vsp.push_back(tmp);
    }
    for (int i = 0; i < threadNum; ++i)
    {
        vsp[i]->start();
    }
    sleep(3);
}

void other()
{
    // 1 line
    cout << "----------other test-----------" << endl;
    LOG_ERROR << "fddsa" << 'c' << 0 << 3.666 << string("This is a string");
	LOG_LOG << "fddsa" << 'c' << 0 << 3.666 << string("This is a string");
	LOG_DEBUG << "fddsa" << 'c' << 0 << 3.666 << string("This is a string");
	LOG_WARN << "fddsa" << 'c' << 0 << 3.666 << string("This is a string");
	LOG_FATAL << "fddsa" << 'c' << 0 << 3.666 << string("This is a string");
}


int main()
{
    // 共500014行
	    Logger::setLevel(INFO);
    type_test();
    sleep(3);

    stressing_single_thread();
    sleep(3);

    other();
    sleep(3);


    stressing_multi_threads();

    sleep(5);
    return 0;
}
