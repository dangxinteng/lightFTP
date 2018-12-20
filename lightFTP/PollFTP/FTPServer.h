#pragma once
#include<memory>
#include<atomic>
#include<map>
#include<vector>
#include<poll.h>
#include"Socket.h"
#include"Exception.h"

class Protocal;
struct UserInfo
{
	std::string pass;
	std::string root;
};
class FTPServer
{
	Socket m_lsock;
	std::map<std::string, UserInfo> m_User;
    std::map<SOCKET, Protocal*> m_socks;
    std::vector<Protocal *> m_addSocks;
    std::vector<Protocal *> m_delSocks;
	static void ListenProc()
	{
		try
		{
			m_serv.Run();
		}
		catch (const std::exception &e)
		{
			std::cout << e.what() << std::endl;
			std::cin.get();
			return;
		}
		
	}
    void Poll(std::map<SOCKET, Protocal*>::iterator &it);
    void Poll(pollfd *pfds,int count);
	void Load(const char *userFile = "./user.db");
	void Run();
	int Menu();
public:
	static std::atomic_bool m_bRun;
	static FTPServer m_serv;
	int Main();
	std::string GetRootPath(const std::string &uName)const
	{
		return m_User.at(uName).root;
	}
	bool CheckUserPass(const std::string &uName, const std::string &uPass)const
	{
		return  m_User.at(uName).pass == uPass;
	}
	bool IsExistUser(const std::string &sName)const
	{
		return m_User.count(sName) > 0;
	}
	static const std::atomic_bool& IsRun()
	{
		return m_bRun;
	}
	FTPServer()
	{
		m_bRun = true;
	}
	~FTPServer() = default;
};

