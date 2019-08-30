//
// Created by soft01 on 2019/8/8.
//

#ifndef SERVER_REDIS_H
#define SERVER_REDIS_H

#include <memory>
#include <vector>
#include <hiredis/hiredis.h>


class Redis {
public:
    Redis();

    Redis(const Redis&) = delete;

    Redis& operator=(const Redis&) = delete;

    ~Redis();

    void init();
                            /*字符串操作相关*/
    //使用默认超时的设置k-v
    void setStringValue(unsigned int key, const std::string& value);

    //自定义超时的设置k-v
    void setStringValue(unsigned int key, const std::string& value, int timeout);

    //使用默认超时的设置K-V
    void setStringValue(const std::string& key, unsigned int value);

    //使用自定义超时的K-V
    void setStringValue(const std::string& key, unsigned int value, int timeout);

    //使用默认超时的k-v
    void setStringValue(unsigned int key, int value);

    //自定义超时的k-v
    void setStringValue(unsigned int key, int value, int timeout);

    //获取k
    std::string getStringValue(const std::string& key);

    std::string getStringValue(unsigned int key);


                                    /*集合操作 保存用户浏览历史*/
    //使用默认超时的设置k-v 如果存在则是添加
    void setSetValue(unsigned int key, unsigned int value);

    //自定义超时的设置k-v 如果存在则是添加
    void setSetValue(unsigned int key, unsigned int value, int time);

    //获取v
    std::vector<unsigned int> getSetValue(unsigned int key);

                                                /*sort set 操作*/
    //使用默认超时的设置k-v 如果存在则是修改
    void setSortSetValue(const std::string& key, unsigned int value);

    //增长
    void sortSetIncr(const std::string& key, int num);

    //get top 6
    std::vector<std::string> sortSetTop();



    //判断k是否存在
    bool existsKey(const std::string& key);

    bool existsKey(unsigned int key);

    //切换表
    void selectTable(int num);

    bool hasExpire(const std::string& key);

    bool hasExpire(unsigned int key);



private:
    redisContext* connect_;
    redisReply* reply_;
    int timeout_{86400};//默认超时时间为24小时


};


#endif //SERVER_REDIS_H
