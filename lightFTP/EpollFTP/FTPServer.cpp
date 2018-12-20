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
#define EPEV_SIZE 512
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
void FTPServer::EpollAddfd(int epollfd, SOCKET sfd)
{
    epoll_event ev;
    ev.data.fd = sfd;
    ev.events = EPOLLIN;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,sfd,&ev);
}
void FTPServer::EpollDelfd(int epollfd, SOCKET sfd)
{
    epoll_event ev;
    ev.data.fd = sfd;
    ev.events = EPOLLIN;
    epoll_ctl(epollfd,EPOLL_CTL_DEL,sfd,&ev);
}
void FTPServer::Epoll(int epollfd)
{
    char sIP[32];
    UINT nPort;
    epoll_event epEvents[EPEV_SIZE];
    while (m_bRun)
    {
        int n = epoll_wait(epollfd,epEvents,EPEV_SIZE,1);
        if(n == -1)
            throw Error("select error.");
        if(n == 0)
            continue;
        int i = -1;
        while (++i < n)
        {
            SOCKET sfd = epEvents[i].data.fd;
            if(!(epEvents[i].events & EPOLLIN))
                continue;
            if(sfd == m_lsock)
            {

                Protocal * pProto = new Protocal;
                if (m_lsock.Accept(pProto->GetSocket(), sIP, &nPort))
                {
                    std::cout << "User " << m_socks.size() << //except listen socket
                                 " Come from:" << sIP << "-" << nPort << std::endl;
                    pProto->Welcome();
                    m_socks[pProto->GetSocket()] = pProto;
                    EpollAddfd(epollfd, pProto->GetSocket());
                }
                else
                    std::cout << "Accept Error:" << m_lsock.GetLastError() << std::endl;
            }
            else
            {
                Protocal* p = m_socks[sfd];
                if (p)
                    if (!p->OnReceive())
                    {
                        std::cout << "exit from:" << sfd << std::endl;
                        EpollDelfd(epollfd, sfd);
                        m_socks.erase(sfd);
                        delete p;
                    }
            }
        }
    }
    close(epollfd);
}
void FTPServer::Run()
{
	if (!m_lsock.Listen())
        throw Error("server socket listen error:" + std::to_string(m_lsock.GetLastError()));
    m_socks[m_lsock] = nullptr;
    int epollfd = epoll_create(EPEV_SIZE);
    EpollAddfd(epollfd, m_lsock);

    Epoll(epollfd);
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

    sleep(3);

	return 0;
}


	



