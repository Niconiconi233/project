#ifndef FILESERV_TOOLS_H
#define FILESERV_TOOLS_H

#include <vector>
#include <string>
#include <cstring>

std::vector<std::string> splice(const std::string& data, const std::string fmt)
{
    size_t beginpos = 0;
	size_t endpos = 0;
    size_t len = strlen(fmt.c_str());
    std::vector<std::string> lists;
    while(endpos != std::string::npos)
    {
        endpos = data.find(fmt, beginpos);
		if(endpos == beginpos)
			break;
        lists.emplace_back(data, beginpos, endpos - beginpos);
        beginpos = endpos + len;
    }
    return lists;
}

#endif // !FILESERV_TOOLS_H