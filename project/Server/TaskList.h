//
// Created by soft01 on 2019/9/11.
//

#ifndef SERVER_TASKLIST_H
#define SERVER_TASKLIST_H

#include "../net/Timestamp.h"
#include "../net/TimerHeap.hpp"
#include "DbManager.h"

#include <string>
#include <memory>


using taskItem = std::pair<Timestamp, std::string>;

class TaskList {
public:
    TaskList();
    ~TaskList();

    TaskList(const TaskList&) = delete;
    TaskList& operator=(const TaskList&) = delete;

    void push(int time, const std::string& orderlist);

    void check();

    bool remove(const std::string& orderlist);

    bool exist(const std::string& orderlist);

private:
    void handleTask();

private:
    std::unique_ptr<TimerHeap<taskItem> > timerHeap_;
    std::vector<taskItem > taskList_;
    bool handleing_;
    DbManager dbManager_;
};


#endif //SERVER_TASKLIST_H
