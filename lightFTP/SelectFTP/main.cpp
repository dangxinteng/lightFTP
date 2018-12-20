#include<iostream>
#include"FTPServer.h"

int main()
{
	try
	{
		return FTPServer::m_serv.Main();
	}
	catch (const std::exception &e)
	{
		std::cout << e.what() << std::endl;
		std::cin.get();
		return -1;
	}
}