#include "Socket.h"
#ifdef _WIN32
#pragma comment (lib,"ws2_32.lib")
#endif
BOOL Socket::Create(UINT nPort, int nSocketType, LPCTSTR lpszSocketAddress)
{
    SOCKET sock = socket(AF_INET, nSocketType, 0);
	if (sock == INVALID_SOCKET)
		return FALSE;
#ifndef _WIN32
    int on = 1;
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
        return FALSE;
#endif
	sockaddr_in sa = { AF_INET,htons(nPort) };
	if (lpszSocketAddress)
		sa.sin_addr.s_addr = inet_addr(lpszSocketAddress);
	if (bind(sock, (sockaddr*)&sa, sizeof(sa)))
	{
		closesocket(sock);
		return FALSE;
	}
	m_hSocket = sock;
	return TRUE;
}

BOOL Socket::Connect(LPCTSTR lpszHostAddress, UINT nHostPort)
{
	sockaddr_in sa = { AF_INET,htons(nHostPort) };
	sa.sin_addr.s_addr = inet_addr(lpszHostAddress);
	return SOCKET_ERROR != connect(m_hSocket, (sockaddr*)&sa, sizeof(sa));
}

int Socket::SendTo(const void * lpBuf, int nBufLen, UINT nHostPort, LPCTSTR lpszHostAddress, int nFlags)
{
	sockaddr_in sa = { AF_INET,htons(nHostPort) };
	if (lpszHostAddress)
		sa.sin_addr.s_addr = inet_addr(lpszHostAddress);
	else
	{
		//gethostbyname()//
	}
	return sendto(m_hSocket, (const char*)lpBuf, nBufLen, nFlags, (sockaddr*)&sa, sizeof(sa));
}

int Socket::ReceiveFrom(void * lpBuf, int nBufLen, LPSTR rSocketAddress, UINT * rPort, int nFlags)
{
	sockaddr_in sa;
	socklen_t nSize = sizeof(sa);
	int nRes =recv(m_hSocket, (char*)lpBuf, nBufLen, nFlags);

	if (nRes > 0)
	{
		if(rPort || rSocketAddress)
			getpeername(m_hSocket,  (sockaddr*)&sa, &nSize);
		if(rPort)
			*rPort = htons(sa.sin_port);
		if (rSocketAddress)
			inet_ntop(AF_INET, &sa.sin_addr, rSocketAddress, 16);
			
	}
	return nRes;
}

BOOL Socket::Accept(Socket & rSocket, char * sIP, UINT * rPort)
{
	if (sIP || rPort)
	{
		sockaddr_in sa;
		socklen_t nLen = sizeof(sa);
		rSocket.m_hSocket = accept(m_hSocket, (sockaddr*)&sa, &nLen);
		if(sIP)
			inet_ntop(AF_INET, &sa.sin_addr, sIP, 16);
		if (rPort)
			*rPort = htons(sa.sin_port);
	}
	else
		rSocket.m_hSocket = accept(m_hSocket, NULL, NULL);
	return rSocket.m_hSocket != INVALID_SOCKET;
}


int Socket::GetSockName(LPSTR rSocketAddress, UINT * rPort)
{
	sockaddr_in sa;
	socklen_t nLen = sizeof(sa);
	int nRes = getsockname(m_hSocket,(sockaddr*) &sa, &nLen);
	if (!nRes)
	{
		if (rSocketAddress)
			inet_ntop(AF_INET, &sa.sin_addr, rSocketAddress, 16);
		if (rPort)
			*rPort = htons(sa.sin_port);
	}
	return nRes;
}

int Socket::GetPeerName(LPSTR rSocketAddress, UINT * rPort)
{
	sockaddr_in sa;
	socklen_t nLen = sizeof(sa);
	int nRes = getpeername(m_hSocket, (sockaddr*)&sa, &nLen);
	if (!nRes)
	{
		if (rSocketAddress)
			inet_ntop(AF_INET, &sa.sin_addr, rSocketAddress, 16);
		if (rPort)
			*rPort = htons(sa.sin_port);
	}
	return nRes;
}

Socket::Socket():m_hSocket(INVALID_SOCKET)
{
#ifdef _WIN32
	WSAData wd;
	WSAStartup(0x0202, &wd);
#endif
}

Socket::~Socket()
{
	if (INVALID_SOCKET != m_hSocket)
		closesocket(m_hSocket);
}
#include <iostream>
using namespace std;
SocketEx::SocketEx(const char * sIP)
{
	if (!Create())
		return;
	enum { SERVER_PORT = 8888 };

	if (!Connect(sIP, SERVER_PORT))
	{
		cout << "connect server fail:" << GetLastError() << endl;
		Close();
	}

}
