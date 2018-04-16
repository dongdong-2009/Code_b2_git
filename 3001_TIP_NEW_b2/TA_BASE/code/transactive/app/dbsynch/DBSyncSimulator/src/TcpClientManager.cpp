#include "TcpClientManager.h"

#include "core/sockets/src/TcpServerSocket.h"
#include "core/sockets/src/TcpSocket.h"
#include "core/sockets/src/SocketSet.h"

#include "core/utilities/src/DebugUtil.h"

#include "CommonDef.h"
#include "CommonData.h"
#include "UtilityFun.h"
			
#include "TcpClientWrapper.h"


using TA_Base_Core::DebugUtil;

NS_BEGIN(TA_DB_Sync)


	
CTcpClientManager* CTcpClientManager::m_pInstance = 0;
NonReEntrantThreadLockable CTcpClientManager::m_instanceLock;

CTcpClientManager& CTcpClientManager::getInstance()
{
	FUNCTION_ENTRY("CTcpClientManager::GetInstance()");
	ThreadGuard guard( m_instanceLock );
	if (NULL == m_pInstance)
	{
		m_pInstance = new CTcpClientManager();
	}

	FUNCTION_EXIT;
	return (*m_pInstance);
}

void CTcpClientManager::removeInstance()
{
	DEF_DELETE(m_pInstance);
}


CTcpClientManager::CTcpClientManager()
{	
   m_pTcpClientWrapper = NULL;
   m_pTcpClientWrapper = new CTcpClientWrapper();
	
}

CTcpClientManager::~CTcpClientManager()
{
	if (NULL != m_pTcpClientWrapper)
	{
		DEF_DELETE(m_pTcpClientWrapper);
	} 		

}



std::string CTcpClientManager::createNewTcpClient( const std::string& strServerIp, const std::string& strServerPort, const std::string& strQName )
{
	std::string	 strServerID;
	strServerID = m_pTcpClientWrapper->createNewTcpClient(strServerIp, strServerPort, strQName);
	return strServerID;
}





int CTcpClientManager::removeTcpClient( const std::string& strServerID )
{
	int nFunRes = 0;
	nFunRes =	m_pTcpClientWrapper->removeTcpClient(strServerID);
	return nFunRes;
}



int CTcpClientManager::sendDataToServer( const std::string& strServerID, PMSGPKG pSendMsgPakage )
{
	int nFunRes = 0;
	nFunRes =	m_pTcpClientWrapper->sendDataToServer(strServerID, pSendMsgPakage);
	return nFunRes;
}


int CTcpClientManager::registerObserver(IObserverRcvMsg* Handler )
{
	int nFunRes = 0;
	nFunRes =	m_pTcpClientWrapper->registerObserver(Handler);

	return nFunRes;
}

int CTcpClientManager::connectToServer( const std::string& strServerID )
{
	int nFunRes = 0;
	nFunRes =	m_pTcpClientWrapper->connectToServer(strServerID);

	return nFunRes;
}




NS_END(TA_DB_Sync)








