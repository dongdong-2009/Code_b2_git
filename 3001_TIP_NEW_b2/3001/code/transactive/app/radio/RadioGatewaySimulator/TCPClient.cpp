#include "StdAfx.h"
#include "TCPClient.h"
#include "HexStringHelper.h"
#include "CRCCalculator.h"
#include "winsock2.h"

TCPClient::TCPClient(HWND hWnd)
:m_hWnd(hWnd),m_connected(false),m_bHex(true),m_bWithCRC(false)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD( 2, 2 );
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) 
		return;
	
}

TCPClient::~TCPClient(void)
{
	if(m_connected)
	{
		StopConnect();
		CloseConnection();
	}
	WSACleanup( );
}

bool TCPClient::Connect(const string& ip,int port)
{
	int errorCode;
	char errorMsg[1024*60];
	m_server = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	memset(errorMsg,0,sizeof(errorMsg));
	sockaddr_in addrserver;
	addrserver.sin_family=AF_INET;
	addrserver.sin_port=htons(port);
	addrserver.sin_addr.s_addr= inet_addr(ip.c_str());
	int rtn  = WSAAsyncSelect(m_server,m_hWnd,WM_CLIENT_EVENT,FD_CONNECT|FD_READ|FD_CLOSE);
	if(SOCKET_ERROR==rtn)
	{
		errorCode=WSAGetLastError();
		closesocket(m_server);
		sprintf(errorMsg,"Fail to set connect socket event,error code:[%d]",errorCode);
		NotifyUI("Error",string(errorMsg));
		return false;
	}
	for(int i(0);i<5;i++)
	{
		rtn=connect(m_server,(sockaddr*)&addrserver,sizeof(sockaddr));
		if(SOCKET_ERROR==rtn)
		{
			errorCode=WSAGetLastError();
			if(errorCode == WSAEISCONN)
				break;
			else if	(errorCode == WSAEWOULDBLOCK)
			{
				Sleep(1000);
				continue;
			}
			closesocket(m_server);
			sprintf(errorMsg,"Fail to connect server,error code:[%d]",errorCode);
			NotifyUI("Error",string(errorMsg));
			return false;
			
		}
		else
			break;
	}
	NotifyUI("Connected","Connected to server");
	m_connected = true;
	return true;
}

bool TCPClient::Send(const string& strMessage)
{
	int errorCode;
	char errorMsg[1024*60];
	string inputData;
	if(m_bHex)
	{	
		HexStringHelper::StringHex(strMessage,inputData);
	}
	else
		inputData.append(strMessage.c_str(),strMessage.length());
	if(m_bWithCRC)
	{
		unsigned short crc = CRCCalculator::getCRC(inputData);
		inputData.push_back(crc>>8);
		inputData.push_back(crc&0xff);
	}
	int rtn = send(m_server,inputData.c_str(),inputData.length(),0);
	if(SOCKET_ERROR==rtn)
	{
		errorCode=WSAGetLastError();	
		sprintf(errorMsg,"Fail to send message,error code:[%d]",errorCode);
		NotifyUI("Error",string(errorMsg));
		return false;
	}
	string outputData;
	if(m_bHex)
	{	
		HexStringHelper::HexString(inputData,outputData);
	}
	else
		outputData = inputData;
	NotifyUI("Sent",outputData);
	return true;
}

bool TCPClient::StopConnect()
{
	WSAAsyncSelect(m_server,m_hWnd,WM_CLIENT_EVENT,0);
	shutdown(m_server,SD_SEND);
	//CloseConnection();
	return true;
}

void TCPClient::CloseConnection()
{
	closesocket(m_server);
	m_connected = false;
	NotifyUI("Disconnected","Disconnected from server");
}

void TCPClient::OnConnect()
{
//	NotifyUI("Connected","Connected to server");
}

void TCPClient::ReadSocketInput(SOCKET sSocket)
{
	char buf[1024*60];
	memset(buf,0,sizeof(buf));
	
	char * pszData = new char [2048];

	int recvlen=recv(sSocket,pszData,2048,0);
	string strInput;
	strInput.erase(strInput.begin(),strInput.end());
	strInput.append(pszData,recvlen);
	if(m_bHex)
	{
		string inputData;
		HexStringHelper::HexString(strInput,inputData);
		NotifyUI("Recieved",inputData);
	}
	else
		NotifyUI("Recieved",strInput);
	if(m_bWithCRC)
	{
		string strData="";
		strData.append(pszData,recvlen-2);
		unsigned short crc = CRCCalculator::getCRC(strData);
		if((crc>>8)!=strInput.c_str()[recvlen-2]||(crc&0xff)!=strInput.c_str()[recvlen-1])
			NotifyUI("Error","Failed to valicate CRC");
	}

	delete [] pszData;
}

LRESULT TCPClient::OnSocketEvent(WPARAM wParam,LPARAM lParam)
{
	switch(WSAGETSELECTEVENT(lParam))
	{
	case FD_CONNECT:
		OnConnect();
		break;
	case FD_READ:
		ReadSocketInput((SOCKET)wParam);
		break;
	case FD_CLOSE:
		CloseConnection();
		break;
	}
	return 0l;
}


void TCPClient::NotifyUI(const string& caption,const string& msg)
{
	::SendMessage(m_hWnd,WM_CLIENT_NOTE,(WPARAM)&caption,(LPARAM)&msg);
}
