#include "ClientComWorker.h"
	
#include "core/sockets/src/TcpSocket.h"
#include "core/sockets/src/SocketSet.h"							   
#include "core/sockets/src/TcpServerSocket.h"

#include "core/utilities/src/DebugUtil.h"

#include "IMsgItem.h"
#include "MsgItemImp.h"
#include "IMsgPackage.h"
#include "MsgPackageImp.h"

#include "ComProtocol.h"
#include "IObserverRcvMsg.h"

using TA_Base_Core::DebugUtil;

NS_BEGIN(TA_DB_Sync)


CClientComWorker::CClientComWorker(
const std::string& strServerIP, 
const std::string& strServerPort, 
const std::string& strQName,  
const std::string& strServerID)
{	
	m_strServerIP = strServerIP;
	m_strServerPort = strServerPort;
	m_strServerID = strServerID;
	
	m_pRecvBufferOnce = new CBuffer(BUFFER_LEN_READ);
	m_pRecvBufferTotal = new CBuffer(BUFFER_LEN_READ);

	m_lstRecvMsgPackage.clear();
	m_pObserverRcvMsg = NULL;
	m_nLogCount = 0;
	m_pComProtocol = new CComProtocol();
	m_socketTimeout = SERVER_LIMIT_TCP_TIMEOUT;
	
	m_pSocket = NULL;
	m_isConnected = false;
	m_toTerminate = false;

}

CClientComWorker::~CClientComWorker()
{
	m_pObserverRcvMsg = NULL;
	
	DEF_DELETE(m_pRecvBufferOnce);	
	DEF_DELETE(m_pRecvBufferTotal);
		
	DEF_DELETE(m_pSocket);
	_FreeLstRecvMsgPackage();
	
}


void CClientComWorker::run()
{
	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "CClientComWorker  begin run()");

	while( false == m_toTerminate ) 
	{
		_ThreadJob();
		//TA_Base_Core::Thread::sleep(SERVER_LIMIT_THREAD_SLEEP);
	}//while
	
	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "CClientComWorker  end run()");

}



void CClientComWorker::terminate()
{		
	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "CClientComWorker terminate().");
	m_toTerminate = true;		
}



void CClientComWorker::_ThreadJob()
{
	int nFunRes = 0;
	bool bRecvRes = true;
	bool bDataReadly = true;
		
	nFunRes = _SocketRead();
	if (0 != nFunRes)
	{
		//LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "err:read data from socket error!");
		TA_Base_Core::Thread::sleep(SERVER_LIMIT_THREAD_SLEEP);
		nFunRes = -1;
		return;
	}

	_ProcessRecvBuffer();
	_ProcessRcvMsgPackages();	
	_CheckIntValue();
		
	return;
	
}
void CClientComWorker::_CheckIntValue()
{	
	if (m_nLogCount > SERVER_LIMIT_LOG_VLAUE_MAX)
	{
		m_nLogCount	= 0;
	}
}

int CClientComWorker::_ConnectToServer(int nTimeOut)
{
	int nFunRes = 0;
	
	TA_Base_Core::ThreadGuard guardRead(m_SocketReadLock);
	TA_Base_Core::ThreadGuard guardWrite(m_SocketWriteLock);
	TA_Base_Core::ThreadGuard guardInfo(m_SocketInfoLock);


	//init data	 
	DEF_DELETE(m_pSocket);
	m_isConnected = false;
	m_pRecvBufferOnce->clear();
	m_pRecvBufferTotal->clear();
	_FreeLstRecvMsgPackage();

	//create new Socket
	m_pSocket = new TcpSocket(m_strServerIP, m_strServerPort);

	LOG1(SourceInfo, DebugUtil::DebugDebug,	"begin connect to [ServerID=%s]", m_strServerID.c_str());

	//connect to server
	try
	{
		m_pSocket->connect();
		m_isConnected = m_pSocket->stillConnected();
		LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "connect to [ServerID=%s][isConnected=%d] ok!",
			m_strServerID.c_str(), m_isConnected);

		if (m_isConnected && NULL != m_pObserverRcvMsg)
		{
			m_pObserverRcvMsg->processTcpEvent(m_strServerID, TCP_CONNECTED);
		}
		nFunRes = 0;									
	}
	catch (TA_Base_Core::TcpSocketException& e)
	{
		m_isConnected = false;
		if (NULL != m_pObserverRcvMsg)
		{
			m_pObserverRcvMsg->processTcpEvent(m_strServerID, TCP_DISCONNECTED);
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Failed to connect to %s:%s. Error:%s", 
			m_strServerIP.c_str(), m_strServerPort.c_str(), e.what());
		//int WError = WSAGetLastError();
		nFunRes = -1;
	}
	catch (...)
	{
		m_isConnected = false;
		if (NULL != m_pObserverRcvMsg)
		{
			m_pObserverRcvMsg->processTcpEvent(m_strServerID, TCP_DISCONNECTED);
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Failed to connect to %s:%s", 
			m_strServerIP.c_str(), m_strServerPort.c_str());
		nFunRes = -1;
	}
	return nFunRes;
}	





void CClientComWorker::managerStart() 
{
	start();
}

void CClientComWorker::managerTerminateAndWait()
{
	terminateAndWait();

}

int	CClientComWorker::registerObserver(IObserverRcvMsg* Handler)
{
	int nFunRes = 0;
	TA_Base_Core::ThreadGuard guard(m_SocketInfoLock);

	m_pObserverRcvMsg = Handler;
	return nFunRes;
}

int	CClientComWorker::removeObserver()
{
	int nFunRes = 0;
	TA_Base_Core::ThreadGuard guard(m_SocketInfoLock);

	m_pObserverRcvMsg = NULL;
	return nFunRes;
}



int	CClientComWorker::clientSendData(const std::string& strServerID, PMSGPKG pSendMsgPakage)
{
	int nFunRes = 0;
	IMsgPackage* pSendData = NULL;
	if (NULL == pSendMsgPakage)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "MsgPkg handle is NULL");
		nFunRes = -1;
		return nFunRes;	 

	}

	m_pComProtocol->processSendData(pSendMsgPakage, pSendData);	

	_FreeLstMsgItems(pSendMsgPakage->m_lstAllMsgItems);	
	DEF_DELETE(pSendMsgPakage);
	pSendMsgPakage = NULL;

	nFunRes = _SocketWriteMsgPackage(pSendData);  

	DEF_DELETE(pSendData);
	pSendData = NULL;

	return nFunRes;
}


int	CClientComWorker::connectToServer(int nTimeOut)
{
	int nFunRes = 0;
	nFunRes = _ConnectToServer(nTimeOut);
	return nFunRes;
}

 

int CClientComWorker::_FreeLstMsgItem(lstMsgItemConT& lstMsgItem)
{
	int nFunRes = 0;
	lstMsgItemConIterT iterlstMsgItem;
	IMsgItem* pGetMsgItem = NULL;

	iterlstMsgItem = lstMsgItem.begin();
	while (iterlstMsgItem != lstMsgItem.end())
	{
		pGetMsgItem = *iterlstMsgItem;			
		DEF_DELETE(pGetMsgItem);				
		iterlstMsgItem++;
	}
	lstMsgItem.clear();	
	return nFunRes;
}

int CClientComWorker::_FreeLstMsgItems(lstMsgItemsConT& lstMsgItems)
{
	int nFunRes = 0;
	lstMsgItemsConIterT iterLstAllMsgItems;
	lstMsgItemConT* pGetLstMsgItem = NULL;

	iterLstAllMsgItems = lstMsgItems.begin();
	while (iterLstAllMsgItems != lstMsgItems.end())
	{
		pGetLstMsgItem = *iterLstAllMsgItems;
		_FreeLstMsgItem(*pGetLstMsgItem);
		//fix memory problem
		DEF_DELETE(pGetLstMsgItem);
		iterLstAllMsgItems++;
	}

	lstMsgItems.clear();

	return nFunRes;
}
 

int CClientComWorker::_FreeLstRecvMsgPackage()
{
	int nFunRes = 0;  	
	lstMSGPKGConIterT  iterMsgPkg;
	PMSGPKG pEachMsgPkg = NULL;

	if (m_lstRecvMsgPackage.empty())
	{
		nFunRes = 0;
		return nFunRes;
	}

	iterMsgPkg = m_lstRecvMsgPackage.begin();
	while (iterMsgPkg != m_lstRecvMsgPackage.end())
	{
		pEachMsgPkg = *iterMsgPkg;
		nFunRes = _FreeLstMsgItems(pEachMsgPkg->m_lstAllMsgItems);	
		DEF_DELETE(pEachMsgPkg);
		pEachMsgPkg = NULL;

		iterMsgPkg++;
	}
	m_lstRecvMsgPackage.clear();

	return nFunRes;
}


int CClientComWorker::_SocketRead()
{ 
	int					nFunRes = 0;
	unsigned int		nGetLength = 0;
	int					nRcvedRes = -1;		
	bool				bCanRead = false;
	
	TA_Base_Core::ThreadGuard guard(m_SocketReadLock);

	{
		TA_Base_Core::ThreadGuard guard(m_SocketInfoLock);
		if (!m_isConnected)
		{
			//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "client disconnect to server");
			nFunRes = -1;
			return nFunRes;
		}
	}



	//check if have data to read
	bCanRead = m_pSocket->isReadRequired(m_socketTimeout);		
	if (!bCanRead)
	{
		//if bCanRead = false, no data in socket can be readed
		nFunRes = 0;
		return nFunRes;
	} 

	LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[ServerID=%s][isReadRequired() == %d]", 
		m_strServerID.c_str(), bCanRead );

	try
	{  
		m_pRecvBufferOnce->clear();

		nGetLength = 0;
		nRcvedRes = -1;
		nRcvedRes = m_pSocket->read(m_pRecvBufferOnce->getWritePos(), &nGetLength, m_pRecvBufferOnce->getBlankSize()-1);
		m_pRecvBufferOnce->moveWritePos(nGetLength);

		if (nGetLength <= 0 || nRcvedRes != 0)
		{
			TA_Base_Core::ThreadGuard guard(m_SocketInfoLock);
			m_isConnected = false;
			if (NULL != m_pObserverRcvMsg)
			{
				m_pObserverRcvMsg->processTcpEvent(m_strServerID, TCP_DISCONNECTED);
			}
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"err: read data from socket error! [0 != nRcvedRes=%d][nGetLength=%d][ServerID=%s]",
				nRcvedRes, nGetLength, m_strServerID.c_str());
			nFunRes = -1;
		}
		else
		{
			LOG1(SourceInfo, DebugUtil::DebugDebug, "-->client get data from Server [nGetLength=%d]", nGetLength);
			m_pRecvBufferTotal->bufferSizePrepare(m_pRecvBufferOnce->remainDataLength() + BUFFER_LEN_ADDEX);
			m_pRecvBufferTotal->write(m_pRecvBufferOnce->getReadPos(), m_pRecvBufferOnce->remainDataLength());
			m_pRecvBufferOnce->moveReadPos(m_pRecvBufferOnce->remainDataLength());
			nFunRes = 0;
		}


	}
	catch (TA_Base_Core::TcpSocketException& e)
	{	
		TA_Base_Core::ThreadGuard guard(m_SocketInfoLock);
		m_isConnected = false;
		if (NULL != m_pObserverRcvMsg)
		{
			m_pObserverRcvMsg->processTcpEvent(m_strServerID, TCP_DISCONNECTED);
		}

		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
			"err: read data from socket error! TcpSocketException:%s  [ServerID=%s]", e.what(), m_strServerID.c_str());
		nFunRes = -1;	
	}
	catch (...)
	{
		TA_Base_Core::ThreadGuard guard(m_SocketInfoLock);
		m_isConnected = false;
		if (NULL != m_pObserverRcvMsg)
		{
			m_pObserverRcvMsg->processTcpEvent(m_strServerID, TCP_DISCONNECTED);
		}

		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
			"err: read data from socket error! [ServerID=%s]", m_strServerID.c_str());
		nFunRes = -1;	
	}

	return nFunRes;
}


int CClientComWorker::_ProcessRecvBuffer()
{
	TA_Base_Core::ThreadGuard guard(m_SocketReadLock);
	int			nFunRes = 0;
	bool		bHaveOneFrame = false;


	bHaveOneFrame = m_pComProtocol->isBufferHaveOneFrame(m_pRecvBufferTotal);
	if (false == bHaveOneFrame)
	{
		nFunRes = -1;
		return nFunRes;
	}	
	m_pComProtocol->processRecvData(m_pRecvBufferTotal, m_lstRecvMsgPackage);
	//m_pRecvBufferTotal->resizeFitData();
	m_pRecvBufferTotal->trim();	

	if (0 == (m_nLogCount % SERVER_LIMIT_TCP_RecvBuffReset))
	{
		m_pRecvBufferTotal->resizeFitData();
		m_nLogCount = 1;
	}
	m_nLogCount++;

	return nFunRes;
}


int CClientComWorker::_SocketWriteMsgPackage(IMsgPackage* pSendMsgPackage)
{
	int			nFunRes = 0;
	CBuffer*	pSendBuff = NULL;
	int			nSendBuffLen = 0;

	nSendBuffLen =	pSendMsgPackage->getMsgFramePL() + BUFFER_LEN_ADDEX;

	pSendBuff = new CBuffer(nSendBuffLen);

	pSendBuff->write((const char *)pSendMsgPackage->getFrameStartPtr(), pSendMsgPackage->getMsgFramePL());

	nFunRes = _SocketWriteBuffer(pSendBuff);

	DEF_DELETE(pSendBuff);

	return nFunRes;		  
}



int CClientComWorker::_SocketWriteBuffer(CBuffer* pSendBuff)
{
	int		nSendRes = 0;
	char*	pSendPos = NULL;
	int     nSendLen = 0;

	TA_Base_Core::ThreadGuard guard(m_SocketWriteLock);

	{
		TA_Base_Core::ThreadGuard guard(m_SocketInfoLock);
		if (!m_isConnected)
		{
			nSendRes = -1;
			return nSendRes;
		}
	}

	pSendPos = pSendBuff->getReadPos();
	nSendLen = pSendBuff->remainDataLength();

	try
	{		
		nSendRes = m_pSocket->write(pSendPos, nSendLen);
		LOG3(SourceInfo, DebugUtil::DebugDebug, "<--client send data to server [nSendRes=%d][nBufLen=%d][ServerID=%s]",
			nSendRes, nSendLen, m_strServerID.c_str());

		if (nSendRes != nSendLen)
		{
			TA_Base_Core::ThreadGuard guard(m_SocketInfoLock);
			m_isConnected = false;
			if (NULL != m_pObserverRcvMsg)
			{
				m_pObserverRcvMsg->processTcpEvent(m_strServerID, TCP_DISCONNECTED);
			}

			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "error! client send data to server error![nSendRes=%d][nBufLen=%d][ServerID=%s]",
				nSendRes, nSendLen, m_strServerID.c_str()); 
			nSendRes = -1;
		}
		else
		{
			//send data ok, return value is 0
			nSendRes = 0;
		}
	}					
	catch (TA_Base_Core::TcpSocketException& e)
	{
		TA_Base_Core::ThreadGuard guard(m_SocketInfoLock);
		m_isConnected = false;
		if (NULL != m_pObserverRcvMsg)
		{
			m_pObserverRcvMsg->processTcpEvent(m_strServerID, TCP_DISCONNECTED);
		}

		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "error! client send data to server error! TcpSocketException:%s [ServerID=%s]", e.what(), m_strServerID.c_str());
		nSendRes = -1;
	}
	catch (...)
	{
		TA_Base_Core::ThreadGuard guard(m_SocketInfoLock);
		m_isConnected = false;
		if (NULL != m_pObserverRcvMsg)
		{
			m_pObserverRcvMsg->processTcpEvent(m_strServerID, TCP_DISCONNECTED);
		}

		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "error! client send data to server error! [ServerID=%s]", m_strServerID.c_str());
		nSendRes = -1;
	}

	return nSendRes;
}




int CClientComWorker::_ProcessRcvMsgPackages()
{	  	
	TA_Base_Core::ThreadGuard guard(m_SocketReadLock);
	int nFunRes = 0;
	lstMSGPKGConIterT  iterMsgPkg;
	PMSGPKG pEachMsgPkg = NULL;

	if (m_lstRecvMsgPackage.empty())
	{
		nFunRes = -1;
		return nFunRes;
	}

	iterMsgPkg = m_lstRecvMsgPackage.begin();
	pEachMsgPkg = *iterMsgPkg;		

	if (NULL != m_pObserverRcvMsg)
	{
		m_lstRecvMsgPackage.erase(iterMsgPkg);
		m_pObserverRcvMsg->processRcvFrame(m_strServerID, pEachMsgPkg);
	}
	else
	{
		LOG2(SourceInfo, DebugUtil::DebugError, 	
			"m_pServerObserverRcvMsg is NULL,[%s][PkgID=%s] client delete one Frame ", 	
			m_strServerID.c_str(), pEachMsgPkg->m_strPkgUUID32.c_str());

		//erase this iter from list
		m_lstRecvMsgPackage.erase(iterMsgPkg);

		//m_pServerObserverRcvMsg is NULL then free this  MsgPkg
		nFunRes = _FreeLstMsgItems(pEachMsgPkg->m_lstAllMsgItems);	
		DEF_DELETE(pEachMsgPkg);
		pEachMsgPkg = NULL;
		  
		pEachMsgPkg = NULL;	 

	}

	return nFunRes;
}
 
NS_END(TA_DB_Sync)





