//
// Created by soft01 on 2019/9/11.
//

#include "../TaskList.h"
#include "../../net/EventLoop.h"

int main()
{
    EventLoop loop;
    TaskList task;
    task.push(5, "5");
    task.push(10, "10");
    task.push(12, "12");
    task.push(14, "14");
    task.push(17, "17");
    task.push(19, "19");
    task.push(3, "3");
    task.push(7, "7");
    task.push(11, "11");
    loop.runEvery(1,std::bind(&TaskList::check, &task));
    loop.loop();

    return 0;
}

