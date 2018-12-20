#pragma once
#include"Socket.h"
class Protocal
{
	Socket m_socka, m_sockp, m_sockt;
	std::string m_sName;
	std::string m_sPass;
	std::string m_sRoot;
	std::string m_sCwd{ "/" };
	std::string m_sOldName;
	enum { BUFFER_SIZE = 128 };
	enum ModeType { T_TEXT, T_BINARY };
	ModeType m_type;
	bool Retr(const char * sData);
	bool Size(const char * sData);
	bool Stor(const char * sData);
	bool Rnto(const char* sData);
	bool Rnfr(const char* sData);
	bool Mkd(const char * sData);
	bool Rmd(const char * sData);
	bool Dele(const char * sData);
	bool List();
	bool Pasv();
	bool Cwd(const char * sData);
	bool Type(const char * sData);
	bool Pwd();
	bool Pass(const char * sData);
	bool User(const char* sData);
	bool Unknow(const char* sData);
	bool Process(char* sCmd);
	bool Run(char* s);
	std::string GetPath();
public: 
	static void WorkProc(Protocal *p);
	void Do();
	Socket&  GetSocket()
	{
		return m_socka;
	}
	Protocal() = default;
	~Protocal() = default;
};

