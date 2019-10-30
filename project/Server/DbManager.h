//
// Created by soft01 on 2019/9/4.
//

#ifndef SERVER_DBMANAGER_H
#define SERVER_DBMANAGER_H

//#include <boost/thread/mutex.hpp>
//#include <boost/thread/shared_mutex.hpp>

#include <string>


class RapidJson;

class DbManager {
//    using WR_Mutex = boost::shared_mutex;
//    using writeLock = boost::unique_lock<WR_Mutex>;
//    using readLock = boost::shared_lock<WR_Mutex>;
public:
    DbManager(){}
    ~DbManager(){}

    DbManager(const DbManager&) = delete;
    DbManager& operator=(const DbManager&) = delete;

    //判断用户登陆相关返回用户id
    int checkUserAccount(const std::string& account, const std::string& pwd);

    //判断邮箱是否存在
    bool accountExists(const std::string& account);

    //添加用户
    bool addUserInDB(const std::string& account, const std::string& pwd, const std::string& name);

    //随机获取数据
    bool getProductionByRand(RapidJson& json);

    //获取用户详细信息
    bool getUserInfoByUid(RapidJson& json, unsigned int uid);

    //关键字搜索
    bool getProductionByName(RapidJson& json, const std::string& kw);

    //类别搜索
    bool getProductionByCid(RapidJson& json, unsigned int cid);

    //修改用户信息
    bool modUserInfoByUid(const std::string& nickname, const std::string& sex, const std::string& description, unsigned int uid);

    //修改商品信息
    bool modProductionByPid(const std::string& name, const std::string& description, double price, unsigned int pid);

    //聊天信息持久化
    bool addChatMessage(unsigned int suid, unsigned int ruid, const std::string& message);

    //获取商品详细信息
    bool getProductionByPid(RapidJson& json, unsigned int pid);

    //获取用户发布的商品
    bool getUserProduction(RapidJson& json, unsigned int uid);

    //点赞
    bool addLikeCount(unsigned int pid);

    //获取用户评价
    bool getProductionMessageByPid(RapidJson& json, unsigned int pid);

    //添加用户评价
    bool addProductionMessageByPid(unsigned int uid, unsigned int pid, const std::string& message);

    //检查商品所有权
    bool checkProductionUser(unsigned pid, unsigned uid);

    //订阅用户
    bool addSubscribeUserList(unsigned int uid, unsigned int suid);

    //获取订阅列表
    bool getSubscribeUserList(RapidJson& json, unsigned int uid, bool total);

    //订阅商品
    bool addSubscribeProductionList(unsigned int uid, unsigned int pid);

    //订阅商品
    bool getSubscribeProductionList(RapidJson& json, unsigned int uid, bool total);

    //购买订单插入
    bool doOrder(unsigned int uid, unsigned int pid, std::string& orderlist);

    //订单失效
    bool releaseOrder(const std::string& orderlist, unsigned int pid);

    //历史订单
    bool getHistoryOrderList(RapidJson& json, unsigned int uid);

    //未完成订单
    bool getUnfinishOrderList(RapidJson& json, unsigned int uid);

    bool releaseWithoutGuarantee(unsigned uid, unsigned cid, const std::string& name, const std::string& description, const std::string& images, const std::string& addr, double price);

    bool releasewithGuarantee(unsigned uid, unsigned cid, const std::string& name, const std::string& description, const std::string& images, const std::string& addr, double price);

//private:
//    WR_Mutex mutex_;

};


#endif //SERVER_DBMANAGER_H
