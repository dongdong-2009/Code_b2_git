#include "TcpConnectionPool.h"
#include "CommonDef.h"	
#include "CommonData.h"	 
#include "UtilityFun.h"
#include "core/utilities/src/DebugUtil.h"

#include "TcpConnectionActor.h"
#include "ConnectionActorReleaser.h"
NS_BEGIN(TA_DB_Sync)

using  TA_Base_Core::DebugUtil;

ReEntrantThreadLockable CTcpConnectionPool::m_singletonLock;
CTcpConnectionPool* CTcpConnectionPool::m_theClassInstance = NULL;

CTcpConnectionPool* CTcpConnectionPool::getInstance()
{
	TA_Base_Core::ThreadGuard guard(m_singletonLock);

	// If guard acquired then make sure the singleton is still NULL
	if ( m_theClassInstance == NULL )
	{
		// Create the one & only object
		m_theClassInstance = new CTcpConnectionPool();
	}

	return  m_theClassInstance;
}


CTcpConnectionPool::CTcpConnectionPool()
{	
	m_toTerminate = false;
	m_nMaxClientNum = SERVER_LIMIT_CLIENT_COUNT_MAX;	
	m_nCurrentClientNum = 0;
	m_sequenceID=0;
}


CTcpConnectionPool::~CTcpConnectionPool()
{
	_ClearAll();
}


void CTcpConnectionPool::_ClearAll()
{
	_ClearAllTcpSocket();
	_ClearAllConnectedClients();
}


void CTcpConnectionPool::_ClearAllConnectedClients()
{
	TA_Base_Core::ThreadGuard	guard(m_lockSetAllClients);
	vecConPoolManagerConIterT   iterSet;
	TcpConnectionActor*			pClientGet = NULL;
	std::string					strClientID;

	iterSet = m_AllConnectedClients.begin();
	while (iterSet != m_AllConnectedClients.end())
	{
		pClientGet = iterSet->second;
		m_AllConnectedClients.erase(iterSet);

		strClientID = pClientGet->getClientID();
		pClientGet->serverstopWrok(CurrentClientDestroy_Normal);
		DEF_DELETE(pClientGet);

		m_nCurrentClientNum--;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"after free one server [CurrentClientNum=%d][strClientID=%s]", 
			m_nCurrentClientNum, strClientID.c_str()); 


		iterSet = m_AllConnectedClients.begin();
	}
	m_AllConnectedClients.clear();
}


void CTcpConnectionPool::_ClearAllTcpSocket()
{
	TA_Base_Core::ThreadGuard	guard(m_lockSetAllClients);
	vecTcpSocketConIterT			iterVec;
	TcpSocket*					pClientGet = NULL;

	iterVec = m_AllNewTCPClients.begin();
	while (iterVec != m_AllNewTCPClients.end())
	{
		pClientGet = *iterVec;
		m_AllNewTCPClients.erase(pClientGet);

		DEF_DELETE(pClientGet);
		iterVec = m_AllNewTCPClients.begin();
	}
	m_AllNewTCPClients.clear();
}



void CTcpConnectionPool::run()
{  
	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "CTcpConnectionPool  begin run()");

	while( false == m_toTerminate ) 
	{
		_ThreadJob();
		TA_Base_Core::Thread::sleep( SERVER_LIMIT_THREAD_SLEEP );
	}//while
	_ClearAll();
	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "CTcpConnectionPool  end run()");

}


void CTcpConnectionPool::terminate()
{
	m_toTerminate = true;
	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
		"CTcpConnectionPool::terminate()");
}


void CTcpConnectionPool::_ThreadJob()
{
	//change TCP socket into TCPConnectionActor
	_JobAcceptNewClient();	 

	//release TCPConnectionActor if its TCP connection is lost.
	_JobCheckConnectedClients();
}



void CTcpConnectionPool::_JobCheckConnectedClients()
{
	vecConPoolManagerConT     unHealthClients;
	vecConPoolManagerConIterT   iterSet;
	TcpConnectionActor*			pClientGet = NULL;
	std::string                 strClientID;
	{ //fix dead lock problem	
		TA_Base_Core::ThreadGuard guard(m_lockSetAllClients);	
		iterSet = m_AllConnectedClients.begin();

		while (iterSet != m_AllConnectedClients.end())
		{
			pClientGet = iterSet->second;	 
			if (pClientGet !=NULL && false == pClientGet->isHealth())
			{
				m_AllConnectedClients.erase(iterSet); 
				m_nCurrentClientNum--;  //why we need this count?
				unHealthClients[pClientGet->getID()]=pClientGet;		
				iterSet = m_AllConnectedClients.begin();
			}		 		 
			else if( pClientGet ==NULL )
			{
				m_AllConnectedClients.erase(iterSet);   //a= map[key], if map does not include an object with key 'key', map will insert an empty object with key 'key'
				iterSet = m_AllConnectedClients.begin();			 
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "unHealth,empty action,[ClientID=%s][ClientNum=%d]", 
					strClientID.c_str(), m_nCurrentClientNum);
			}
			else
			{
				iterSet++;
			}

		}//	while (iter_set)
	}


	//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "remove unHealth");

	iterSet = unHealthClients.begin();
	while (iterSet != unHealthClients.end())
	{
		pClientGet = iterSet->second;	
		ConnectionActorReleaser::getInstance()->push(pClientGet);
		iterSet++;		 

	}//	while (iter_set)
	unHealthClients.clear();


}

bool CTcpConnectionPool::poolAcceptOneNewClient( TcpSocket* pNewClient)
{
	TA_Base_Core::ThreadGuard guard(m_lockSetAllClients);
	bool bFunRes = true;

	m_AllNewTCPClients.insert(pNewClient);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Number of New Client's Request= %d",m_AllNewTCPClients.size());
	return bFunRes;					   	
}


bool CTcpConnectionPool::_JobAcceptNewClient()
{
	TA_Base_Core::ThreadGuard guard(m_lockSetAllClients);
	bool bFunRes = true;
	std::string SrcDbCon;
	TcpSocket* pNewClient = NULL;
	vecTcpSocketConIterT iterSet;

	iterSet = m_AllNewTCPClients.begin();
	if (iterSet == m_AllNewTCPClients.end())
	{
		bFunRes = false;
		return bFunRes;
	}
	pNewClient = *iterSet;
	m_AllNewTCPClients.erase(iterSet);

	if (!pNewClient->stillConnected())
	{

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "new,lost client connection");		 
		pNewClient->close(); //ok,  no other thread access this socket,so it can be closed 
		DEF_DELETE(pNewClient);
		bFunRes = false;
		return bFunRes;
	}

	if (m_AllConnectedClients.size() > m_nMaxClientNum)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "new,Arrived maximum allowable count of client ");	
		pNewClient->close();  
		DEF_DELETE(pNewClient);
		bFunRes = false;
		return bFunRes;

	}

	SrcDbCon = g_system_variables.m_SrcDbConStr;	
		   
	bFunRes = _AcceptNewClient(pNewClient);
			  
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
		"new,[AllNewTCPClients.size()=%d][SrcDbCon=%s][DBType=%d]", 
		m_AllNewTCPClients.size(), SrcDbCon.c_str(), g_system_variables.m_nDBType);

	return bFunRes;
}


bool CTcpConnectionPool::_AcceptNewClient( TcpSocket* pNewClient)
{ 
	bool						bFunRes = true;
	TcpConnectionActor*			pNeedCheckClient = NULL;
	int                         nCheckRes =	 0;

	if (NULL == pNewClient)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "new,NULL == pNewClient");
		bFunRes = false;
		return bFunRes;
	}	
	unsigned long id =_getNextActorID();
	pNeedCheckClient = new TcpConnectionActor(pNewClient,id);	
	m_AllConnectedClients[id]= pNeedCheckClient;
	pNeedCheckClient->serverStartComWork();	//start one TCP communication thread     
	m_nCurrentClientNum++;

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Connection clients = %d", m_AllConnectedClients.size());

	return bFunRes;
}




void CTcpConnectionPool::releaseInvaildAction(unsigned long newActionID)
{
	TA_Base_Core::ThreadGuard guard(m_lockSetAllClients); 


	TcpConnectionActor* pNewClient=NULL;

	vecConPoolManagerConIterT  iter=m_AllConnectedClients.find(newActionID);

	if( iter != m_AllConnectedClients.end() )
	{
		pNewClient=iter->second;
	}

	std::list<TcpConnectionActor*>  rlist;
	if (pNewClient==NULL)  //it may happen if connection is lost and TCPConnectionPool releases it.
	{
		return;
	}

	std::string					strClientIDCheck;
	std::string					strClientAgtName;
	vecConPoolManagerConIterT   iterSet;
	TcpConnectionActor			*pClientGet = NULL;
	TcpConnectionActor			*pCreateNewClient = NULL; 	
	std::string					strClientIDGet;
	std::string					strClientAgtNameGet;

	//value eg.: AgtName_QName_IP 
	strClientIDCheck = pNewClient->getClientID();
	strClientAgtName = pNewClient->getClientAgentName();


	/*find and delete old */
	iterSet = m_AllConnectedClients.begin();
	while (iterSet != m_AllConnectedClients.end())
	{
		pClientGet = iterSet->second;		 
		if (pClientGet !=NULL)  
		{	 
			strClientIDGet = pClientGet->getClientID();
			strClientAgtNameGet = pClientGet->getClientAgentName();	

			if (strClientIDCheck == strClientIDGet &&  pClientGet->getID() != newActionID)
			{			 
				LOG5(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					"found old server [CurrentClientNum=%d][ClientIDGet=%s][ClientAgtNameGet=%s][oldActionID=%d][newActionID=%d]",
					m_nCurrentClientNum, strClientIDGet.c_str(), strClientAgtNameGet.c_str(), pClientGet->getID(), newActionID); 
				rlist.push_back(pClientGet);
				pClientGet = NULL;	
			}	

		}
		iterSet++;

	}//	while (iter_set)


	std::list<TcpConnectionActor*>::iterator iterRemove=rlist.begin();

	while (iterRemove != rlist.end())
	{
		pClientGet = *iterRemove;
		m_AllConnectedClients.erase(pClientGet->getID());
		ConnectionActorReleaser::getInstance()->push(pClientGet);
		iterRemove++;
	}
	rlist.clear();

}



unsigned long CTcpConnectionPool::_getNextActorID()
{
	ThreadGuard guard(m_seqIDLock);
	m_sequenceID++;

	if( m_sequenceID == ULONG_MAX )
	{
		m_sequenceID = 1;
	}

	return m_sequenceID;
}

//Dequeue meet problem, need to release TcpConnectionActor
void  CTcpConnectionPool::setDequeueStatusBad(unsigned long  actorID)
{ 
	TA_Base_Core::ThreadGuard guard(m_lockSetAllClients); 
	TcpConnectionActor*			actor = NULL;  

	vecConPoolManagerConIterT  iter = m_AllConnectedClients.find(actorID);

	if( iter != m_AllConnectedClients.end() )
	{
		actor=iter->second;
		if (actor !=NULL) 
		{
			actor->setDequeueStatusBad(true);
		}	
	}


}

//Enqueue meet problem, need to release TcpConnectionActor
void    CTcpConnectionPool::setEnqueueStatusBad(unsigned long  actorID)
{
	TA_Base_Core::ThreadGuard guard(m_lockSetAllClients); 
	TcpConnectionActor*			actor = NULL; 		 

	vecConPoolManagerConIterT  iter=m_AllConnectedClients.find(actorID);

	if( iter != m_AllConnectedClients.end() )
	{
		actor=iter->second;
		if (actor !=NULL) 
		{
			actor->setEnqueueStatusBad(true);
		}	
	}

}

NS_END(TA_DB_Sync)








