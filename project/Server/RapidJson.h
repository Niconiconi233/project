//
// Created by soft01 on 2019/8/12.
//

#ifndef SERVER_RAPIDJSON_H
#define SERVER_RAPIDJSON_H

#include <vector>

#include "../rapidjson/rapidjson.h"
#include "../rapidjson/document.h"
#include "../rapidjson/writer.h"
#include "../rapidjson/stringbuffer.h"

class RapidJson{
public:
    RapidJson()
        :document_(),
        allocator_(document_.GetAllocator()),
        handling(false)
    {
        document_.SetObject();
    }

    RapidJson(const RapidJson&) = delete;

    RapidJson& operator=(const RapidJson&) = delete;

    ~RapidJson();

    void setObject(const char* key, const char* value);

    void setObject(const char* key, int value);

    void setObject(const char* key, double value);

    //void setObject(const std::string& key, bool value);

    void setObjectToArray(const char* key, const char* value);

    void setObjectToArray(const char* key, int value);

    void setObjectToArray(const char* key, double value);

    //void setObjectToArray(const std::string& key, bool value);

    void flushToArray();

    void flushToRoot(const std::string& key);

    std::string toString();

    //void clear();

private:

    rapidjson::Document document_;
    rapidjson::Document::AllocatorType& allocator_;
    rapidjson::Value* arrPtr_{nullptr};
    std::vector<rapidjson::Value> objVector_;
    bool handling;

};


#endif //SERVER_RAPIDJSON_H
