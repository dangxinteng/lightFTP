#pragma once
#include<exception>
#include<string>
struct Error : std::exception
{
	std::string errMsg;
	Error(const std::string &errMsg)
		:errMsg(errMsg){}
	virtual const char* what() const noexcept
	{
		return errMsg.c_str();
	}
};
