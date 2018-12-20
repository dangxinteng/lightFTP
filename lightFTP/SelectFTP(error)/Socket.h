#pragma once
#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <process.h>
typedef int socklen_t;
#else
#include <netinet/in.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
typedef int SOCKET;
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

#define CONST const
typedef char CHAR;
typedef short SHORT;
typedef long LONG;
typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef FLOAT               *PFLOAT;
typedef BOOL            *PBOOL;
typedef BOOL             *LPBOOL;
typedef BYTE            *PBYTE;
typedef BYTE             *LPBYTE;
typedef int             *PINT;
typedef int              *LPINT;
typedef WORD            *PWORD;
typedef WORD             *LPWORD;
typedef long             *LPLONG;
typedef DWORD           *PDWORD;
typedef DWORD            *LPDWORD;
typedef void             *LPVOID;
typedef CONST void *LPCVOID;

typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int        *PUINT;


typedef  CONST CHAR *LPCSTR, *PCSTR;
typedef  CHAR *LPSTR, *PSTR;
typedef LPCSTR LPCTSTR;
typedef LPSTR LPTSTR;

#define closesocket(x) close(x)

#endif
class Socket
{
protected:
	SOCKET m_hSocket;
public:
	operator SOCKET() const
	{
		return m_hSocket;
	}
	BOOL Create(UINT nPort = 0, int nSocketType = SOCK_STREAM, LPCTSTR lpszSocketAddress = NULL);
	inline int Receive(void* lpBuf, int nBufLen, int nFlags = 0)
	{
		return recv(m_hSocket, (char*)lpBuf, nBufLen, nFlags);
	}
	inline int Send(const void* lpBuf, int nBufLen, int nFlags = 0)
	{
		return send(m_hSocket, (char*)lpBuf, nBufLen, nFlags);
	}
	int SendTo(const void* lpBuf, int nBufLen, UINT nHostPort, LPCTSTR lpszHostAddress = NULL, int nFlags = 0);
	int ReceiveFrom(void* lpBuf, int nBufLen, LPSTR rSocketAddress = NULL, UINT* rPort = NULL, int nFlags = 0);
	BOOL Accept(Socket& rSocket, char* sIP=NULL, UINT* rPort=NULL);
	inline BOOL Listen(int nBacklog = 5)
	{
		return SOCKET_ERROR != listen(m_hSocket, nBacklog);
	}
	DWORD GetLastError() const
	{
#ifdef _WIN32
		return WSAGetLastError();
#else
		return errno;
#endif
	}
	void Close()
	{
		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}
	BOOL Connect(LPCTSTR lpszHostAddress,UINT nHostPort);
	int GetSockName(LPSTR rSocketAddress = NULL, UINT* rPort = NULL);
	int GetPeerName(LPSTR rSocketAddress = NULL, UINT* rPort = NULL);
	Socket();
	~Socket();
};
class SocketEx :public Socket
{
public:
	SocketEx(const char* sIP = "192.168.8.134");
	bool IsValid()const
	{
		return m_hSocket != INVALID_SOCKET;
	}
};