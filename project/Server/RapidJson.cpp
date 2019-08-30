//
// Created by soft01 on 2019/8/12.
//

#include "RapidJson.h"

using namespace rapidjson;

void RapidJson::setObject(const char* key, int value)
{
    document_.AddMember(Value().SetString(key, allocator_).Move(), value, allocator_);
}

//void RapidJson::setObject(const std::string &key, bool value)
//{
//    document_.AddMember(Value().SetString(key.c_str(), key.length(), allocator_).Move(), value, allocator_);
//}

void RapidJson::setObject(const char* key, double value)
{
    document_.AddMember(Value().SetString(key, allocator_).Move(), value, allocator_);
}

void RapidJson::setObject(const char* key, const char* value)
{
    document_.AddMember(Value().SetString(key, allocator_).Move(), Value().SetString(value, allocator_).Move(), allocator_);
}

void RapidJson::setObjectToArray(const char* key, int value)
{
    if(!handling)
    {
        objVector_.emplace_back(Value(kObjectType));
        handling = true;
    }
    objVector_.back().AddMember(Value().SetString(key, allocator_).Move(), value, allocator_);
}

//void RapidJson::setObjectToArray(const std::string &key, bool value)
//{
//    if(!handling)
//    {
//        objVector_.emplace_back(Value(kObjectType));
//        handling = true;
//    }
//    objVector_.back().AddMember(Value().SetString(key.c_str(), key.length(), allocator_).Move(), value, allocator_);
//
//}

void RapidJson::setObjectToArray(const char* key, double value)
{
    if(!handling)
    {
        objVector_.emplace_back(Value(kObjectType));
        handling = true;
    }
    objVector_.back().AddMember(Value().SetString(key, allocator_).Move(), value, allocator_);

}

void RapidJson::setObjectToArray(const char* key, const char* value)
{
    if(!handling)
    {
        objVector_.emplace_back(Value(kObjectType));
        handling = true;
    }
    objVector_.back().AddMember(Value().SetString(key, allocator_).Move(), Value().SetString(value, allocator_).Move(), allocator_);

}

void RapidJson::flushToArray()
{
    if(arrPtr_ == nullptr)
        arrPtr_ = new Value(kArrayType);
    arrPtr_->PushBack(objVector_.back(), allocator_);
    handling = false;
}

void RapidJson::flushToRoot(const std::string &key)
{
    if(arrPtr_ != nullptr)
    {
        document_.AddMember(StringRef(key.c_str(), key.length()), *arrPtr_, allocator_);
    }
}

std::string RapidJson::toString()
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<StringBuffer> writer(buffer);
    document_.Accept(writer);
    return buffer.GetString();
}

RapidJson::~RapidJson()
{
    if(arrPtr_)
        delete arrPtr_;
}
