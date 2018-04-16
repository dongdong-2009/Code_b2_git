#pragma once
#include <string>
#include <iostream>
#include "Winsock2.h"
#include "HexStringHelper.h"

using namespace std;

#define WM_CLIENT_EVENT WM_USER+310
#define WM_CLIENT_NOTE  WM_USER+330


class TCPClient
{
public:
	TCPClient(HWND hWnd);
	~TCPClient(void);
	LRESULT OnSocketEvent(WPARAM wParam,LPARAM lParam);
	bool Connect(const string& ip,int port);
	bool Send(const string& strMessage);
	bool StopConnect();
	inline void SetHex(bool bHex) {m_bHex=bHex;};
	inline void SetCRC(bool bCRC) {m_bWithCRC=bCRC;};
private:
	
	void CloseConnection();
	void OnConnect();
	void ReadSocketInput(SOCKET sSocket);
	void NotifyUI(const string& caption,const string& msg);
private:
	string m_ip;
	int	m_port;
	SOCKET m_server;
	HWND m_hWnd;
	bool m_connected;
	bool m_bHex,m_bWithCRC;
	HexStringHelper m_hexStringHelper;
};
