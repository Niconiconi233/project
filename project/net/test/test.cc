//
// Created by soft01 on 2019/7/30.
//

//#include "../EventLoop.h"
//#include "../Timestamp.h"
//#include "../../Logging/base/Logging.h"

//#include "../TimerHeap.hpp"
//#include "../Timer.h"
#include "../File.h"

#include <iostream>
#include <map>
#include <unistd.h>

using namespace std;

/*void runatcb()
{
    std::cout<<"run at cb"<<std::endl;
}

void runaftercb()
{
    std::cout<<"in run after cb"<<std::endl;
}

void runeverycb()
{
    std::cout<<"in run every cb time 5"<<std::endl;
}

void runeverycb2()
{
    std::cout<<"in run every cb time 9"<<std::endl;
}

using NodeItem = std::pair<Timestamp, uint64_t>;

bool compare(const NodeItem& lhs, const NodeItem& rhs)
{
    return lhs.second == rhs.second;
}

bool NodeCompare(const NodeItem& lhs, const NodeItem& rhs)
{
    return lhs.first > rhs.first;
}

bool cm(int i, int j)
{
    return i > j;
}*/

int main()
{
    /*auto cb = std::bind(&runatcb);
    EventLoop loop;
    loop.runAtWithHeap(addTime(Timestamp::now(), 3), std::bind(&runatcb));
    loop.runEveryWithHeap(5, std::bind(&runeverycb));
    loop.runAfterWithHeap(7, std::bind(&runaftercb));
    loop.runEveryWithHeap(9, std::bind(&runeverycb2));
    loop.loop();
    Timestamp when1(addTime(Timestamp::now(), 5));
    Timestamp when2(addTime(Timestamp::now(), 3));
    Timestamp when3(addTime(Timestamp::now(), 8));
    Timestamp when4(addTime(Timestamp::now(), 7));
    TimerHeap<NodeItem> heap(std::bind(&NodeCompare, std::placeholders::_1, std::placeholders::_2));
    Timer tm1(cb, when1, 0);
    Timer tm2(cb, when2, 5);
    Timer tm3(cb, when3, 0);
    Timer tm4(cb, when4, 7);
    std::map<int64_t, Timer*> map;
    map[tm1.sequence()] = &tm1;
    map[tm2.sequence()] = &tm2;
    map[tm3.sequence()] = &tm3;
    map[tm4.sequence()] = &tm4;
    heap.insert(std::make_pair(tm1.expiration(), tm1.sequence()));
    heap.insert(std::make_pair(tm2.expiration(), tm2.sequence()));
    heap.insert(std::make_pair(tm3.expiration(), tm3.sequence()));
    heap.insert(std::make_pair(tm4.expiration(), tm4.sequence()));
    bool ret = heap.remove(std::make_pair(tm3.expiration(), tm3.sequence()), std::bind(&compare, std::placeholders::_1, std::placeholders::_2));

    while(true)
    {
        if(heap.isEmpty())
            break;
        NodeItem i;
        heap.getAndDelete(i);
        cout<<i.first.toFormattedString()<<endl;
        if(map[i.second]->repeat())
        {
            Timestamp now(Timestamp::now());
            map[i.second]->restart(now);
            heap.insert(std::make_pair(map[i.second]->expiration(), i.second));
        }
        cout<<endl;
        sleep(3);
    }*/

    File file("1.txt");
    char str[] = "nmsl\t\nwdnmd\n";
    if(file.ensure())
    {
        if(file.get_fd())
            file.write(str, sizeof str);
    } else
    {
        cout<<"error"<<endl;
    }

    return 0;
}
