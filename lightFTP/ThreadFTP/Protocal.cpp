#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif
#include<iostream>
#include<memory>
#include<thread>
#include<atomic>
#include<boost/filesystem.hpp>
#include<boost/system/error_code.hpp>
#include"Protocal.h"
#include"FTPServer.h"
#include"Utility.h"

using namespace std;
using namespace boost;
std::atomic_int m_nWorkCount(0);

bool Protocal::Retr(const char * sData)
{
	string path = GetPath() + sData;
	const char* ps[] = { "r" , "rb" };// 
	FILE* pf = fopen(path.c_str(), ps[m_type]);
	if (!pf)
	{
		string str = "426 File open failed.\r\n";
		m_socka.Send(str.c_str(), str.length());
		return false;
	}
	string str = "150 File status okay.\r\n";
	m_socka.Send(str.c_str(), str.length());
	char s[2048];
	int n = 0;
	while ((n = fread(s, 1, sizeof(s), pf)) > 0)
		m_sockt.Send(s, n);
	fclose(pf);
	m_sockt.Close();
	m_sockp.Close();
	str = "226 Transfer finished successfully. Data connection closed.\r\n";
	m_socka.Send(str.c_str(), str.length());
	return true;
}

bool Protocal::Size(const char * sData)
{
	string path = GetPath() + sData,str;
	if (filesystem::is_regular_file(path))
		str = "213 " + to_string(filesystem::file_size(path)) + " Get file size success.\r\n";
	else
		str = "213 0 Get file size failed.\r\n";
	m_socka.Send(str.c_str(), str.length());
	return true;
}
bool Protocal::Stor(const char * sData)
{
	string path = GetPath() + sData;
	const char* ps[] = { "w" , "wb" };// \n \r\n
	FILE* pf = fopen(path.c_str(), ps[m_type]);
	if (!pf)
	{
		string str = "426 File open failed.\r\n";
		m_socka.Send(str.c_str(), str.length());
		return false;
	}
	string str = "150 File status okay.\r\n";
	m_socka.Send(str.c_str(), str.length());
	char s[2048];
	int n = 0;
	while ((n = m_sockt.Receive(s, sizeof(s)))>0)
		fwrite(s, n, 1, pf);
	fclose(pf);
	m_sockt.Close();
	m_sockp.Close();
	str = "226 Transfer finished successfully. Data connection closed.\r\n";
	m_socka.Send(str.c_str(), str.length());
	return true;
}
bool Protocal::Rnto(const char* sData)
{
	string oldName = GetPath() + m_sOldName;
	string newName = GetPath() + sData;
	string str;
	system::error_code ec;
	filesystem::rename(oldName, newName, ec);
	if (!ec)
		str = "250 file renamed from ";
	else
		str = "553 file rename failed:from";
	str += m_sOldName + "to " + str + ".\r\n";
	return m_socka.Send(str.c_str(), str.length()) > 0;
}
bool Protocal::Rnfr(const char* sData)
{
	m_sOldName = sData;
	string str = "350 Requested file new name.\r\n";
	m_socka.Send(str.c_str(), str.length());
	return true;
}
bool Protocal::Mkd(const char * sData)
{
	string str = GetPath() + sData;
	if (!filesystem::create_directory(str))
		str = "553 Create directory failed.\r\n";
	else
		str = "250 Create directory success.\r\n";
	m_socka.Send(str.c_str(), str.length());
	return true;
}
bool Protocal::Rmd(const char * sData)
{
	string str = GetPath() + sData;

	if (filesystem::remove_all(str)<0)
		str = "553 file remove directory failed.\r\n";
	else
		str = "250 file remove directory success.\r\n";
	return m_socka.Send(str.c_str(), str.length()) > 0;
}
bool Protocal::Dele(const char * sData)
{
	string str = GetPath() + sData;
	if (!filesystem::remove(str))
		str = "553 DELE file failed.\r\n";
	else
		str = "250 DELETE file success.\r\n";
	m_socka.Send(str.c_str(), str.length());
	return true;
}
bool Protocal::List()
{
	string listPath = GetPath();
	filesystem::path path(listPath.c_str());
	if (!filesystem::exists(path)|| !filesystem::is_directory(path))
		return false;
	
	string str = "150 File status okay.\r\n";
	m_socka.Send(str.c_str(), str.length());
	for (auto&& file : filesystem::directory_iterator(path))
	{
		string fileSize;
		if (filesystem::is_regular_file(file.path()))
		{
			str = '-';
			fileSize = to_string(filesystem::file_size(file.path()));
		}
		else if (filesystem::is_directory(file.path()))
		{
			str = 'd';
			fileSize = "0";
		}
		str += "rwxrwxrwx 1 " + m_sName + " nogroup "
			+ fileSize + " "
			+ get_file_time(filesystem::last_write_time(file.path()))
			+ " " + file.path().filename().string() + "\r\n";
		m_sockt.Send(str.c_str(), str.length());
	}	
	str = "226 Transfer finished successfully. Data connection closed.\r\n";
	m_socka.Send(str.c_str(), str.length());
	m_sockt.Close();
	m_sockp.Close();
	return true;
}
bool Protocal::Pasv()
{
	string str;
	m_sockp.Close();
	m_sockt.Close();

	if (!m_sockp.Create() || !m_sockp.Listen())
	{
		str = "425 Passive mode create failed.\r\n";
		m_socka.Send(str.c_str(), str.length());
		return true;
	}
	char sIP[32];
	UINT nPort;
	m_sockp.GetSockName(sIP, &nPort);
	int high = nPort >> 8;
	int low = nPort & 0x000000ff;
	m_socka.GetSockName(sIP, &nPort);
	replace(sIP, '.', ',');
	
	str = "227 (" + string(sIP) + "," + to_string(high) + "," + to_string(low) + ") data socket is ok.\r\n";
	m_socka.Send(str.c_str(), str.length());
	if (!m_sockp.Accept(m_sockt))
		return false;

	return true;
}

bool Protocal::Cwd(const char * sData)
{
	string str = m_sCwd;
	if (*sData == '/')
	{
		str = sData;
	}
	else
	{
		if (str[str.length() - 1] != '/')
			str += '/';
		str += sData;
	}		
	if (str[str.length() - 1] != '/')
		str += '/';
	string path = m_sRoot + str;
	if (filesystem::exists(path) && filesystem::is_directory(path))
	{
		m_sCwd = str;
		str = string("250 ") + "\"" + m_sCwd + "\" is current directory.\r\n";
	}
	else
		str = "550 Cannot change directory(" + m_sCwd + ").\r\n";
	m_socka.Send(str.c_str(), str.length());
	return true;
}
bool Protocal::Type(const char * sData)
{
	if (toupper(*sData) == 'I')
		m_type = T_BINARY; 
	else
		m_type = T_TEXT;
	string str = string("200 ") + sData + " mode is set.\r\n";
	m_socka.Send(str.c_str(), str.length());
	return true;
}
bool Protocal::Pwd()
{
	string str = "257 \"" + m_sCwd + "\" is current directory.\r\n";
	m_socka.Send(str.c_str(), str.length());
	return true;
}
bool Protocal::Pass(const char * sData)
{
	string str;
	if (sData && FTPServer::m_serv.CheckUserPass(m_sName, sData))
	{
		str = "230 User logged in success.\r\n";
		m_sRoot = FTPServer::m_serv.GetRootPath(m_sName);
	}
	else
		str = "530 Login incorrect.\r\n";

	m_socka.Send(str.c_str(), str.length());
	return true;
}
bool Protocal::User(const char * sData)
{
	string str;

	if (FTPServer::m_serv.IsExistUser(sData))
	{
		str = string("331 Password required for ") + sData + ".\r\n";
		m_sName = sData;
	}
	else
		str = string("530 Login incorrect for ") + sData + ".\r\n";
	m_socka.Send(str.c_str(), str.length());
	return true;
}
bool Protocal::Unknow(const char *sCmd)
{
	string str;
	str = string("502 ") + (sCmd ? sCmd : "") + " not implemented.\r\n";
	m_socka.Send(str.c_str(), str.length());
	return true;
}

bool Protocal::Process(char *sData)
{
	char* sCmd = strtok(sData, " \r\n");
	if (!sCmd)
		return false;
	sData = strtok(NULL, "\r\n");
	strupper(sCmd);

	if (!strcmp(sCmd, "USER"))
		return User(sData);
	if (!strcmp(sCmd, "PASS"))
		return Pass(sData);
	if (!strcmp(sCmd, "PWD"))
		return Pwd();
	if (!strcmp(sCmd, "TYPE"))
		return Type(sData);
	if (!strcmp(sCmd, "CWD"))
		return Cwd(sData);
	if (!strcmp(sCmd, "PASV"))
		return Pasv();

	if (!strcmp(sCmd, "LIST"))
		return List();
	if (!strcmp(sCmd, "DELE"))
		return Dele(sData);
	if (!strcmp(sCmd, "RMD"))
		return Rmd(sData);
	if (!strcmp(sCmd, "MKD"))
		return Mkd(sData);
	if (!strcmp(sCmd, "RNFR"))
		return Rnfr(sData);
	if (!strcmp(sCmd, "RNTO"))
		return Rnto(sData);
	if (!strcmp(sCmd, "STOR"))
		return Stor(sData);
	if (!strcmp(sCmd, "SIZE"))
		return Size(sData);
	if (!strcmp(sCmd, "RETR"))
		return Retr(sData);

	return Unknow(sCmd);
};

bool Protocal::Run(char* s)
{
	int nLen = m_socka.Receive(s, BUFFER_SIZE);
	if (nLen <= 0)
		return false;
	s[nLen] = 0;
	cout << s << endl;

	return Process(s);
}

void Protocal::WorkProc(Protocal *p)
{
	try
	{
		char s[BUFFER_SIZE];
		cout << "WorkProc start:" << m_nWorkCount << endl;
		while (FTPServer::IsRun() && p->Run(s))
			;
		delete p;
		cout << "WorkProc exit:" << m_nWorkCount << endl;
		--m_nWorkCount;
	}
	catch (const std::exception &e)
	{
		std::cout << e.what() << std::endl;
		std::cin.get();
		return;
	}
}

void Protocal::Do()
{

	++m_nWorkCount;
	const char* p = "220 welcome guy.\r\n";
	m_socka.Send(p, strlen(p));
	thread t(WorkProc, this);
	t.detach();
}

std::string Protocal::GetPath()
{
	string str = m_sRoot + m_sCwd;
	if (str[str.length() - 1] != '/')
		str += '/';
	return str;
}