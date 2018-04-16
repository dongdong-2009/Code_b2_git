#include "StdAfx.h"
#include "TCPClient.h"
#include "HexStringHelper.h"
#include "CRCCalculator.h"
#include "SimulatorConfig.h"

//#include "Cxml.h"

TCPClient::TCPClient(HWND hWnd)
:m_hWnd(hWnd),m_connected(false),m_bHex(true),m_bWithCRC(false),m_server(INVALID_SOCKET)
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
	//if(NULL!=m_cml)
	//{
	//	delete m_cml;
	//	m_cml = NULL;
	//}
	if(m_connected)
	{
		StopConnect();
		CloseConnection();
	}

	WSACleanup( );
}

bool TCPClient::CreateSocket()
{
	int errorCode;
	char errorMsg[1024*60];
	if(SimulatorConfig::getInstance()->m_bIsUDP)
		m_server = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	else
		m_server = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	memset(errorMsg,0,sizeof(errorMsg));

	if(m_server==INVALID_SOCKET)
	{
		errorCode=WSAGetLastError();
		closesocket(m_server);
		sprintf(errorMsg,"Fail to set connect socket event,error code:[%d]",errorCode);
		NotifyUI("Error",string(errorMsg));
		return false;
	}
	return true;
}

bool TCPClient::Connect(const string& ip,int port)
{
	int errorCode;
	char errorMsg[1024*60];
	if(m_server ==INVALID_SOCKET)
		if(!CreateSocket())
			return false;
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

//ip and port are useful only in the case of UDP socket
bool TCPClient::Send(const string& strMessage,const string& ip,int port)
{
	int errorCode;
	char errorMsg[1024*60];
	string inputData;
	bool bIsUDP = SimulatorConfig::getInstance()->m_bIsUDP;
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
	if(m_server == INVALID_SOCKET)
		if(!CreateSocket())
			return false;
	int rtn;  
	sockaddr_in addrserver;
	addrserver.sin_family=AF_INET;
	addrserver.sin_port=htons(port);
	addrserver.sin_addr.s_addr= inet_addr(ip.c_str());
	int addrlen = sizeof(sockaddr); 
	if(bIsUDP)
		rtn = sendto(m_server,inputData.c_str(),inputData.length(),0,(sockaddr*)&addrserver,addrlen);
	else
		rtn = send(m_server,inputData.c_str(),inputData.length(),0);
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
	if(!SimulatorConfig::getInstance()->m_bIsUDP)
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


/*void AppendInputRecord(string strInput)
{
	FILE *fp=fopen("TCPInputRecord.txt","a+t");
	if(NULL==fp)
		return;
	fprintf(fp,"%s",strInput.c_str());
	fclose(fp);
}*/

typedef std::basic_string<TCHAR> tstring;  

TCHAR* StringToTCHAR(string& s)
{
	tstring tstr;
	const char* all = s.c_str();
	int len = 1 + strlen(all);
	wchar_t* t = new wchar_t[len]; 
	if (NULL == t) throw std::bad_alloc();
	mbstowcs(t, all, len);
	return (TCHAR*)t;
}

std::string TCHARToString(const TCHAR* ptsz)
{
	int len = wcslen((wchar_t*)ptsz);
	char* psz = new char[2*len + 1];
	wcstombs(psz, (wchar_t*)ptsz, 2*len + 1);
	std::string s = psz;
	delete [] psz;
	return s;
}

void TCPClient::ReadSocketInput(SOCKET sSocket)
{
	char buf[1024*60];
	memset(buf,0,sizeof(buf));
	int recvlen=recv(sSocket,buf,sizeof(buf),0);
	string strInput;
	strInput.erase(strInput.begin(),strInput.end());
	strInput.append(buf,recvlen);
	if(m_bHex)
	{
		string inputData;
		HexStringHelper::HexString(strInput,inputData);
		NotifyUI("Recieved",inputData);
	}
	else
		NotifyUI("Recieved",strInput);
	/*AppendInputRecord(strInput);
	if(NULL!=strInput.find("<rss"))
	{
		m_recvBuf.erase(m_recvBuf.begin(),m_recvBuf.end());
	}
	m_recvBuf.append(strInput);
	if(NULL!=strInput.find("/rss>"))
	{
		TCHAR* ptchar = StringToTCHAR(m_recvBuf);
		m_cml->ParseString( ptchar);
		
		CNode* root =m_cml->GetRootNode();
		do 
		{
			TCHAR* name = root->GetName();
			TCHAR* value = root->GetValue();
		} while (root->GetNextChild()!=NULL);
		delete ptchar;
	}*/
	if(m_bWithCRC)
	{
		string strData="";
		strData.append(buf,recvlen-2);
		unsigned short crc = CRCCalculator::getCRC(strData);
		if((crc>>8)!=strInput.c_str()[recvlen-2]||(crc&0xff)!=strInput.c_str()[recvlen-1])
			NotifyUI("Error","Failed to valicate CRC");
	}
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
