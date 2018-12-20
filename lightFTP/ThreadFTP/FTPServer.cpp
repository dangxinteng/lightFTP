#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
#include<regex>
#include<thread>
#include<fstream>
#include"FTPServer.h"
#include"Protocal.h"
#include"Exception.h"

FTPServer FTPServer::m_serv;
std::atomic_bool FTPServer::m_bRun(false);

void FTPServer::Load(const char * userFile)
{
	std::ifstream file(userFile);
	if (!file.is_open())
		throw Error("open userfile error.");
	char line[128];
	std::string uname;
	UserInfo uinfo = {};
	while (!file.eof())
	{
        file.getline(line, sizeof(line), '\n');
		if (strlen(line) == 0)
			continue;
		uname = strtok(line, " ");
        uinfo.pass = strtok(NULL, " ");
        uinfo.root = strtok(NULL, " ");
		if (uname != "" && uinfo.pass != "" && uinfo.root != "")
			m_User.insert({ uname, uinfo });
	}
	
	file.close();
}

void FTPServer::Run()
{
	if (!m_lsock.Listen())
		throw Error("server socket listen error??" + std::to_string(m_lsock.GetLastError()));
	
	while (m_bRun)
	{
		Protocal *protoc = new Protocal;
		if (!m_lsock.Accept(protoc->GetSocket()))
		{
			delete protoc;
			if(!m_bRun)
				break;
			throw Error(std::string("server socket accept error??") + std::to_string(m_lsock.GetLastError()));
		}
		protoc->Do();
	}
}

int FTPServer::Menu()
{
	std::string str;
	std::regex e("[0-9]+");
	std::cin >> str;
	return std::regex_match(str, e) ? 
		std::stoi(str) : 1;
}

int FTPServer::Main()
{
    if (!m_lsock.Create(2111))
        throw Error(std::string("server socket create error:") + std::to_string(m_lsock.GetLastError()));
	Load();
	std::thread t(ListenProc);
	t.detach();

	while (Menu())
		;

	m_bRun = false;
	m_lsock.Close();

	return 0;
}


	



