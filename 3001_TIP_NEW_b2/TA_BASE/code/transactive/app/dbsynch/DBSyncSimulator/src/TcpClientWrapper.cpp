#include "TcpClientWrapper.h"


#include "core/sockets/src/TcpServerSocket.h"
#include "core/sockets/src/TcpSocket.h"
#include "core/sockets/src/SocketSet.h"

#include "core/utilities/src/DebugUtil.h"

#include "CommonData.h"
#include "CommonDef.h"

#include "UtilityFun.h"

#include "ITcpComType.h"
#include "ClientComWorker.h"


using TA_Base_Core::DebugUtil;

NS_BEGIN(TA_DB_Sync)



CTcpClientWrapper::CTcpClientWrapper()
{
	m_mapAllClients.clear();
	m_pDispatchMsgHandle = NULL;
}

CTcpClientWrapper::~CTcpClientWrapper()
{
	vecClientsManagerConIterT iterMap;
	ITcpComType* pGetClient = NULL;

	iterMap = m_mapAllClients.begin();
	while (m_mapAllClients.end() != iterMap)
	{
		pGetClient = iterMap->second;
		
		//first terminate client thread
		//then removeObserver
		pGetClient->managerTerminateAndWait(); 
		pGetClient->removeObserver();
		DEF_DELETE(pGetClient);
		
		iterMap++;

	} //while
	m_mapAllClients.clear();
	
	m_pDispatchMsgHandle = NULL;

}



std::string CTcpClientWrapper::createNewTcpClient(const std::string& strServerIp, const std::string& strServerPort, const std::string& strQName)
{
	TA_Base_Core::ThreadGuard guard(m_AllClientsMapLock);

	std::string strServerID;
	ITcpComType *pNewClient = NULL;
	vecClientsManagerConIterT iterFind;
	strServerID = _GetServerID(strServerIp, strServerPort, strQName); 
   	
	//duplicate check
	iterFind =	 m_mapAllClients.find(strServerID);
	if (iterFind == m_mapAllClients.end())
	{
		pNewClient = new CClientComWorker(strServerIp, strServerPort, strQName, strServerID);
		m_mapAllClients.insert(vecClientsManagerValueType(strServerID, pNewClient));
		if (NULL != m_pDispatchMsgHandle)
		{
			pNewClient->registerObserver(m_pDispatchMsgHandle);
		}
		pNewClient->managerStart();

	}
	else
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
			"ServerID:%s is already in clients map", strServerID.c_str());
		strServerID.clear();		
	}
	return strServerID;
}




int CTcpClientWrapper::removeTcpClient( const std::string& strServerID )
{
	TA_Base_Core::ThreadGuard guard(m_AllClientsMapLock);

	int nFunRes = 0;

	vecClientsManagerConIterT iterMap;
	ITcpComType* pGetClient = NULL;

	std::string strInfo = strServerID;

	iterMap = m_mapAllClients.find(strInfo);
	if (m_mapAllClients.end() != iterMap)
	{
		pGetClient = iterMap->second;

		pGetClient->managerTerminateAndWait();
		DEF_DELETE(pGetClient);

		m_mapAllClients.erase(iterMap);
		nFunRes = 0;
	}
	else
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "removeNewTcpClient() not find %s", strInfo.c_str());
		nFunRes = -1;
	}


	return nFunRes;
}




int CTcpClientWrapper::_FreeLstMsgItem(lstMsgItemConT& lstMsgItem)
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

int CTcpClientWrapper::_FreeLstMsgItems(lstMsgItemsConT& lstMsgItems)
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

int CTcpClientWrapper::sendDataToServer( const std::string& strServerID, PMSGPKG pSendMsgPakage )
{
	int nFunRes = 0;
	vecClientsManagerConIterT iterMap;
	ITcpComType* pGetClient = NULL;

	//find
	iterMap = m_mapAllClients.find(strServerID);
	if (m_mapAllClients.end() != iterMap)
	{
		pGetClient = iterMap->second;

		nFunRes = pGetClient->clientSendData(strServerID, pSendMsgPakage);

	}
	else
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "SendDataToServer() not find [ServerID=%s] delete this frame [PkgID=%s]", 
			strServerID.c_str(), pSendMsgPakage->m_strPkgUUID32.c_str());

		_FreeLstMsgItems(pSendMsgPakage->m_lstAllMsgItems);	
		DEF_DELETE(pSendMsgPakage);
		pSendMsgPakage = NULL;

				
		nFunRes = -1;
	}
	
	//send
	
	


	return nFunRes;
}


int CTcpClientWrapper::registerObserver(IObserverRcvMsg* Handler )
{
	int nFunRes = 0; 
	vecClientsManagerConIterT iterMap;
	ITcpComType* pGetClient = NULL;

	if (NULL == Handler)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
				"registerObserver() Handler == NULL");

		nFunRes = -1;
		return nFunRes;
	} 	
	m_pDispatchMsgHandle = Handler;		

	iterMap = m_mapAllClients.begin();
	while (iterMap != m_mapAllClients.end())
	{
		pGetClient = iterMap->second; 
		nFunRes = pGetClient->registerObserver(m_pDispatchMsgHandle);
		iterMap++;
	} 
	return nFunRes;
}


std::string CTcpClientWrapper::_GetServerID( const std::string& strServerIp, const std::string& strServerPort, const std::string& strQName )
{
	std::string strInfo = "";
	strInfo = strServerIp + ":" + strServerPort + ":" + strQName;
	return strInfo;
}

int CTcpClientWrapper::connectToServer( const std::string& strServerID )
{
	int nFunRes = 0;  
	vecClientsManagerConIterT iterMap;
	ITcpComType* pGetClient = NULL;

	std::string strInfo = strServerID;

	iterMap = m_mapAllClients.find(strInfo);
	if (m_mapAllClients.end() != iterMap)
	{
		pGetClient = iterMap->second; 
		nFunRes = pGetClient->connectToServer(0);
	}
	else
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "SendDataToServer() not find %s", strInfo.c_str());
		nFunRes = -1;
	}


	return nFunRes;
}



//}//namespace 


NS_END(TA_DB_Sync)













