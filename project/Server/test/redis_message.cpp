//
// Created by soft01 on 2019/9/5.
//
#include <mysql/mysql.h>
#include <string>
#include <hiredis/hiredis.h>

#include "../RapidJson.h"

//对留言进行缓存
extern "C" long long cacheMessage(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
    redisContext* conn = redisConnect("127.0.0.1", 6379);
    RapidJson json;
    json.setObject("suid", *(int*)args->args[0]);
    json.setObject("ruid", *(int*)args->args[1]);
    json.setObject("message", (char*)args->args[2]);
    json.setObject("time", (char*)args->args[3]);
    std::string json_body(json.toString());
    redisReply* reply = (redisReply*)redisCommand(conn, "select 3");
    freeReplyObject(reply);
    reply = (redisReply*)redisCommand(conn, "lpush %d %s", *(long long*)args->args[1], json_body.c_str());
    freeReplyObject(reply);
    reply = (redisReply*)redisCommand(conn, "EXPIRE %d %d", *(long long*)args->args[1], 86400);
    freeReplyObject(reply);
    redisFree(conn);
    return 1;
}

extern "C" my_bool cacheMessage_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    return 0;
}

extern "C" long long cacheUser(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
    redisContext* conn = redisConnect("127.0.0.1", 6379);
    RapidJson json;
    json.setObject("uid", *(int*)args->args[0]);
    json.setObject("nickname", std::string(args->args[1], args->lengths[1]).c_str());
    json.setObject("description", std::string(args->));
    json.setObject("sex", args->args[3]);
    json.setObject("image", args->args[4]);
    json.setObject("credit", *(int*)args->args[5]);
    json.setObject("create_time", args->args[6]);
    std::string json_body(json.toString());
    redisReply* reply = (redisReply*)redisCommand(conn, "select 2");
    freeReplyObject(reply);
    reply = (redisReply*)redisCommand(conn, "set %d %s EX 86400", *(long long*)args->args[0], json_body.c_str());
    freeReplyObject(reply);
    redisFree(conn);
    return 1;
}

extern "C" my_bool cacheUser_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    return 0;
}

extern "C" long long cacheProduction(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
    redisContext* conn = redisConnect("127.0.0.1", 6379);
    RapidJson json;
    json.setObject("pid", *(int*)args->args[0]);
    json.setObject("pname", args->args[1]);
    json.setObject("p_description", args->args[2]);
    json.setObject("pimages", args->args[3]);
    json.setObject("price", *(double*)args->args[4]);
    json.setObject("wcount", *(int*)args->args[5]);
    std::string json_body(json.toString());
    redisReply* reply = (redisReply*)redisCommand(conn, "select 1");
    freeReplyObject(reply);
    reply = (redisReply*)redisCommand(conn, "set %d %s EX 86400", *(long long*)args->args[0], json_body.c_str());
    freeReplyObject(reply);
    redisFree(conn);
    return 1;
}

extern "C" my_bool cacheProduction_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    return 0;
}