#include "ConfigReader.h"



#include <fstream>
#include <cassert>
#include <cstring>//strchar
#include <cstdio>//snprintf
#include <exception>//for error

ConfigReader::ConfigReader(const std::string filename)
    : state_(kencoding),
    FileName_(filename),
    hasChanged_(false)
{
    reader(FileName_);
}

ConfigReader::~ConfigReader()
{
    if(hasChanged_)
        saveFile();
}

void ConfigReader::reader(const std::string& FileName)
{
    std::ifstream input;
    input.open(FileName_, std::ios::in);
    if(!input.is_open())
    {
        setState(kerror);
        throw "ConfigReader failed can't open file";

    }
    while(!input.eof()){
        char line[1024] = {0};
        input.getline(line, sizeof line);
        parseLine(line);
    }
    input.close();
    setState(ksuccess);
}

void skipSpace(char* ptr)
{
    while(*ptr == ' ')
        ++ptr;
}

int charToInt(const std::string& lhs)
{
    int result = 0;
    for(auto it = lhs.begin(); it != lhs.end(); ++it)
    {
        result = result * 10 + (*it - '0');
    }
    return result;
}

double charToDouble(const std::string& lhs)
{
    double result1 = 0.0;
    double result2 = 0.0;
    bool flag = false;
    double i = 10;
    for(auto it = lhs.begin(); it != lhs.end(); ++it)
    {
        if(*it == '.'){
            flag = true;
            continue;
        }
        if(!flag)
        {
            result1 = result1 * 10 + (*it - '0');
        }else
        {
            result2 += (*it - '0') / i;
            i *= 10;
        }
    }
    return result1 + result2;
}

void ConfigReader::parseLine(char* line)
{
    assert(line != nullptr);
    char* p = line;
    skipSpace(line);
    if(*p == '#')
        return;
    else
    {
        char* ptr = strchr(p, '=');
        if(ptr == nullptr)
            return;
        *ptr = '\0';
        ++ptr;
        std::string key(p);
        std::string value(ptr);
        items_[key] = value;
    }
}

int ConfigReader::getAsInt(const std::string key)
{
    assert(state_ == ksuccess);
    std::string tmp;
    auto it = items_.find(key);
    if(it != items_.end())
    {
        tmp = it->second;
        return charToInt(tmp);
    }else
    {
        throw std::invalid_argument("no such key");
    }

}

double ConfigReader::getAsDouble(const std::string key)
{
    assert(state_ == ksuccess);
    std::string tmp;
    auto it = items_.find(key);
    if(it != items_.end())
    {
        tmp = it->second;
        return charToDouble(tmp);
    } else
    {
        throw std::invalid_argument("no such key");
    }

}

std::string ConfigReader::getAsString(const std::string key)
{
    assert(state_ == ksuccess);
    std::string tmp;
    auto it = items_.find(key);
    if(it != items_.end())
    {
        tmp = it->second;
        return tmp;
    }else
    {
        throw std::invalid_argument("no such key");
    }

}

char ConfigReader::getAsChar(const std::string key)
{
    assert(state_ == ksuccess);
    std::string tmp;
    auto it = items_.find(key);
    if(it != items_.end())
    {
        tmp = it->second;
        return *tmp.begin();
    }else
    {
        throw std::invalid_argument("no such key");
    }

}

bool ConfigReader::getAsBool(const std::string key)
{
    assert(state_ == ksuccess);
    std::string tmp;
    auto it = items_.find(key);
    if(it != items_.end())
    {
        tmp = it->second;
        if(tmp.compare("true") == 0)
            return true;
        else
            return false;
    }else
    {
        throw std::invalid_argument("no such key");
    }

}

void ConfigReader::setValue(const std::string key, int newValue)
{
    assert(state_ == ksuccess);
    char buf[1024] = {0};
    snprintf(buf, sizeof buf, "%d", newValue);
    std::string str(buf);
    items_[key] = str;
    hasChanged_ = true;
}

void ConfigReader::setValue(const std::string key, double newValue)
{
    assert(state_ == ksuccess);
    char buf[1024] = {0};
    snprintf(buf, sizeof buf, "%lf", newValue);
    std::string str(buf);
    items_[key] = str;
    hasChanged_ = true;
}

void ConfigReader::setValue(const std::string key, char ch)
{
    assert(state_ == ksuccess);
    char buf[1024] = {0};
    snprintf(buf, sizeof buf, "%c", ch);
    std::string str(buf);
    items_[key] = str;
    hasChanged_ = true;
}

void ConfigReader::setValue(const std::string key, const std::string newValue)
{
    assert(state_ == ksuccess);
    items_[key] = newValue;
    hasChanged_ = true;
}



void ConfigReader::saveFile()
{
    std::ofstream out;
    out.open(FileName_, std::ios::out);
    assert(out.is_open());
    for(auto it = items_.begin(); it != items_.end(); ++it)
    {
        char buf[1024] = {0};
        snprintf(buf, sizeof buf, "%s=%s\n", it->first.c_str(), it->second.c_str());
        out.write(buf, strlen(buf));
    }
    out.close();
}

void ConfigReader::remove(const std::string key)
{
    auto it = items_.find(key);
    if(it != items_.end())
    {
        items_.erase(it);
        hasChanged_ = true;
    }

}