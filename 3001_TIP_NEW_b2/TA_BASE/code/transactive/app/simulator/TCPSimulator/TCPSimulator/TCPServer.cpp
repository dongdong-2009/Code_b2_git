#include "StdAfx.h"
#include "TCPServer.h"

#include "ATSMessagehandler.h"
#include "ATSC95MessageHandler.h"
#include "TVSSMessageHandler.h"
#include "RadioMessageHandler.h"
#include "CCTVMessageHandler.h"
#include "TestMessagehandler.h"
#include "HexStringHelper.h"
#include "CRCCalculator.h"


TCPServer::TCPServer(HWND hWnd)
:m_hWnd(hWnd),m_msgHandler(NULL),m_bHex(true),m_bWithCRC(false),m_clients(),m_bUDPLooping(false)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD( 2, 2 );
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) 
		return;
	

}

TCPServer::~TCPServer(void)
{
	if(m_msgHandler!=NULL)
	{
		delete m_msgHandler;
		m_msgHandler = NULL;
	}
	StopServer();
//	CloseConnection();
	WSACleanup( );
}

bool TCPServer::StartServer(const string& ip,int port,const string& handleType)
{
	if(m_msgHandler!=NULL)
	{
		delete m_msgHandler;
		m_msgHandler = NULL;
	}
	m_bIsUDP = SimulatorConfig::getInstance()->m_bIsUDP;
	if("ATS" == handleType)
		m_msgHandler = new ATSMessageHandler(m_hWnd,WM_SERVER_NOTE);
	else if("ATSC95" ==handleType)
		m_msgHandler = new ATSC95MessageHandler(m_hWnd,WM_SERVER_NOTE,SimulatorConfig::getInstance()->m_baseAddr);
	else if("TVSS" == handleType)
		m_msgHandler = new TVSSMessageHandler(m_hWnd,WM_SERVER_NOTE);
	else if("RADIO" == handleType)
		m_msgHandler = new RadioMessageHandler(m_hWnd,WM_SERVER_NOTE);
	else if("CCTV" == handleType)
		m_msgHandler = new CCTVMessageHandler(m_hWnd,WM_SERVER_NOTE);
	else if("TEST" == handleType)
		m_msgHandler = new TestMessageHandler(m_hWnd);
	else
	{
		NotifyUI("Error","Require unknown type of message handler");
		return false;
	}
	int errorCode;
	char errorMsg[1024*60];
	memset(errorMsg,0,sizeof(errorMsg));
	m_ip =ip;
	m_port = port;
	if(m_bIsUDP)
		m_server = socket(AF_INET,SOCK_DGRAM,0);
	else
		m_server = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	//m_server = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	sockaddr_in serviceAddr;
	serviceAddr.sin_family = AF_INET;
	if(m_bIsUDP)
		serviceAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	else
		serviceAddr.sin_addr.s_addr = inet_addr(m_ip.c_str());
	serviceAddr.sin_port = htons(m_port);
	int rtn = bind(m_server,(SOCKADDR*) &serviceAddr,sizeof(serviceAddr)) ;
	if(rtn == SOCKET_ERROR) 
	{
		closesocket(m_server);
		errorCode=WSAGetLastError();
		sprintf(errorMsg,"Fail to bind server address,error code:[%d]",errorCode);
		NotifyUI("Error",string(errorMsg));

		return false;
	}
	if(m_bIsUDP)
	{
		m_bUDPLooping = true;
		int iMode = 1;
		ioctlsocket(m_server,FIONBIO, (u_long FAR*) &iMode);//
		//UDPReadLoop();
	}
	else
	{
		rtn = WSAAsyncSelect(m_server,m_hWnd,WM_SERVER_EVENT,FD_ACCEPT|FD_READ|FD_CLOSE);
		if(rtn == SOCKET_ERROR) 
		{
			closesocket(m_server);
			errorCode=WSAGetLastError();
			sprintf(errorMsg,"Fail to set socket event,error code:[%d]",errorCode);
			NotifyUI("Error",string(errorMsg));
			return false;
		}
		listen(m_server,5);
	}
	NotifyUI("Start","Server started");
	return true;
}

bool TCPServer::UDPReadLoop()
{
	MSG message;
	while(m_bUDPLooping)
	{
		ReadSocketInput(m_server);
		for(int i=0;i<1000;i++)
		if (::PeekMessage(&message,NULL,0,0,PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
		}
		Sleep(100);
	}
	return true;
}

LRESULT TCPServer::OnSocketEvent(WPARAM wParam,LPARAM lParam)
{
	switch(WSAGETSELECTEVENT(lParam))
	{
	case FD_ACCEPT:
		AcceptSocket();
		break;
	case FD_READ:
		ReadSocketInput((SOCKET)wParam);
		break;
	case FD_CLOSE:
		DisconnectClient((SOCKET)wParam);
		break;
	}
	return 0l;
}

void TCPServer::AcceptSocket()
{
	int addrlen=sizeof(sockaddr);
	sockaddr_in addr;
	SOCKET client=accept(m_server,(sockaddr*)(&addr),&addrlen);
	m_clients.push_back(client);
	char chInfo[256];
	memset(chInfo,0,sizeof(chInfo));
	sprintf(chInfo,"Client (%u) ip: %d.%d.%d.%d : %u",client,addr.sin_addr.S_un.S_un_b.s_b1,addr.sin_addr.S_un.S_un_b.s_b2,
		addr.sin_addr.S_un.S_un_b.s_b3,addr.sin_addr.S_un.S_un_b.s_b4,addr.sin_port);
	NotifyUI("Accepted",string(chInfo));
}


void TCPServer::ReadSocketInput(SOCKET sSocket)
{
	char buf[1024*60];
	memset(buf,0,sizeof(buf));
	int recvlen=0;
	SOCKADDR_IN addrClient;  
	int addrlen = sizeof(sockaddr); 
	if(m_bIsUDP)
	{	
		recvlen = recvfrom( sSocket,buf,sizeof(buf),0,(sockaddr*)&addrClient,&addrlen);
		if(recvlen<=0)
		{
			if(recvlen == SOCKET_ERROR) 
			{
				int errorCode;
				char errorMsg[1024*60];
				errorCode=WSAGetLastError();
				if(errorCode!=WSAEWOULDBLOCK)
				{
					closesocket(m_server);
				
					sprintf(errorMsg,"Fail to read message,error code:[%d]",errorCode);
					NotifyUI("Error",string(errorMsg));
					m_bUDPLooping = false;
				}
			}
			return;
		}
	}
	else
		recvlen = recv(sSocket,buf,sizeof(buf),0);
	string strInput;
	string msgTitle = "Received";
	if(m_bIsUDP)
	{
		char* ipClient = inet_ntoa(addrClient.sin_addr);
		msgTitle.append("(");
		msgTitle.append(ipClient);
		msgTitle.append(")");
	}
	strInput.erase(strInput.begin(),strInput.end());
	strInput.append(buf,recvlen);
	if(m_bHex)
	{
		string inputInputHex;
		HexStringHelper::HexString(strInput,inputInputHex);
		NotifyUI(msgTitle,inputInputHex);
	}
	else
		NotifyUI(msgTitle,strInput);
	
	string strData = "";
	if(m_bWithCRC)
	{
		strData = "";
		strData.append(buf,recvlen-2);
		unsigned short crc = CRCCalculator::getCRC(strData);
		if(strInput.c_str()[recvlen-2]!=(crc>>8) || strInput.c_str()[recvlen-1]!=(crc&0xff))
		{
			NotifyUI("Error","Failed to valicate CRC");
			return ;
		}
	}
	else
		strData.append(buf,recvlen);
	m_msgHandler->setInputData(strData);
	if(m_msgHandler->DoAnalyze())
	{
		string strOutput = m_msgHandler->getOutputData();
		if(m_bWithCRC)
		{
			unsigned short crc = CRCCalculator::getCRC(strOutput);
			strOutput.push_back(crc>>8);
			strOutput.push_back(crc&0xff);
		}
		if(m_bIsUDP)
		{
			sendto(sSocket,strOutput.c_str(),strOutput.length(),0,(sockaddr*)&addrClient,addrlen);
		}
		else
			send(sSocket,strOutput.c_str(),strOutput.length(),0);
		if(m_bHex)
		{
			string outputData;
			HexStringHelper::HexString(strOutput,outputData);
			NotifyUI("Replied",outputData);
		}
		else
			NotifyUI("Replied",strOutput);
		
	}
}

bool TCPServer::Send(const string& strMsg)
{
	if(m_clients.empty())
	{
		NotifyUI("Error","No client connect to this server");
		return false;
	}
	string outputBuf;
	if(m_bHex)
	{	
		HexStringHelper::StringHex(strMsg,outputBuf);	
	}
	else
		outputBuf.append(strMsg.c_str(),strMsg.length());
	if(m_bWithCRC)
	{
		unsigned short crc = CRCCalculator::getCRC(outputBuf);
		outputBuf.push_back(crc>>8);
		outputBuf.push_back(crc&0xff);
	}
	list<SOCKET>::iterator sock_it = m_clients.begin();
	for(;sock_it!=m_clients.end();sock_it++)
		send( *sock_it,outputBuf.c_str(),outputBuf.length(), 0);
	string outputData;
	if(m_bHex)
	{	
		HexStringHelper::HexString(outputBuf,outputData);	
	}
	else
		outputData=strMsg.c_str();
	NotifyUI("Sent",outputData);
}

void TCPServer::DisconnectClient(SOCKET sSocket)
{
	char chInfo[1024*60];
	sprintf(chInfo,"Client (%u) disconnected",sSocket);
	NotifyUI("Disconnected",string(chInfo));
	m_clients.remove(sSocket);
}

void TCPServer::OpenConnection()
{
	;
}

bool TCPServer::StopServer()
{
	if(m_bIsUDP)
	{
		m_bUDPLooping = false;
	}
	else
		WSAAsyncSelect(m_server,m_hWnd,WM_SERVER_EVENT,0);
	CloseConnection();
	NotifyUI("Stopped","Server stopped");
	return true;
}

void TCPServer::CloseConnection()
{
	if(!m_clients.empty())
	{
		list<SOCKET>::iterator sock_it=m_clients.begin();
		for(;sock_it!=m_clients.end();sock_it++)
			shutdown(*sock_it,SD_SEND);
	}
	closesocket(m_server);
}

void TCPServer::NotifyUI(const string& caption,const string& msg)
{
	::SendMessage(m_hWnd,WM_SERVER_NOTE,(WPARAM)&caption,(LPARAM)&msg);
}
