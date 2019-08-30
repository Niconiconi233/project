#ifndef FILESERV_CONFIGREADER_H
#define FILESERV_CONFIGREADER_H
/*
    简单的配置文件解析
 */

#include <map>
#include <string>



class ConfigReader
{
public:
    ConfigReader(const std::string FileName = "ServerConfig.conf");
    ~ConfigReader();

    ConfigReader(const ConfigReader&) = delete;
    ConfigReader& operator=(const ConfigReader&) = delete;

    int getAsInt(const std::string key);
    double getAsDouble(const std::string key);
    std::string getAsString(const std::string key);
    char getAsChar(const std::string key);
    bool getAsBool(const std::string key);

    void setValue(const std::string key, int newValue);
    void setValue(const std::string key, double newValue);
    void setValue(const std::string key, char ch);
    void setValue(const std::string key, const std::string newValue);

    void remove(const std::string key);

private: 
    enum State {ksuccess, kerror, kencoding};
    std::map<std::string, std::string> items_;
    State state_;
    std::string FileName_;
    bool hasChanged_;

    void setState(State st)
    {
        state_ = st;
    }
    void reader(const std::string& FileName);
    void parseLine(char* line);
    void saveFile();

};

#endif // !FILESERV_CONFIGREADER_H