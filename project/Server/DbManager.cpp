//
// Created by soft01 on 2019/9/4.
//

#ifdef debug
#include <chrono>
#endif

#include <sys/time.h>
#include <stdio.h>
#include <string.h>


#include "RapidJson.h"
#include "DbManager.h"
#include "connectionPool.h"
#include "../Logging/base/Logging.h"
#include "../Logging/base/SingLeton.h"

namespace
{
    std::string getTime()
    {
        struct timeval tv;
        time_t time;
        char str_t[32] = {0};
        gettimeofday(&tv, NULL);
        time = tv.tv_sec;
        struct tm* p_time = localtime(&time);
        strftime(str_t, sizeof str_t, "%Y%m%d%H%M%S", p_time);
        char buf[10];
        snprintf(buf, sizeof buf, "%06ld ", tv.tv_usec);
        strcat(str_t, buf);
        return std::string(str_t, 18);
    }
}

int DbManager::checkUserAccount(const std::string &account, const std::string &pwd)
{
    ConnectionPool& mysqlpool = Singletion<ConnectionPool>::instance();
    auto mysqlconn = mysqlpool.getConnection();
    int id = -1;
    std::unique_ptr<sql::PreparedStatement> prep_stmt(mysqlconn->prepareStatement("select id from t_user where email=? and pwd=md5(concat(?, ?))"));
    prep_stmt->setString(1, account);
    prep_stmt->setString(2, account);
    prep_stmt->setString(3, pwd);
    std::unique_ptr<sql::ResultSet> res;
    try {
#ifdef debug
        auto begin = std::chrono::high_resolution_clock::now();
#endif
        res.reset(prep_stmt->executeQuery());
#ifdef debug
        auto end = std::chrono::high_resolution_clock::now();
        LOG_DEBUG <<"checkUserAccount select sql speed :"<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
#endif
        if(res->next())
            id = res->getInt("id");

        mysqlpool.close(mysqlconn);
        return id;
    }catch (sql::SQLException& e)
    {
        LOG_ERROR << "DbManager::checkUserAccount catch exception " << e.what();
        mysqlpool.close(mysqlconn);
        return false;
    }
}

bool DbManager::accountExists(const std::string &account)
{
    auto& mysqlpool = Singletion<ConnectionPool>::instance();
    auto mysqlconn = mysqlpool.getConnection();
    bool flag = false;
    std::unique_ptr<sql::PreparedStatement> prep_stmt(mysqlconn->prepareStatement("select id from t_user where email=?"));
    prep_stmt->setString(1, account);
    std::unique_ptr<sql::ResultSet> res;
    try {
#ifdef debug
        auto begin = std::chrono::high_resolution_clock::now();
#endif
        res.reset(prep_stmt->executeQuery());
#ifdef debug
        auto end = std::chrono::high_resolution_clock::now();
        LOG_DEBUG <<"accountExists select sql speed :"<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
#endif
        if(!res->next())
            flag = false;
        else
            flag = true;
        mysqlpool.close(mysqlconn);
        return flag;
    }catch (sql::SQLException& e)
    {
        LOG_ERROR << "DbManager::accountExists catch exception " << e.what();
        mysqlpool.close(mysqlconn);
        return false;
    }
}

bool DbManager::addUserInDB(const std::string &account, const std::string &pwd, const std::string& name)
{
    auto& mysqlpool = Singletion<ConnectionPool>::instance();
    auto mysqlconn = mysqlpool.getConnection();
    int ret = 0;
    bool flag = false;
    std::unique_ptr<sql::PreparedStatement> prep_stmt(mysqlconn->prepareStatement("insert into t_user (email, pwd, nickname) values (?, md5(concat(?, ?)), ?)"));
    prep_stmt->setString(1, account);
    prep_stmt->setString(2, account);
    prep_stmt->setString(3, pwd);
    prep_stmt->setString(4, name);
    try {
#ifdef debug
        auto begin = std::chrono::high_resolution_clock::now();
#endif
        ret = prep_stmt->executeUpdate();
#ifdef debug
        auto end = std::chrono::high_resolution_clock::now();
        LOG_DEBUG <<"doRegist insert sql speed :"<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
#endif
        if(ret == 1)
            flag = true;
        else
            flag = false;
        mysqlpool.close(mysqlconn);
        return flag;
    }catch (sql::SQLException& e)
    {
        LOG_ERROR << "DbManager::addUserInDb catch exception " << e.what();
        mysqlpool.close(mysqlconn);
        return false;
    }
}

bool DbManager::getProductionByRand(RapidJson &json)
{
    auto& mysqlpool = Singletion<ConnectionPool>::instance();
    auto mysqlconn = mysqlpool.getConnection();
    bool flag = false;
    std::unique_ptr<sql::PreparedStatement> prep_stmt(mysqlconn->prepareStatement("SELECT p_id, p_name, p_price, p_wcount, p_imageaddr, uid, nickname, image FROM `production_info` AS t1 JOIN (SELECT ROUND(RAND() * ((SELECT MAX(p_id) FROM `production_info`)-(SELECT MIN(p_id) FROM `production_info`))+(SELECT MIN(p_id) FROM `production_info`)) AS id) AS t2\n"
                                                                                  "WHERE t1.p_id >= t2.id\n"
                                                                                  "ORDER BY t1.p_id LIMIT 12;"));
    std::unique_ptr<sql::ResultSet> res;
    try {
#ifdef debug
        auto begin = std::chrono::high_resolution_clock::now();
#endif
        res.reset(prep_stmt->executeQuery());
#ifdef debug
        auto end = std::chrono::high_resolution_clock::now();
        LOG_DEBUG <<"doRegist select sql 2 speed :"<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
#endif
        if(res->next())
        {
            json.setObject("status", 1);
            do{
                json.setObjectToArray("pid", res->getInt("p_id"));
                json.setObjectToArray("pname", res->getString("p_name").c_str());
                json.setObjectToArray("pimages", res->getString("p_imageaddr").c_str());
                json.setObjectToArray("watched", res->getInt("p_wcount"));
                json.setObjectToArray("price", static_cast<double>(res->getDouble("p_price")));
                json.setObjectToArray("uid", res->getInt("uid"));
                json.setObjectToArray("nickname", res->getString("nickname").c_str());
                json.setObjectToArray("image", res->getString("image").c_str());
                json.flushToArray();
            }while(res->next());
            json.flushToRoot("data");
            flag = true;
        }else
            flag = false;
        mysqlpool.close(mysqlconn);
        return flag;
    }catch (sql::SQLException& e)
    {
        LOG_ERROR << "DbManager::getProductionByRand catch exception " << e.what();
        mysqlpool.close(mysqlconn);
        return false;
    }
}

bool DbManager::getUserInfoByUid(RapidJson &json, unsigned int uid)
{
    auto& mysqlpool = Singletion<ConnectionPool>::instance();
    auto mysqlconn = mysqlpool.getConnection();
    std::unique_ptr<sql::PreparedStatement> prep_stmt(mysqlconn->prepareStatement("select nickname, description, sex, image, credit, create_time , email, phone from t_user where id = ?"));
    bool flag = false;
    prep_stmt->setInt(1, uid);
    try {
#ifdef debug
        auto begin = std::chrono::high_resolution_clock::now();
#endif
        std::unique_ptr<sql::ResultSet> res(prep_stmt->executeQuery());
#ifdef debug
        auto end = std::chrono::high_resolution_clock::now();
    LOG_DEBUG <<"doRegist select sql 2 speed :"<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
#endif
        if(res->next()) {
            json.setObject("status", 1);
            json.setObject("uid", static_cast<int>(uid));
            json.setObject("nickname", res->getString("nickname").c_str());
            json.setObject("description", res->getString("description").c_str());
            json.setObject("sex", res->getString("sex").c_str());
            json.setObject("image", res->getString("image").c_str());
            json.setObject("credit", res->getInt("credit"));
            json.setObject("create_time", res->getString("create_time").c_str());
            json.setObject("email", res->getString("email").c_str());
            json.setObject("phone", res->getString("phone").c_str());
            flag = true;
        }else
        {
            flag = false;
        }
        mysqlpool.close(mysqlconn);
        return flag;
    }catch (sql::SQLException& e)
    {
        LOG_ERROR << "DbManager::getUserInfoByUid catch exception " << e.what();
        mysqlpool.close(mysqlconn);
        return false;
    }
}

bool DbManager::getProductionByName(RapidJson &json, const std::string& kw)
{
    auto& mysqlpool = Singletion<ConnectionPool>::instance();
    auto mysqlconn = mysqlpool.getConnection();
    std::unique_ptr<sql::PreparedStatement> prep_stmt(mysqlconn->prepareStatement("select p_id, p_name, p_price, p_wcount, p_imageaddr,uid, nickname, image\n"
                                                                                  "from production_info\n"
                                                                                  "where p_name like ?\n"
                                                                                  "order by rand()\n"
                                                                                  "limit 20"));
    bool flag = false;
    prep_stmt->setString(1, "%" + kw + "%");
    try {
#ifdef debug
        auto begin = std::chrono::high_resolution_clock::now();
#endif
        std::unique_ptr<sql::ResultSet> res(prep_stmt->executeQuery());
#ifdef debug
        auto end = std::chrono::high_resolution_clock::now();
    LOG_DEBUG <<"doRegist select sql 2 speed :"<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
#endif
        if(res->next()) {
            json.setObject("status", 1);
            do {
                json.setObjectToArray("pid", res->getInt("p_id"));
                json.setObjectToArray("pname", res->getString("p_name").c_str());
                json.setObjectToArray("pimages", res->getString("p_imageaddr").c_str());
                json.setObjectToArray("price", static_cast<double>(res->getDouble("p_price")));
                json.setObjectToArray("uid", res->getInt("uid"));
                json.setObjectToArray("nickname", res->getString("nickname").c_str());
                json.setObjectToArray("image", res->getString("image").c_str());
                json.flushToArray();
            } while (res->next());
            json.flushToRoot("data");
            flag = true;
        }else
        {
            flag = false;
        }
        mysqlpool.close(mysqlconn);
        return flag;
    }catch (sql::SQLException& e)
    {
        LOG_ERROR << "DbManager::getProductionByName catch exception " << e.what();
        mysqlpool.close(mysqlconn);
        return false;
    }
}

bool DbManager::getProductionByCid(RapidJson &json, unsigned int cid)
{
    auto& mysqlpool = Singletion<ConnectionPool>::instance();
    auto mysqlconn = mysqlpool.getConnection();
    std::unique_ptr<sql::PreparedStatement> pre_stmt(mysqlconn->prepareStatement("select p_id, p_name, p_price, p_wcount, p_imageaddr,uid, nickname, image\n"
                                                                                 "from production_info\n"
                                                                                 "where p_cid = ?\n"
                                                                                 "order by rand()\n"
                                                                                 "limit 20"));

    bool flag = false;
    pre_stmt->setInt(1, cid);
    try {
#ifdef debug
        auto begin = std::chrono::high_resolution_clock::now();
#endif
        std::unique_ptr<sql::ResultSet> res(pre_stmt->executeQuery());
#ifdef debug
        auto end = std::chrono::high_resolution_clock::now();
    LOG_DEBUG <<"doRegist select sql 2 speed :"<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
#endif
        if(res->next()) {
            json.setObject("status", 1);
            do {
                json.setObjectToArray("pid", res->getInt("p_id"));
                json.setObjectToArray("pname", res->getString("p_name").c_str());
                json.setObjectToArray("pimages", res->getString("p_imageaddr").c_str());
                json.setObjectToArray("price", static_cast<double>(res->getDouble("p_price")));
                json.setObjectToArray("uid", res->getInt("uid"));
                json.setObjectToArray("nickname", res->getString("nickname").c_str());
                json.setObjectToArray("image", res->getString("image").c_str());
                json.flushToArray();
            } while (res->next());
            json.flushToRoot("data");
            //pool.close(conn);
            //return true;
            flag = true;
        }else
        {
            flag = false;
        }
        mysqlpool.close(mysqlconn);
        return flag;
    }catch (sql::SQLException& e)
    {
        LOG_ERROR << "DbManager::getProductionByPid catch exception " << e.what();
        mysqlpool.close(mysqlconn);
        return false;
    }
}

bool DbManager::modUserInfoByUid(const std::string &nickname, const std::string &sex, const std::string &description,
                                 unsigned int uid) {
    auto& mysqlpool = Singletion<ConnectionPool>::instance();
    auto mysqlconn = mysqlpool.getConnection();
    std::unique_ptr<sql::PreparedStatement> prep_stmt(mysqlconn->prepareStatement("update t_user set sex=?,nickname=?,description=? \n"
                                                                                  "where id = ?"));

    bool flag = false;
    prep_stmt->setString(1, sex);
    prep_stmt->setString(2, nickname);
    prep_stmt->setString(3, description);
    prep_stmt->setInt(4, uid);
    try {
#ifdef debug
        auto begin = std::chrono::high_resolution_clock::now();
#endif
        int ret = 0;
        ret = prep_stmt->executeUpdate();
#ifdef debug
        auto end = std::chrono::high_resolution_clock::now();
        LOG_DEBUG <<"doRegist select sql 2 speed :"<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
#endif
        if(ret == 1)
            flag = true;
        else
            flag = false;

        mysqlpool.close(mysqlconn);
        return flag;
    }catch (sql::SQLException& e)
    {
        LOG_ERROR << "DbManager::modUserInfoByUidcatch exception " << e.what();
        mysqlpool.close(mysqlconn);
        return false;
    }
}

bool DbManager::modProductionByPid(const std::string &name, const std::string &description, double price,
                                   unsigned int pid) {
    auto& mysqlpool = Singletion<ConnectionPool>::instance();
    auto mysqlconn = mysqlpool.getConnection();
    int ret = 0;
    std::unique_ptr<sql::PreparedStatement> prep_stmt(mysqlconn->prepareStatement("update t_productions set p_name=?,p_description=?,p_price=? \n"
                                                                                  "where p_id = ?"));

    bool flag = false;
    prep_stmt->setString(1, name);
    prep_stmt->setString(2, description);
    prep_stmt->setDouble(3, price);
    prep_stmt->setInt(4, pid);
    try {
#ifdef debug
        auto begin = std::chrono::high_resolution_clock::now();
#endif
        ret = prep_stmt->executeUpdate();
#ifdef debug
        auto end = std::chrono::high_resolution_clock::now();
        LOG_DEBUG <<"doRegist select sql 2 speed :"<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
#endif
        if(ret == 1)
            flag = true;
        else
            flag = false;
        mysqlpool.close(mysqlconn);
        return flag;
    }catch (sql::SQLException& e)
    {
        LOG_ERROR << "DbManager::modProductionByPid catch exception " << e.what();
        mysqlpool.close(mysqlconn);
        return false;
    }

}

bool DbManager::addChatMessage(unsigned int suid, unsigned int ruid, const std::string &message)
{
    auto& mysqlpool = Singletion<ConnectionPool>::instance();
    auto mysqlconn = mysqlpool.getConnection();
    std::unique_ptr<sql::PreparedStatement> prep_stmt(mysqlconn->prepareStatement("insert into t_message(suid, ruid, message) values(?, ?, ?)"));

    bool flag = false;
    prep_stmt->setInt(1, suid);
    prep_stmt->setInt(2, ruid);
    prep_stmt->setString(3, message);
    try {
#ifdef debug
        auto begin = std::chrono::high_resolution_clock::now();
#endif
        int ret = 0;
        ret = prep_stmt->executeUpdate();
#ifdef debug
        auto end = std::chrono::high_resolution_clock::now();
        LOG_DEBUG <<"doRegist select sql 2 speed :"<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
#endif
        if(ret == 1)
            flag = true;
        else
            flag = false;
        mysqlpool.close(mysqlconn);
        return flag;
    }catch (sql::SQLException& e)
    {
        LOG_ERROR << "DbManager::addChatMessage catch exception " << e.what();
        mysqlpool.close(mysqlconn);
        return false;
    }
}

//获取商品详细信息
bool DbManager::getProductionByPid(RapidJson &json, unsigned int pid)
{
    auto& mysqlpool = Singletion<ConnectionPool>::instance();
    auto mysqlconn = mysqlpool.getConnection();
    std::unique_ptr<sql::PreparedStatement> prep_stmt(mysqlconn->prepareStatement(
            "select p_id, p_name, p_description, p_price, p_wcount, p_imageaddr, uid\n"
            "from production_info\n"
            "where p_id = ?\n"));
    bool flag = false;
    prep_stmt->setInt(1, pid);
    try {
#ifdef debug
        auto begin = std::chrono::high_resolution_clock::now();
#endif
        std::unique_ptr<sql::ResultSet> res(prep_stmt->executeQuery());
#ifdef debug
        auto end = std::chrono::high_resolution_clock::now();
        LOG_DEBUG <<"doRegist select sql 2 speed :"<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
#endif
        if (res->next()) {
            json.setObject("status", 1);
            json.setObject("pid", res->getInt("p_id"));
            json.setObject("pname", res->getString("p_name").c_str());
            json.setObject("p_description", res->getString("p_description").c_str());
            json.setObject("pimages", res->getString("p_imageaddr").c_str());
            json.setObject("price", static_cast<double>(res->getDouble("p_price")));
            json.setObject("uid", res->getInt("uid"));
            json.setObject("wcount", res->getInt("p_wcount"));
            flag = true;
        }else
            flag = false;
        mysqlpool.close(mysqlconn);
        return flag;
    }catch (sql::SQLException& e)
    {
        LOG_ERROR << "DbManager::getProductionByPid catch exception " << e.what();
        mysqlpool.close(mysqlconn);
        return false;
    }
}

//获取用户发布的商品
bool DbManager::getUserProduction(RapidJson &json, unsigned int uid)
{
    auto& mysqlpool = Singletion<ConnectionPool>::instance();
    auto mysqlconn = mysqlpool.getConnection();
    std::unique_ptr<sql::PreparedStatement> prep_stmt(mysqlconn->prepareStatement("select p_id, p_name, p_price, p_wcount, p_imageaddr,\n"
                                                                                  "uid, nickname, image\n"
                                                                                  "from production_info\n"
                                                                                  "where uid = ?\n"
                                                                                  "limit 8"));
    prep_stmt->setInt(1, uid);
    bool flag = false;
    try {
        std::unique_ptr<sql::ResultSet> res(prep_stmt->executeQuery());

        if(res->next())
        {
            do{
                json.setObjectToArray("pid", res->getInt("p_id"));
                json.setObjectToArray("pname", res->getString("p_name").c_str());
                json.setObjectToArray("pimages", res->getString("p_imageaddr").c_str());
                json.setObjectToArray("watched", res->getInt("p_wcount"));
                json.setObjectToArray("price", static_cast<double>(res->getDouble("p_price")));
                json.setObjectToArray("uid", res->getInt("uid"));
                json.setObjectToArray("nickname", res->getString("nickname").c_str());
                json.setObjectToArray("image", res->getString("image").c_str());
                json.flushToArray();
            }while(res->next());
            json.flushToRoot("data");
            flag = true;
        }else
        {
            flag = false;
        }
        mysqlpool.close(mysqlconn);
        return flag;
    }catch (sql::SQLException& e)
    {
        LOG_ERROR << "DbManager::getUserProduction catch exception " << e.what();
        mysqlpool.close(mysqlconn);
        return false;
    }
}


bool DbManager::addLikeCount(unsigned int pid)
{
    auto& mysqlpool = Singletion<ConnectionPool>::instance();
    auto mysqlconn = mysqlpool.getConnection();
    std::unique_ptr<sql::PreparedStatement> prep_stmt(mysqlconn->prepareStatement("update t_productions set p_wcount = p_wcount + 1 where p_id = ?"));

    bool flag = false;
    int ret = -1;
    prep_stmt->setInt(1, pid);
    try {
        ret = prep_stmt->executeUpdate();
        if(ret == 1)
            flag = true;
        else
            flag = false;
        mysqlpool.close(mysqlconn);
        return flag;
    }catch (sql::SQLException& e)
    {
        LOG_ERROR << "DbManager::getUserProduction catch exception " << e.what();
        mysqlpool.close(mysqlconn);
        return false;
    }
}

bool DbManager::getProductionMessageByPid(RapidJson &json, unsigned int pid)
{
    auto& mysqlpool = Singletion<ConnectionPool>::instance();
    auto mysqlconn = mysqlpool.getConnection();
    std::unique_ptr<sql::PreparedStatement> prep_stmt(mysqlconn->prepareStatement("select nickname, image, uid, message, time from leftmessage where pid=? limit 10"));

    prep_stmt->setInt(1, pid);
    bool flag = false;
    try {

        std::unique_ptr<sql::ResultSet> res(prep_stmt->executeQuery());

        if(res->next())
        {
            json.setObject("status", 1);
            do{
                json.setObjectToArray("pid", static_cast<int>(pid));
                json.setObjectToArray("uid", res->getInt("uid"));
                json.setObjectToArray("nickname", res->getString("nickname").c_str());
                json.setObjectToArray("message", res->getString("message").c_str());
                json.setObjectToArray("image", res->getString("image").c_str());
                json.setObjectToArray("time", res->getString("time").c_str());
                json.flushToArray();
            }while(res->next());
            json.flushToRoot("data");
            flag = true;
        }else
        {
            flag = false;
        }
        mysqlpool.close(mysqlconn);
        return flag;
    }catch (sql::SQLException& e)
    {
        LOG_ERROR << "DbManager::getProductionMessageByPid catch exception " << e.what();
        mysqlpool.close(mysqlconn);
        return false;
    }
}

bool DbManager::addProductionMessageByPid(unsigned int uid, unsigned int pid, const std::string &message)
{
    auto& mysqlpool = Singletion<ConnectionPool>::instance();
    auto mysqlconn = mysqlpool.getConnection();
    std::unique_ptr<sql::PreparedStatement> prep_stmt(mysqlconn->prepareStatement("insert into t_leftmessage(uid, pid, message) values(?, ?, ?)"));

    prep_stmt->setInt(1, uid);
    prep_stmt->setInt(2, pid);
    prep_stmt->setString(3, message);
    bool flag = false;
    int ret = -1;
    try {

        ret = prep_stmt->executeUpdate();
        if(ret == 1)
            flag = true;
        else
            flag = false;
        mysqlpool.close(mysqlconn);
        return flag;
    }catch (sql::SQLException& e)
    {
        LOG_ERROR << "DbManager::addProductionMessageByPid catch exception " << e.what();
        mysqlpool.close(mysqlconn);
        return false;
    }
}

bool DbManager::checkProductionUser(unsigned pid, unsigned uid)
{
    auto& mysqlpool = Singletion<ConnectionPool>::instance();
    auto mysqlconn = mysqlpool.getConnection();
    std::unique_ptr<sql::PreparedStatement> prep_stmt(mysqlconn->prepareStatement("SELECT p_id FROM t_productions where p_id = ? and p_uid = ?"));

    bool flag = false;
    prep_stmt->setInt(1, pid);
    prep_stmt->setInt(2, uid);

    try {
        std::unique_ptr<sql::ResultSet> res(prep_stmt->executeQuery());
        if(res->next())
            flag = true;
        else
            flag = false;
        mysqlpool.close(mysqlconn);
        return flag;
    }catch (sql::SQLException& e)
    {
        LOG_ERROR << "DbManager::checkProductionUser catch exception " << e.what();
        mysqlpool.close(mysqlconn);
        return false;
    }
}

bool DbManager::getSubscribeProductionList(RapidJson &json, unsigned int uid, bool total)
{
    auto& mysqlpool = Singletion<ConnectionPool>::instance();
    auto mysqlconn = mysqlpool.getConnection();
    std::unique_ptr<sql::PreparedStatement> prep_stmt;
    if(total)
    {
        prep_stmt.reset(mysqlconn->prepareStatement("SELECT p_name, pid, p_imageaddr\n"
                                                    "FROM v_subscribe_production\n"
                                                    "where uid=?\n"
                                                    "limit 10"));
    }else
    {
        prep_stmt.reset(mysqlconn->prepareStatement("SELECT pid FROM v_subscribe_production where uid=? limit 10"));
    }
    bool flag = false;
    prep_stmt->setInt(1, uid);
    try {
        std::unique_ptr<sql::ResultSet> res(prep_stmt->executeQuery());
        if(total)
        {
            if(res->next())
            {
                json.setObject("status", 1);
                do{
                    json.setObjectToArray("sp_name", res->getString("p_name").c_str());
                    json.setObjectToArray("sp_pid", res->getInt("pid"));
                    json.setObjectToArray("sp_images", res->getString("p_imageaddr").c_str());
                    json.flushToArray();
                }while (res->next());
                json.flushToRoot("sp_data");
                flag = true;
            }else
            {
                flag = true;
            }
        }else
        {
            if(res->next())
            {
                //json.setObject("status", 1);
                do{
                    json.setObjectToArray("sp_pids", res->getString("pid").c_str());
                    json.flushToArray();
                }while(res->next());
                json.flushToRoot("sp_data");
                flag = true;
            }else
            {
                flag = true;
            }
        }
        mysqlpool.close(mysqlconn);
        return flag;
    }catch (sql::SQLException& e)
    {
        LOG_ERROR << "DbManager::getSubscribeProductionList catch exception " << e.what();
        mysqlpool.close(mysqlconn);
        return false;
    }
}

bool DbManager::addSubscribeProductionList(unsigned int uid, unsigned int pid)
{
    auto& mysqlpool = Singletion<ConnectionPool>::instance();
    auto mysqlconn = mysqlpool.getConnection();
    int ret = -1;
    std::unique_ptr<sql::PreparedStatement> prep_stmt(mysqlconn->prepareStatement("insert into t_subscribe_production(uid, pid) values(?, ?)"));
    bool flag = false;
    prep_stmt->setInt(1, uid);
    prep_stmt->setInt(2, pid);

    try {
        ret = prep_stmt->executeUpdate();
        if(ret == 1)
            flag = true;
        else
            flag = false;
        mysqlpool.close(mysqlconn);
        return flag;
    }catch (sql::SQLException& e)
    {
        LOG_ERROR << "DbManager::addSubscribeProductionList catch exception " << e.what();
        mysqlpool.close(mysqlconn);
        return false;
    }
}

bool DbManager::getSubscribeUserList(RapidJson &json, unsigned int uid, bool total)
{
    auto& mysqlpool = Singletion<ConnectionPool>::instance();
    auto mysqlconn = mysqlpool.getConnection();
    std::unique_ptr<sql::PreparedStatement> prep_stmt;
    if(total)
    {
        prep_stmt.reset(mysqlconn->prepareStatement("select suid, nickname, image\n"
                                                    "from v_subscribe_user\n"
                                                    "where uid = ?\n"
                                                    "limit 10"));
    }else
    {
        prep_stmt.reset(mysqlconn->prepareStatement("select suid from v_subscribe_user where uid = ?"));
    }
    bool flag = false;
    prep_stmt->setInt(1, uid);
    try {
        std::unique_ptr<sql::ResultSet> res(prep_stmt->executeQuery());
        if(total)
        {
            if(res->next())
            {
                json.setObject("status", 1);
                do{
                    json.setObjectToArray("su_suid", res->getInt("suid"));
                    json.setObjectToArray("su_nickname", res->getString("nickname").c_str());
                    json.setObjectToArray("su_image", res->getString("image").c_str());
                    json.flushToArray();
                }while(res->next());
                json.flushToRoot("su_data");
                flag = true;
            }else
            {
                flag = true;
            }
        }else
        {
            if(res->next())
            {
                //json.setObject("status", 1);
                do
                {
                    json.setObjectToArray("su_uid", res->getInt("suid"));
                    json.flushToArray();
                }while(res->next());
                json.flushToRoot("su_data");
                flag = true;
            }else
            {
                flag = true;
            }
        }
        mysqlpool.close(mysqlconn);
        return flag;
    }catch (sql::SQLException& e)
    {
        LOG_ERROR << "DbManager::getSubscribeUserList catch exception " << e.what();
        mysqlpool.close(mysqlconn);
        return false;
    }
}

bool DbManager::addSubscribeUserList(unsigned int uid, unsigned int suid)
{
    auto& mysqlpool = Singletion<ConnectionPool>::instance();
    auto mysqlconn = mysqlpool.getConnection();
    int ret = -1;
    std::unique_ptr<sql::PreparedStatement> prep_stmt(mysqlconn->prepareStatement("insert into t_subscribe_user(uid, suid) values(?, ?)"));
    bool flag = false;
    prep_stmt->setInt(1, uid);
    prep_stmt->setInt(2, suid);

    try {
        ret = prep_stmt->executeUpdate();
        if(ret == 1)
            flag = true;
        else
            flag = false;
        mysqlpool.close(mysqlconn);
        return flag;
    }catch (sql::SQLException& e)
    {
        LOG_ERROR << "DbManager::addSubscribeUserList catch exception " << e.what();
        mysqlpool.close(mysqlconn);
        return false;
    }
}

bool DbManager::doOrder(unsigned int uid, unsigned int pid, std::string &orderlist)
{
    auto& mysqlpool = Singletion<ConnectionPool>::instance();
    auto mysqlconn = mysqlpool.getConnection();
    mysqlconn->setAutoCommit(false);
    std::unique_ptr<sql::PreparedStatement> prep_stmt(mysqlconn->prepareStatement("update t_productions set p_avaliable = 0 where p_id=? and p_avaliable = 1"));

    prep_stmt->setInt(1, pid);
    try {
        std::string p_name;
        int suid;
        double price;
        //std::unique_ptr<sql::ResultSet> res(prep_stmt->executeQuery());
        int ret = prep_stmt->executeUpdate();
        if(ret == 1)
        {
            //锁单
            //std::unique_ptr<sql::PreparedStatement> prep_stmt(mysqlconn->prepareStatement("update t_productions set p_avaliable = 0 where p_id = ?"));
            //prep_stmt->setInt(1, pid);
            //prep_stmt->executeUpdate();
            prep_stmt.reset(mysqlconn->prepareStatement("select p_name, p_uid, p_price from t_productions where p_id=?"));
            prep_stmt->setInt(1, pid);
            std::unique_ptr<sql::ResultSet> res(prep_stmt->executeQuery());
            if(res->next())
            {
                p_name = res->getString("p_name");
                suid = res->getInt("p_uid");
                price = res->getDouble("p_price");
            }else
            {
                LOG_ERROR << "DbManager::doOrder failed can't get info";
                mysqlconn->rollback();
                mysqlpool.close(mysqlconn);
                return false;
            }
        }else//无此商品或被购买了
        {
            LOG_ERROR << "DbManager::doOrder failed no such production " << pid;
            mysqlconn->rollback();
            mysqlpool.close(mysqlconn);
            return false;
        }
        orderlist = std::move(getTime());
        prep_stmt.reset(mysqlconn->prepareStatement("insert into t_orderlist(out_trade_no, list_pid, list_buid, list_suid, list_address_1, list_address_2, subject, total_amount)\n"
                                                    "values(?, ?, ?, ?, ?, ?, ?, ?)"));

        prep_stmt->setString(1, orderlist);
        prep_stmt->setInt(2, pid);
        prep_stmt->setInt(3, uid);
        prep_stmt->setInt(4, suid);
        prep_stmt->setInt(5, 110101);
        prep_stmt->setString(6, "丁瑞家");
        prep_stmt->setString(7, p_name);
        prep_stmt->setDouble(8, price);
        bool flag = false;
        ret = -1;
        ret = prep_stmt->executeUpdate();
        if(ret == 1)
            flag = true;
        else
            flag = false;
        mysqlconn->commit();
        mysqlpool.close(mysqlconn);
        return flag;
    }catch (sql::SQLException& e)
    {
        LOG_ERROR << "DbManager::doOrder catch exception " << e.what();
        mysqlconn->rollback();
        mysqlpool.close(mysqlconn);
        return false;
    }
}

bool DbManager::releaseOrder(const std::string &orderlist, unsigned int pid)
{
    auto& mysqlpool = Singletion<ConnectionPool>::instance();
    auto mysqlconn = mysqlpool.getConnection();
    mysqlconn->setAutoCommit(false);
    std::unique_ptr<sql::PreparedStatement> prep_stmt(mysqlconn->prepareStatement("update t_orderlist set list_status = 4 where out_trade_no=?"));
    bool flag = false;
    prep_stmt->setString(1, orderlist);

    try {
        int ret = prep_stmt->executeUpdate();
        if(ret != 1)
        {
            LOG_ERROR << "DbManager::releaseOrder set status failed " << orderlist;
            flag = false;
        }
        prep_stmt.reset(mysqlconn->prepareStatement("update t_productions set p_avaliable = 1 where p_id =?"));
        prep_stmt->setInt(1, pid);
        ret = prep_stmt->executeUpdate();
        if(ret != 1)
        {
            LOG_ERROR << "DbManager::releaseOrder set production avaliable failed " << pid;
            flag = false;
        }
        mysqlconn->commit();
        mysqlpool.close(mysqlconn);
        return flag;
    }catch (sql::SQLException& e)
    {
        LOG_ERROR << "DbManager::releaseOrder catch a exception " << e.what();
        mysqlconn->rollback();
        mysqlpool.close(mysqlconn);
        return false;
    }
}

bool DbManager::getHistoryOrderList(RapidJson &json, unsigned int uid)
{
    auto& mysqlpool = Singletion<ConnectionPool>::instance();
    auto mysqlconn = mysqlpool.getConnection();
    std::unique_ptr<sql::PreparedStatement> prep_stmt(mysqlconn->prepareStatement("SELECT out_trade_no, list_createtime, subject, total_amount\n"
                                                                                  "from t_orderlist\n"
                                                                                  "where list_status = 1 and list_buid = ?"));

    bool flag = false;
    prep_stmt->setInt(1, uid);

    try {
        std::unique_ptr<sql::ResultSet> res(prep_stmt->executeQuery());
        if(res->next())
        {
            json.setObject("status", 1);
            do{
                json.setObjectToArray("orderlist", res->getString("out_trade_no").c_str());
                json.setObjectToArray("time", res->getString("list_createtime").c_str());
                json.setObjectToArray("name", res->getString("subject").c_str());
                json.setObjectToArray("price", static_cast<double>(res->getDouble("total_amount")));
                json.flushToArray();
            }while(res->next());
            json.flushToRoot("data");
            flag = true;
        }else
            flag = true;
        mysqlpool.close(mysqlconn);
        return flag;
    }catch (sql::SQLException& e)
    {
        LOG_ERROR << "DbManager::getHistoryOrderList catch a exception " << e.what();
        mysqlpool.close(mysqlconn);
        return false;
    }
}

bool DbManager::getUnfinishOrderList(RapidJson &json, unsigned int uid)
{
    auto& mysqlpool = Singletion<ConnectionPool>::instance();
    auto mysqlconn = mysqlpool.getConnection();
    std::unique_ptr<sql::PreparedStatement> prep_stmt(mysqlconn->prepareStatement("select out_trade_no, list_createtime, subject, total_amount, list_status\n"
                                                                                  "from t_orderlist\n"
                                                                                  "where list_buid = ? and list_status != 1"));

    bool flag = false;
    prep_stmt->setInt(1, uid);

    try {
        std::unique_ptr<sql::ResultSet> res(prep_stmt->executeQuery());
        if(res->next())
        {
            json.setObject("status", 1);
            do{
                json.setObjectToArray("orderlist", res->getString("out_trade_no").c_str());
                json.setObjectToArray("time", res->getString("list_createtime").c_str());
                json.setObjectToArray("name", res->getString("subject").c_str());
                json.setObjectToArray("price", static_cast<double>(res->getDouble("total_amount")));
                json.setObjectToArray("status", res->getInt("list_status"));
                json.flushToArray();
            }while(res->next());
            json.flushToRoot("data");
            flag = true;
        }else
            flag = true;
        mysqlpool.close(mysqlconn);
        return flag;
    }catch (sql::SQLException& e)
    {
        LOG_ERROR << "DbManager::getHistoryOrderList catch a exception " << e.what();
        mysqlpool.close(mysqlconn);
        return false;
    }
}

bool DbManager::releaseWithoutGuarantee(unsigned uid, unsigned cid, const std::string &name,
                                        const std::string &description, const std::string &images,
                                        const std::string &addr, double price) {
    auto& mysqlpool = Singletion<ConnectionPool>::instance();
    auto mysqlconn = mysqlpool.getConnection();
    //mysqlconn->setAutoCommit(false);

    std::unique_ptr<sql::PreparedStatement> prep_stmt(mysqlconn->prepareStatement("insert into t_productions(p_name, p_cid, p_uid, p_description, p_price, p_address, p_imageaddr)\n"
                                                                                  "values(?, ?, ?, ?, ?, ?, ?)"));
    bool flag = false;
    prep_stmt->setString(1, name);
    prep_stmt->setInt(2, cid);
    prep_stmt->setInt(3, uid);
    prep_stmt->setString(4, description);
    prep_stmt->setDouble(5, price);
    prep_stmt->setString(6, addr);
    prep_stmt->setString(7, images);

    try {
        int ret = prep_stmt->executeUpdate();
        if(ret == 1)
            flag = true;
        else
        {
            flag = false;
            LOG_ERROR << "DbManager::releaseWithoutGuarantee insert info into db failed";
            //mysqlconn->rollback();
            //mysqlpool.close(mysqlconn);
            //return flag;
        }
        //mysqlconn->commit();
        mysqlpool.close(mysqlconn);
        return flag;
    }catch (sql::SQLException& e)
    {
        LOG_ERROR << "DbManager::releaseWithoutGuarantee catch a exception " << e.what();
        //mysqlconn->rollback();
        mysqlpool.close(mysqlconn);
        return false;
    }
}

bool DbManager::releasewithGuarantee(unsigned uid, unsigned cid, const std::string &name,
                                     const std::string &description, const std::string &images, const std::string &addr,
                                     double price) {
    auto& mysqlpool = Singletion<ConnectionPool>::instance();
    auto mysqlconn = mysqlpool.getConnection();

    std::unique_ptr<sql::PreparedStatement> prep_stmt(mysqlconn->prepareStatement("insert into t_listproduction(name, description, cid, uid, origin_price, images, address)\n"
                                                      "values(?, ?, ?, ?, ?, ?, ?)"));

    bool flag = false;
    prep_stmt->setString(1, name);
    prep_stmt->setString(2, description);
    prep_stmt->setInt(3, cid);
    prep_stmt->setInt(4, uid);
    prep_stmt->setDouble(5, price);
    prep_stmt->setString(6, images);
    prep_stmt->setString(7, addr);

    try {
        int ret = prep_stmt->executeUpdate();
        if(ret == 1)
            flag = true;
        else
        {
            flag = false;
            LOG_ERROR << "DbManager::releaseWithGuarantee insert failed";
        }
        mysqlpool.close(mysqlconn);
        return flag;
    }catch (sql::SQLException& e)
    {
        LOG_ERROR << "DbManager::releaseWithGuarantee catch a exception " << e.what();
        mysqlpool.close(mysqlconn);
        return false;
    }
}