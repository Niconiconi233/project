#ifndef SERVER_DEF_H
#define SERVER_DEF_H

struct serv_str_t
{
	const char* str;
	size_t len;
	serv_str_t(const char* ptr, size_t len)
		:str(ptr),
		len(len)
	{}
};

#endif

