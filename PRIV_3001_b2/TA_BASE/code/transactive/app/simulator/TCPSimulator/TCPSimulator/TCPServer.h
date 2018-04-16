#pragma once
#include <string>
#include <iostream>
#include <list>
#include "Winsock2.h"


using namespace std;

#define WM_SERVER_EVENT WM_USER+300
#define WM_SERVER_NOTE  WM_USER+320

class IMessageHandler;
class TCPServer
{
public:
	TCPServer(HWND hWnd);
	LRESULT OnSocketEvent(WPARAM wParam,LPARAM lParam);
	bool StartServer(const string& ip,int port,const string& handleType);
	bool UDPReadLoop();
	bool StopServer();
	bool Send(const string& strMsg);
	inline IMessageHandler* getMessageHandler() { return m_msgHandler;};
	inline bool IsUDPLooping() {return m_bUDPLooping;};
	inline void SetHex(bool bHex) {m_bHex=bHex;};
	inline void SetCRC(bool bCRC) {m_bWithCRC=bCRC;};
public:
	~TCPServer(void);

private:
	void OpenConnection();
	void CloseConnection();
	
	void AcceptSocket();
	void ReadSocketInput(SOCKET cSocket);
	void DisconnectClient(SOCKET sSocket);
	void NotifyUI(const string& caption,const string& msg);
private:
	string m_ip;
	int	m_port;
	SOCKET m_server;
	bool m_bIsUDP;
	list<SOCKET> m_clients;
	HWND m_hWnd;
	IMessageHandler* m_msgHandler;
	bool m_bHex,m_bWithCRC;
	bool m_bUDPLooping;
};
