#include "UtilityFun.h"
#include "dbSynTestClientManager.h"
#include "MsgHelper.h"

#include <signal.h>

#include "UniqueCheck.h"
#include "SyncAQMsgManager.h" 
#include "core/utilities/src/RunParams.h"

#include "CommonData.h"
#include "CommonDef.h"
#include "TcpClientWrapper.h"
#include "MsgHelper.h"
#include "UtilityFun.h"




#include "PropagateMsg.h"
#include "CommonData.h"
#include "UtilityFun.h"
#include "DatabaseImpl.h"
#include "Listener.h"
#include "ConsumerImpl.h"
#include "Message.h"
#include "MsgHelper.h"
#include "IObserverRcvMsg.h"
#include "ProcessRcvMsg.h"
#include "TimeMonitor.h"
#include "DeqMsgJob.h"

#include "IDatabase.h"
#include "IDbData.h"
#include "DatabaseImpl.h"

#include "TcpClientManager.h"
#include "DBException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DbSyncException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DbConnectionFailed.h"
#include "core/uuid/src/TAuuid.h"

#include "MsgDispatcher.h"

#include "core/utilities/src/DebugUtil.h"


//#include "core/utilities/src/DebugUtil.h"
using TA_Base_Core::DebugUtil;


NS_BEGIN(TA_DB_Sync)


CDbSynTestCreateMsgManager::CDbSynTestCreateMsgManager()
{	
	m_nClientIndex = 0;
	m_ulSeqIDFrom = 0;
	m_ulSeqIDTo = 0;
	m_nSRVID = 0;
	m_nIntervalTimeofInsert = 0;
	m_nIntervalTimeofGroup = 0;
	m_nIntervalTimeofPublic = 0;
	m_nIntervalTimeofMMS = 0;
	m_nNumberofInsert = 0;
	m_nNumberofGroup = 0;
	m_nNumberofPublic = 0;
	m_nNumberofMMS = 0;
	m_nPackageSize = 0;
	m_ulSeqIDCurrent = 0;
	m_bSeqIDCurrentHaveBeenGet = false;
	
    m_pstEnqReqTotal = NULL;
    m_pMsgItemHelper = NULL;
	m_pCreateInsertEnqMsgThread = NULL;
	m_pCreateGroupEnqMsgThread = NULL;
	m_pCreatePublicEnqMsgThread= NULL;
	m_pCreateMMSEnqMsgThread = NULL;
	m_pMsgItemHelper = new CMsgItemHelper();
	
	m_nManagerWorkState = CreateMsgManagerWorkState_StartWork;



}


CDbSynTestCreateMsgManager::~CDbSynTestCreateMsgManager()
{
	DEF_DELETE(m_pMsgItemHelper);
	stopCreateMsg();
	if (NULL != m_pstEnqReqTotal)
	{
		_FreeEnqReqEx(&m_pstEnqReqTotal);
	}
	m_nManagerWorkState = CreateMsgManagerWorkState_FinishWork;
}



//////////////////////////////////////////////////////////////////////////
int CDbSynTestCreateMsgManager::setAllParam()
{
	int nFunRes = 0;

	//check
	if (m_strAgtName.empty())
	{
		nFunRes = -1;
		return nFunRes;
	}
	if (m_nClientIndex < 0)
	{
		nFunRes = -1;
		return nFunRes;
	}


	m_strSrvIP = g_CfgSimInfo.m_strCfg_ServerIP;
	m_strSrvPort = g_CfgSimInfo.m_strCfg_ServerPort;
	m_strDbConStr = g_CfgSimInfo.m_strCfg_SrcDbConStr;
	m_strQName = g_CfgSimInfo.m_strCfg_QueueName;
	m_ulSeqIDFrom = g_SynProgramSimInfo.m_n_StartSeqNum;
	m_ulSeqIDTo = g_SynProgramSimInfo.m_n_EndSeqNum;

	m_nNumberofInsert = g_SynProgramSimInfo.m_n_NumberofInsert;
	m_nNumberofGroup = g_SynProgramSimInfo.m_n_NumberofGroup;
	m_nNumberofPublic = g_SynProgramSimInfo.m_n_NumberofPublic;
	m_nNumberofMMS = g_SynProgramSimInfo.m_n_NumberofMMS;

	m_nIntervalTimeofInsert = g_SynProgramSimInfo.m_n_IntervalTimeofInsert;
	m_nIntervalTimeofGroup = g_SynProgramSimInfo.m_n_IntervalTimeofGroup;
	m_nIntervalTimeofPublic = g_SynProgramSimInfo.m_n_IntervalTimeofPublic;
	m_nIntervalTimeofMMS = g_SynProgramSimInfo.m_n_IntervalTimeofMMS;

	m_szAuditDataOracle = g_SynProgramSimInfo.m_szFormat_AuditDataOracle;
	m_szAuditDataMysql = g_SynProgramSimInfo.m_szFormat_AuditDataMysql;
	m_nSRVID = g_SynProgramSimInfo.m_n_Default_SRVID;
	m_nPackageSize = g_SynProgramSimInfo.m_n_PackageSize;

	ThreadGuard guard(m_lockULSeqIDCurrent);
	m_ulSeqIDCurrent =   g_SynProgramSimInfo.m_n_StartSeqNum;
	m_bSeqIDCurrentHaveBeenGet = false;

	ThreadGuard guardData(m_lockENQREQEXTotal);
	
	if (NULL != m_pstEnqReqTotal)
	{
		_FreeEnqReqEx(&m_pstEnqReqTotal);
	}
	m_pstEnqReqTotal = NULL;
	m_pstEnqReqTotal = new ENQREQEX();
	m_pstEnqReqTotal->m_strPkgID.clear();
	m_pstEnqReqTotal->m_lstDBMsg.clear();


	m_nManagerWorkState = CreateMsgManagerWorkState_InitCTOSComInitMsg_Param;
	m_nManagerWorkState = CreateMsgManagerWorkState_InitCTOSEnqMsg_Param;
	return nFunRes;
}
int CDbSynTestCreateMsgManager::startCreateMsg()
{
	int nFunRes = 0; 
	m_nManagerWorkState = CreateMsgManagerWorkState_StartCreateMsg;


	m_nNumberofInsert = g_SynProgramSimInfo.m_n_NumberofInsert;
	m_nNumberofGroup = g_SynProgramSimInfo.m_n_NumberofGroup;
	m_nNumberofPublic = g_SynProgramSimInfo.m_n_NumberofPublic;
	m_nNumberofMMS = g_SynProgramSimInfo.m_n_NumberofMMS;


// 	if (NULL == m_pCreateInitMsg)
// 	{
// 		m_pCreateInitMsg =new CDbSynTestCreateComInitMsg();
// 		//init
// 		m_pCreateInitMsg->m_strAgtName = m_strAgtName;
// 		m_pCreateInitMsg->m_strQName = m_strQName;
// 		m_pCreateInitMsg->m_nSRVID = m_nSRVID;
// 		m_pCreateInitMsg->m_strHostName = "SynSimblator_TEST_HOSTNAME";
// 	}



	if (m_nNumberofInsert > 0)
	{
		if (NULL == m_pCreateInsertEnqMsgThread)
		{
			m_pCreateInsertEnqMsgThread =new CDbSynTestCreateEnqMsg();
			//init

			m_pCreateInsertEnqMsgThread->m_strAgtName = m_strAgtName;
			m_pCreateInsertEnqMsgThread->m_nClientIndex = m_nClientIndex;
			m_pCreateInsertEnqMsgThread->m_strMsgQFierSub = DEF_STRING_MSGQF_INSERTTYPE;
			m_pCreateInsertEnqMsgThread->m_nEnqMsgType = SynEnqMsgMsgType_INSERT;

			m_pCreateInsertEnqMsgThread->m_pManagerCopy = this;

			//
			m_pCreateInsertEnqMsgThread->start();
		}
	}

	if (m_nNumberofGroup > 0)
	{
		if (NULL == m_pCreateGroupEnqMsgThread)
		{
			m_pCreateGroupEnqMsgThread =new CDbSynTestCreateEnqMsg();
			//init
			m_pCreateGroupEnqMsgThread->m_strAgtName = m_strAgtName;
			m_pCreateGroupEnqMsgThread->m_nClientIndex = m_nClientIndex;
			m_pCreateGroupEnqMsgThread->m_strMsgQFierSub = DEF_STRING_MSGQF_GROUPTYPE;
			m_pCreateGroupEnqMsgThread->m_nEnqMsgType = SynEnqMsgMsgType_GROUP;

			m_pCreateGroupEnqMsgThread->m_pManagerCopy = this;

			//
			m_pCreateGroupEnqMsgThread->start();

		}
	}


	if (m_nNumberofPublic > 0)
	{
		if (NULL == m_pCreatePublicEnqMsgThread)
		{
			m_pCreatePublicEnqMsgThread =new CDbSynTestCreateEnqMsg();
			//init
			m_pCreatePublicEnqMsgThread->m_strAgtName = m_strAgtName;
			m_pCreatePublicEnqMsgThread->m_nClientIndex = m_nClientIndex;
			m_pCreatePublicEnqMsgThread->m_strMsgQFierSub = DEF_STRING_MSGQF_PUBLICTYPE;
			m_pCreatePublicEnqMsgThread->m_nEnqMsgType = SynEnqMsgMsgType_PUBLIC;

			m_pCreatePublicEnqMsgThread->m_pManagerCopy = this;

			//
			m_pCreatePublicEnqMsgThread->start();

		}
	}
	
	if (m_nNumberofMMS > 0)
	{
		if (NULL == m_pCreateMMSEnqMsgThread)
		{
			m_pCreateMMSEnqMsgThread =new CDbSynTestCreateEnqMsg();
			//init
			m_pCreateMMSEnqMsgThread->m_strAgtName = m_strAgtName;
			m_pCreateMMSEnqMsgThread->m_nClientIndex = m_nClientIndex;
			m_pCreateMMSEnqMsgThread->m_strMsgQFierSub = DEF_STRING_MSGQF_MMS;
			m_pCreateMMSEnqMsgThread->m_nEnqMsgType = SynEnqMsgMsgType_MMS;

			m_pCreateMMSEnqMsgThread->m_pManagerCopy = this;

			//
			m_pCreateMMSEnqMsgThread->start();

		}
	}



	m_nManagerWorkState = CreateMsgManagerWorkState_CreateingMsg;

	return nFunRes;
}


int CDbSynTestCreateMsgManager::stopCreateMsg()
{
	int nFunRes = 0;

// 	if (NULL != m_pCreateInitMsg)
// 	{
// 		DEF_DELETE(m_pCreateInitMsg);
// 	}

	if (NULL != m_pCreateInsertEnqMsgThread)
	{
		m_pCreateInsertEnqMsgThread->terminateAndWait();
		DEF_DELETE(m_pCreateInsertEnqMsgThread);
	}

	if (NULL != m_pCreateGroupEnqMsgThread)
	{
		m_pCreateGroupEnqMsgThread->terminateAndWait();

		DEF_DELETE(m_pCreateGroupEnqMsgThread);
	}

	if (NULL != m_pCreatePublicEnqMsgThread)
	{
		m_pCreatePublicEnqMsgThread->terminateAndWait();
		DEF_DELETE(m_pCreatePublicEnqMsgThread);
	}
	
	//fix error! not free MMSEnqMsgThread
	if (NULL != m_pCreateMMSEnqMsgThread)
	{
		m_pCreateMMSEnqMsgThread->terminateAndWait();
		DEF_DELETE(m_pCreateMMSEnqMsgThread);
	}


	m_nManagerWorkState = CreateMsgManagerWorkState_StopCreateMsg;
	return nFunRes;
}
int CDbSynTestCreateMsgManager::getMsgCTOSConReq(MSGPKG** ppOUTMsgPkg)
{
	int nFunRes = 0; 

	ENQREQEX* pEnqReqTotal = NULL;
	ENQREQEX* pEnqReqForMsgPkg = NULL;
	MSGPKG* pOneMsgPkg = NULL;

	std::string strUUID36;
	std::string strUUID32;
	std::string strReqPkgID;



	if (NULL == ppOUTMsgPkg)
	{
		nFunRes = -1;
		return nFunRes;
	}

	if (NULL != *ppOUTMsgPkg)
	{
		nFunRes = -1;
		return nFunRes;
	}


	if (CreateMsgManagerWorkState_CreateingMsg != m_nManagerWorkState)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"[m_nClientIndex=%d][m_strServerID=%s]getMsgCTOSConReq()CreateMsgManagerWorkState_CreateingMsg != m_nManagerWorkState", 
			m_nClientIndex, m_strServerID.c_str());
		nFunRes = -1;
		return nFunRes;
	}


	//nFunRes = m_pCreateInitMsg->getMsgCTOSConReq(ppOUTMsgPkg);

	if (0 != nFunRes)
	{
		pEnqReqTotal = NULL;
		nFunRes = -1;
		return nFunRes;
	}
	if (NULL == *ppOUTMsgPkg)
	{
		nFunRes = -1;
		return nFunRes;
	}



	return nFunRes;
}


int CDbSynTestCreateMsgManager::_GetOnePackageFromTotalLst(
	ENQREQEX* pINTotalMsg, int nINPackageSize, ENQREQEX** ppOUTPackage,
	ulong64& ulSeqIDFrom, ulong64& ulSeqIDTo)
{ 
	int nFunRes = 0;

	lstDBMsgConIterT  iterLst;
	TA_AQ_Lib::CMessage* pGet = NULL;
	unsigned int nDataLen = 0;
	ulong64 nIndex = 0;
	ENQREQEX* pPackageTmp = NULL;//*ppOUTPackage;

	if (NULL == pINTotalMsg || NULL == ppOUTPackage || nINPackageSize <= 0)
	{
		nFunRes = -1;
		return nFunRes;
	}

	if (NULL == *ppOUTPackage)
	{
		*ppOUTPackage = new ENQREQEX();
	}
	pPackageTmp = *ppOUTPackage;

	nDataLen = 0;
	iterLst = pINTotalMsg->m_lstDBMsg.begin();
	pGet = NULL;
	ulSeqIDFrom = 0;
	ulSeqIDTo = 0;
	nIndex = 0;
	while (iterLst != pINTotalMsg->m_lstDBMsg.end() )
	{
		pGet = *iterLst;

		pPackageTmp->m_lstDBMsg.push_back(pGet);
		nIndex++;

		pINTotalMsg->m_lstDBMsg.erase(iterLst);
		iterLst = pINTotalMsg->m_lstDBMsg.begin();

		nDataLen +=	 pGet->getPayloadSize();

		if (1 == nIndex)
		{
			ulSeqIDFrom = pGet->getMsgSeqID();
		}

		if (nDataLen >= nINPackageSize)
		{
			break;
		}
	}

	if (NULL != pGet)
	{
		ulSeqIDTo = pGet->getMsgSeqID();
		pGet = NULL;
	}

	pPackageTmp = NULL;

	return nFunRes;

}

int CDbSynTestCreateMsgManager::getMsgCTOSEnqReq(ENQREQEX** ppOOUTEnqReq, ulong64& ulSeqIDFrom, ulong64& ulSeqIDTo)
{
	int nFunRes = 0;

	std::string strUUID36;
	std::string strUUID32;
	std::string strReqPkgID; 

	if (NULL == ppOOUTEnqReq )
	{
		int nFunRes = -1;
		return nFunRes;
	}
	if (NULL == *ppOOUTEnqReq)
	{
		*ppOOUTEnqReq = new ENQREQEX();
	}



	if (CreateMsgManagerWorkState_CreateingMsg != m_nManagerWorkState)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"[m_nClientIndex=%d][m_strServerID=%s]getMsgCTOSEnqReq()CreateMsgManagerWorkState_CreateingMsg != m_nManagerWorkState", 
			m_nClientIndex, m_strServerID.c_str());
		nFunRes = -1;
		return nFunRes;
	}


	//m_nPackageSize = g_SynProgramSimInfo.m_n_PackageSize;
	_GetDataFromTotalList(m_nPackageSize, ppOOUTEnqReq, ulSeqIDFrom, ulSeqIDTo);
	
	
	

	if (NULL == *ppOOUTEnqReq || true == (*ppOOUTEnqReq)->m_lstDBMsg.empty())
	{
		nFunRes = -1;
		return nFunRes;
	}



	return nFunRes;
}

//////////////////////////////////////////////////////////////////////////


int CDbSynTestCreateMsgManager::getLockSeqIDCurrent(ulong64& ulOUTSeqIDCurrent)
{
	int nFunRes = 0;
	ThreadGuard guard(m_lockULSeqIDCurrent);

	if (false == m_bSeqIDCurrentHaveBeenGet)
	{
		//can use
		ulOUTSeqIDCurrent = m_ulSeqIDCurrent; 
		m_bSeqIDCurrentHaveBeenGet = true;
		nFunRes = 0;
	}
	else
	{
		ulOUTSeqIDCurrent = 0;
		nFunRes = -1;
	}
	return nFunRes;
}
int CDbSynTestCreateMsgManager::freeLockSeqIDCurrent(ulong64& ulINSeqIDCurrent)
{
	int nFunRes = 0;
	ThreadGuard guard(m_lockULSeqIDCurrent);

	m_ulSeqIDCurrent = ulINSeqIDCurrent;
	m_bSeqIDCurrentHaveBeenGet = false;

	return nFunRes;
}

 unsigned long    CDbSynTestCreateMsgManager::getMessageCount()
{
	ThreadGuard guard(m_lockENQREQEXTotal);//m_pstEnqReqTotal
	return m_pstEnqReqTotal->m_lstDBMsg.size();
}


int CDbSynTestCreateMsgManager::addNewDataToTotalList(ulong64& ulINSeqIDCurrent, ENQREQEX** ppINEnqReq)
{
	int nFunRes = 0;
	ThreadGuard guard(m_lockENQREQEXTotal);//m_pstEnqReqTotal

	if (NULL == ppINEnqReq || NULL == *ppINEnqReq|| NULL == m_pstEnqReqTotal)
	{
		nFunRes = -1;
		return nFunRes;
	}

	_AddNewEnqDeqMsgToTotalLst(ppINEnqReq, &m_pstEnqReqTotal);			 


	return nFunRes;
}


int CDbSynTestCreateMsgManager::_GetDataFromTotalList(
	int nINPackageSize, ENQREQEX** ppOUTPackage,
	ulong64& ulSeqIDForm, ulong64& ulSeqIDTo)
{
	int nFunRes = 0;

	ThreadGuard guard(m_lockENQREQEXTotal);//m_pstEnqReqTotal

	if (NULL == ppOUTPackage || 0 > ulSeqIDForm)
	{
		nFunRes = -1;
		return nFunRes;
	}
	if (NULL == m_pstEnqReqTotal)
	{
		nFunRes = -1;
		return nFunRes;
	}

	_GetOnePackageFromTotalLst(m_pstEnqReqTotal, nINPackageSize, ppOUTPackage, ulSeqIDForm, ulSeqIDTo);

    if ((*ppOUTPackage)->m_lstDBMsg.size()>0)
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"Get Msg  IDFrom=%lld, IDTo=%lld , RemainSize=%d ",ulSeqIDForm,ulSeqIDTo,m_pstEnqReqTotal->m_lstDBMsg.size());
	}

	return nFunRes;	
} 



int CDbSynTestCreateMsgManager::_AddNewEnqDeqMsgToTotalLst(ENQREQEX** ppINEnqReq, ENQREQEX** ppstEnqReqTotal)
{ 
	int nFunRes = 0;

	lstDBMsgConIterT  iterLst;
	TA_AQ_Lib::CMessage* pGet = NULL;
	int nIndex = 0;
	ENQREQEX* pNewEnqReq = NULL;
	ENQREQEX* pTotalEnqReq = NULL;

	if (NULL == ppINEnqReq || NULL == *ppINEnqReq|| NULL == ppstEnqReqTotal)
	{
		nFunRes = -1;
		return nFunRes;
	}

	if (NULL == *ppstEnqReqTotal)
	{
		*ppstEnqReqTotal = new ENQREQEX();
	}

	pNewEnqReq = *ppINEnqReq;
	pTotalEnqReq = *ppstEnqReqTotal;

	iterLst = pNewEnqReq->m_lstDBMsg.begin();
	nIndex = 0;
	while (iterLst != pNewEnqReq->m_lstDBMsg.end() )
	{
		pGet = *iterLst; 
		pTotalEnqReq->m_lstDBMsg.push_back(pGet); 
		nIndex++;
		iterLst++;
	}
	pGet = NULL;

	pNewEnqReq->m_lstDBMsg.clear();
	DEF_DELETE(*ppINEnqReq);
	*ppINEnqReq = NULL;

	pNewEnqReq = NULL;
	pTotalEnqReq = NULL;
	return nFunRes;

}

void  CDbSynTestCreateMsgManager::_FreeEnqReqEx(ENQREQEX** ppEnqReq)
{

	if (NULL == ppEnqReq || NULL == *ppEnqReq)
	{
		return;
	}
	lstDBMsgConIterT  iter = (*ppEnqReq)->m_lstDBMsg.begin();
	TA_AQ_Lib::CMessage* pGet = NULL;

	while ( iter != (*ppEnqReq)->m_lstDBMsg.end() )
	{
		pGet = *iter;

		DEF_DELETE(pGet);

		iter++;		
	}
	(*ppEnqReq)->m_lstDBMsg.clear();
	(*ppEnqReq)->m_strPkgID.clear();
	
	DEF_DELETE(*ppEnqReq);
}

bool CDbSynTestCreateMsgManager::isFinishWork()
{
	bool bRes = false;

	bool bResInsertThread = false;
	bool bResGroupThread = false;
	bool bResPublicThread = false;
	bool bResMMSThread = false;


	if (NULL != m_pCreateInsertEnqMsgThread)
	{
		bResInsertThread = m_pCreateInsertEnqMsgThread->isFinishWork();
	}
	else
	{
		bResInsertThread = true;
	}
	
	
	if (NULL != m_pCreateGroupEnqMsgThread)
	{
		bResGroupThread = m_pCreateGroupEnqMsgThread->isFinishWork();
	}
	else
	{
		bResGroupThread = true;
	}

	if (NULL != m_pCreatePublicEnqMsgThread)
	{
		bResPublicThread = m_pCreatePublicEnqMsgThread->isFinishWork();
	}
	else
	{
		bResPublicThread = true;
	}
	
	if (NULL != m_pCreateMMSEnqMsgThread)
	{
		bResMMSThread = m_pCreateMMSEnqMsgThread->isFinishWork();
	}
	else
	{
		bResMMSThread = true;
	}

	if (bResInsertThread && bResGroupThread && bResPublicThread && bResMMSThread)
	{
		bRes = true;
	}
	else
	{
		bRes = false;
	}

	return bRes;
}

int CDbSynTestCreateMsgManager::continueWork()
{
	int nFunRes = 0;
	
	if (NULL != m_pCreateInsertEnqMsgThread)
	{
		nFunRes = m_pCreateInsertEnqMsgThread->continueWork();
	}

	if (NULL != m_pCreateGroupEnqMsgThread)
	{
		nFunRes = m_pCreateGroupEnqMsgThread->continueWork();
	}

	if (NULL != m_pCreatePublicEnqMsgThread)
	{
		nFunRes = m_pCreatePublicEnqMsgThread->continueWork();
	}

	if (NULL != m_pCreateMMSEnqMsgThread)
	{
		nFunRes = m_pCreateMMSEnqMsgThread->continueWork();
	}
	
	return nFunRes;
}
int CDbSynTestCreateMsgManager::pauseWork()
{
	int nFunRes = 0;

	if (NULL != m_pCreateInsertEnqMsgThread)
	{
		nFunRes = m_pCreateInsertEnqMsgThread->pauseWork();
	}

	if (NULL != m_pCreateGroupEnqMsgThread)
	{
		nFunRes = m_pCreateGroupEnqMsgThread->pauseWork();
	}

	if (NULL != m_pCreatePublicEnqMsgThread)
	{
		nFunRes = m_pCreatePublicEnqMsgThread->pauseWork();
	}

	if (NULL != m_pCreateMMSEnqMsgThread)
	{
		nFunRes = m_pCreateMMSEnqMsgThread->pauseWork();
	}
	 
	return nFunRes;	 
}



//////////////////////////////////////////////////////////////////////////


void  CDbSynTestCreateMsgManager::_FreeMsgVec(lstDBMsgConT& rVecMsg)
{
	lstDBMsgConIterT  iter = rVecMsg.begin();
	TA_AQ_Lib::CMessage* pGet = NULL;

	while ( iter != rVecMsg.end() )
	{
		pGet = *iter;
		DEF_DELETE(pGet);
		iter++;		
	}
	rVecMsg.clear();
}

//////////////////////////////////////////////////////////////////////////


NS_END(TA_DB_Sync)


























