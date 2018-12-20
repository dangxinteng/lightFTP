#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif
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
void FTPServer::Select(std::map<SOCKET, Protocal*>::const_iterator && it)
{
    int i = FD_SETSIZE;
    auto beginIt = it;
    fd_set fs;
    FD_ZERO(&fs);
    SOCKET max = 0;
    while (it != m_socks.end() && i--)
    {
        FD_SET(it->first, &fs);
        if (max < it->first)
            max = it->first;
        ++it;
    }
    auto endIt = it;
    Select(fs,max,beginIt,endIt);
}

void FTPServer::Select(fd_set & fs,SOCKET max,
    std::map<SOCKET,Protocal*>::const_iterator beginIt,
    std::map<SOCKET, Protocal*>::const_iterator endIt)
{
    char sIP[32];
    UINT nPort;

    struct timeval timeout = { 0,1000 };
    int n = select(max + 1, &fs, NULL, NULL, &timeout);
    if(n == -1)
        throw Error("select error.");
    if(n == 0)
        return;
    while (beginIt != endIt)
    {

        if (!FD_ISSET(beginIt->first, &fs))
        {
            beginIt++;
            continue;
        }
        if(beginIt->first == m_lsock)
        {

            Protocal * pProto = new Protocal;
            if (m_lsock.Accept(pProto->GetSocket(), sIP, &nPort))
            {
                std::cout << "Come from:" << sIP << "-" << nPort << std::endl;
                pProto->Welcome();
                m_socks[pProto->GetSocket()] = pProto;
            }
            else
                std::cout << "Accept Error:" << m_lsock.GetLastError() << std::endl;
        }
        else
        {
            Protocal* p = m_socks[beginIt->first];
            if (p)
                if (!p->OnReceive())
                {
                    m_socks.erase(beginIt->first);
                    delete p;
                }
        }
        beginIt++;
    }
}
void FTPServer::Run()
{
	if (!m_lsock.Listen())
        throw Error("server socket listen error:" + std::to_string(m_lsock.GetLastError()));
    m_socks[m_lsock] = nullptr;
    while (m_bRun)
    {
        auto it = m_socks.begin();
        while (it != m_socks.end())
            Select(it);
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


	



