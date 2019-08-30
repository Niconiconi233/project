//
// Created by soft01 on 2019/8/1.
//
#include "RequestParse.h"
#include "Tools.h"
#include "../Logging/base/Logging.h"
#include "Def.h"

bool RequestParse::parse(const std::string& str)
{
    //TODO 返回的是一个完整的header长度，以此来避免多个请求头被切割问题
    reset();
    str_ = str;
    size_t len = str_.find("\r\n\r\n");
    if(len == std::string::npos)
    {
        headerState_ =HeaderState::HEADER_NOTENOUGHT;
        return false;
    }
    //对于get请求 请求完全 post未知
    headerLen_ = len + 3;//先获取完整的请求头 对于post请求在重新调整
    return parseHeader();
}

/*
 * HEADER_ERROR标示请求头请求有问题
 * HEADER_BAD标示请求未知或暂不支持
 */
bool RequestParse::parseHeader()
{
	headerState_ = HeaderState::HEADER_INIT;
	std::vector<std::string> lists = splice(str_, "\r\n");//分行
	size_t pos = lists[0].find_first_of(" ");
	serv_str_t method(lists[0].c_str(), pos);
    if(strncmp(method.str, "get", method.len) == 0)
    {
        method_ = HTTP_GET;
    }
    else if (strncmp(method.str, "post", method.len) == 0)
    {
        method_ = HTTP_POST;
    }
    else if (strncmp(method.str, "options", method.len) == 0)
    {
        method_ = HTTP_OPTIONS;
    }else
    {
        headerState_ = HEADER_BAD;
        return false;
		//FIXME ADD OTHER
    }
	pos = lists[0].find_last_of(" ");
	serv_str_t version = { lists[0].c_str() + pos + 1, lists[0].length() - pos - 1};
    if(strncmp(version.str, "http/1.1", version.len) == 0)
    {
        httpVersion_ = HTTP_11;
    }
    else if (strncmp(version.str, "http/1.0", version.len) == 0)
    {
        httpVersion_ = HTTP_10;
    }
    else
    {
        headerState_ = HEADER_BAD;
        return false;
    }
	for (size_t i = 1; i < lists.size(); ++i)
	{
		size_t pos = lists[i].find(":");
		if (pos == std::string::npos)
			continue;
		headerList_.insert(std::make_pair(std::string(lists[i], 0, pos), std::string(lists[i], pos + 2, lists[i].length() - (pos + 2))));
		//忽略空格
	}
	serv_str_t request = { lists[0].c_str() + method.len + 1, pos - method.len - 1 };
	if(method_ == HTTP_GET)
    {
	    return parseGetQuery(request);
    }else if(method_ == HTTP_POST)
    {
	    size_t pos = str_.find("\r\n\r\n") + 4;
	    if(pos == str_.length())
        {
	        headerState_ = HeaderState::HEADER_NOTENOUGHT;
	        return false;
        }
		serv_str_t postQuery(str_.c_str() + pos, str_.length() - pos);
	    return parsePostQuery(request, postQuery);
    }else if(method_ == HTTP_OPTIONS)
    {
	    headerState_ = HeaderState ::HEADER_OK;
	    return true;
    }
	return false;
}

bool RequestParse::parseGetQuery(serv_str_t& str)
{
	std::string query(str.str, str.len);
    size_t pos = query.find("?");
    if(pos == std::string::npos)
    {
        headerState_ = HeaderState::HEADER_ERROR;
        LOG_ERROR << "RequestParse::parseQuery unkown query";
        return false;
    }

    serv_str_t queryType(query.c_str() + 1, pos - 1);

    if(strncmp(queryType.str, "login", queryType.len) == 0)
    {
        queryType_ = QueryType::TypeLogin;
        query.erase(0, pos + 1);

    }
    else if(strncmp(queryType.str, "reg", queryType.len) == 0)
    {
        queryType_ = QueryType ::TypeReg;
        query.erase(0, pos + 1);
    }
    else if(strncmp(queryType.str, "release", queryType.len) == 0)
    {
        queryType_ = QueryType::TypeRelease;
        query.erase(0, pos + 1);
    }
    else if(strncmp(queryType.str, "search", queryType.len) == 0)
    {
        queryType_ = QueryType::TypeSearch;
        query.erase(0, pos + 1);
    }
    else if(strncmp(queryType.str, "random", queryType.len) == 0)
    {
        queryType_ = QueryType::TypeRandom;
        query.erase(0, pos + 1);
    }else if(strncmp(queryType.str, "mail", queryType.len) == 0)
    {
        queryType_ = QueryType::TypeMail;
        query.erase(0, pos + 1);
    }else if(strncmp(queryType.str, "guss", queryType.len) == 0)
    {
        queryType_ = QueryType::TypeGuss;
        query.erase(0, pos + 1);
    }else
    {
        headerState_ = HeaderState::HEADER_BAD;
        LOG_ERROR << "unkown query type "<<queryType.str;
        return false;
    }
    auto kvList = splice(query, "&");//FIXME 考虑不使用
    for(auto& i : kvList)
    {
        size_t pos = i.find("=");
        if (pos == std::string::npos)
            continue;
        queryList_.insert(std::make_pair(std::string(i, 0, pos), std::string(i, pos + 1, i.length() - (pos + 1))));
    }
    headerState_ = HeaderState::HEADER_OK;
    return true;
}

bool RequestParse::parsePostQuery(serv_str_t& str, serv_str_t &kw)
{
	//FIXME 考虑不使用string
    serv_str_t queryType(str.str + 1, str.len - 1);

    //TODO 那些操作需要post
    //暂时不管
    std::string kwd(kw.str, kw.len);
    queryType_ = QueryType::TypeReg;
    auto kvList = splice(kwd, "&");
    for(auto& i : kvList)
    {
        size_t pos = i.find("=");
        if (pos == std::string::npos)
            continue;
        queryList_.insert(std::make_pair(std::string(i, 0, pos), std::string(i, pos + 1, i.length() - (pos + 1))));
    }
    headerState_ = HeaderState::HEADER_OK;
    return true;
}

std::string RequestParse::unencode(const char *src) const
{
    int code;
    char buf[1024] = {0};
    char* dest = buf;
    for(; *src != 0; src++, dest++) {
        if (*src == '%') {
            if (sscanf(src + 1, "%2x", &code) != 1)
                LOG_ERROR << "failed in unencode";
            *dest = code;
            src += 2;
        } else
        {
            *dest = *src;
        }
    }
    return buf;
}

void RequestParse::reset()
{
    queryList_.clear();
    headerList_.clear();
    method_ = HTTP_INIT;
    headerState_ = HEADER_INIT;
    queryType_ = TypeInit;
    httpVersion_ = HTTP_00;
}
