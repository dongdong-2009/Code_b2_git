#include "UtilityFun.h"
#include "dbSynTestCreateEnqMsg.h"
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
#include "dbSynTestCreateMsgManager.h"


//#include "core/utilities/src/DebugUtil.h"
using TA_Base_Core::DebugUtil;


NS_BEGIN(TA_DB_Sync)


CDbSynTestCreateEnqMsg::CDbSynTestCreateEnqMsg()
{	
	m_toTerminate = false;
	m_pMsgItemHelper = NULL;
	m_pMsgItemHelper = new CMsgItemHelper();
	m_pManagerCopy = NULL;
	m_ulSeqIDCurrent = 0;
	m_ulSeqIDCurrentNextBegin = 0; 
	m_pNewEnqReq = NULL;
	m_nMsgTypeSeq = 0;

	m_strQName = g_CfgSimInfo.m_strCfg_QueueName;
	m_nNumberofInsert = g_SynProgramSimInfo.m_n_NumberofInsert;
	m_nNumberofGroup = g_SynProgramSimInfo.m_n_NumberofGroup;
	m_nNumberofPublic = g_SynProgramSimInfo.m_n_NumberofPublic;
	m_nNumberofMMS = g_SynProgramSimInfo.m_n_NumberofMMS;

	m_nIntervalTimeofInsert = g_SynProgramSimInfo.m_n_IntervalTimeofInsert;
	m_nIntervalTimeofPublic = g_SynProgramSimInfo.m_n_IntervalTimeofPublic;
	m_nIntervalTimeofGroup = g_SynProgramSimInfo.m_n_IntervalTimeofGroup;
	m_nIntervalTimeofMMS =	g_SynProgramSimInfo.m_n_IntervalTimeofMMS;

	m_szAuditDataOracle = g_SynProgramSimInfo.m_szFormat_AuditDataOracle;
	m_szAuditDataMysql = g_SynProgramSimInfo.m_szFormat_AuditDataMysql;
	m_nSRVID = g_SynProgramSimInfo.m_n_Default_SRVID;
	m_nPackageSize = g_SynProgramSimInfo.m_n_PackageSize;
	m_ulSeqIDTo = g_SynProgramSimInfo.m_n_EndSeqNum;
	
	
	
	m_strFindReplaceNO = defConfig_Replace_NO;	 //"#No#"
	m_strFindReplaceRecipient = defConfig_Replace_Recipient;   //"#Recipient#"
	m_strFindReplaceMsgTypSeq = defConfig_Replace_MsgTypSeq;	//"#MsgTypSeq#"
	m_strFindReplaceMsgTyp =  defConfig_Replace_MsgTyp;//"#MsgTyp#"
	

	m_nSynControlWorkState = SynControlWorkState_DoingWork;
	m_nSynEnqMsgWorkState = SynEnqMsgWorkState_StartWork;
	m_nWorkState = WorkState_Begin;

}


CDbSynTestCreateEnqMsg::~CDbSynTestCreateEnqMsg()
{

	_FreeEnqReqEx(&m_pNewEnqReq);
	m_pNewEnqReq = NULL;

	DEF_DELETE(m_pMsgItemHelper);
	m_pManagerCopy = NULL;
	m_ulSeqIDCurrent = 0; 
	m_pNewEnqReq = NULL;

}



void	CDbSynTestCreateEnqMsg::run()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"start [m_nClientIndex=%d] CDbSynTestCreateEnqMsg::run()", m_nClientIndex);	

	while (false == m_toTerminate)
	{
		_ThreadJob();
		TA_Base_Core::Thread::sleep(SleepValue_WorkerThread_Sleep);
	}

	//user use fun terminateAndWait()
	_ProcessUserTerminate();


	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"end [m_nClientIndex=%d] CDbSynTestCreateEnqMsg::run()", m_nClientIndex);	

}
void	CDbSynTestCreateEnqMsg::terminate()
{
	m_toTerminate = true;
}

bool CDbSynTestCreateEnqMsg::isFinishWork()
{
	bool bIsFinishWork = false;

    //No need to stop the simulator if all data have been created, 
    //Because the data may not be propagated into server side.
	if (WorkState_End == m_nWorkState)
	{
		bIsFinishWork = true;
	}

	return bIsFinishWork;
}


int CDbSynTestCreateEnqMsg::pauseWork()
{
	int nFunRes = 0;
	ThreadGuard guardData(m_lockSynControlWorkState);

	m_nSynControlWorkState = SynControlWorkState_PauseWork;

	return nFunRes;
}

int CDbSynTestCreateEnqMsg::continueWork()
{
	int nFunRes = 0;
	ThreadGuard guardData(m_lockSynControlWorkState);

	m_nSynControlWorkState = SynControlWorkState_DoingWork;

	return nFunRes;
}


int CDbSynTestCreateEnqMsg::_ProcessUserTerminate()
{
	int nFunRes = 0;

	return nFunRes;
}

int CDbSynTestCreateEnqMsg::_ThreadJob()
{
	int nFunRes = 0;
	
	nFunRes = _PocessSynControlWorkState();
	if (0 != nFunRes)
	{
		sleep( defDefaultListenTime ); 
		return nFunRes;
	}
	
	nFunRes = _PocessSynMsgWorkState();
	if (0 != nFunRes)
	{
		return nFunRes;
	}
	
	return nFunRes;
}

int CDbSynTestCreateEnqMsg::_PocessSynControlWorkState()
{
	int nFunRes = 0;
	ThreadGuard guardData(m_lockSynControlWorkState);

	if (SynControlWorkState_DoingWork == m_nSynControlWorkState)
	{
		nFunRes = 0;
	}
	else if (SynControlWorkState_PauseWork == m_nSynControlWorkState)
	{
		nFunRes = -1;
	}
	return nFunRes;
}


int CDbSynTestCreateEnqMsg::_PocessSynMsgWorkState()
{
	int nFunRes = 0;


	switch (m_nSynEnqMsgWorkState)
	{
	case SynEnqMsgWorkState_StartWork:
		nFunRes = _ProcessSynEnqMsgWorkState_StartWork();
		break;
	case SynEnqMsgWorkState_InitCfgParam:
		nFunRes = _ProcessSynEnqMsgWorkState_InitCfgParam();
		break;
	case SynEnqMsgWorkState_BeginCrestEnqMsg:
		nFunRes = _ProcessSynEnqMsgWorkState_BeginCrestEnqMsg();
		break;
	case SynEnqMsgWorkState_CrestEnqMsgGetLock:
		nFunRes = _ProcessSynEnqMsgWorkState_CrestEnqMsgGetLock();
		break;
	case SynEnqMsgWorkState_CrestEnqMsgWorking:
		nFunRes = _ProcessSynEnqMsgWorkState_CrestEnqMsgWorking();
		break;
	case SynEnqMsgWorkState_CrestEnqMsgFreeLock:
		nFunRes = _ProcessSynEnqMsgWorkState_CrestEnqMsgFreeLock();
		break;
	case SynEnqMsgWorkState_EndCrestEnqMsg:
		nFunRes = _ProcessSynEnqMsgWorkState_EndCrestEnqMsg();
		break;
	case SynEnqMsgWorkState_FinishWork:
		nFunRes = _ProcessSynEnqMsgWorkState_FinishWork();
		break;
	default:
		break;
	}//switch


	return nFunRes;
}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

int  CDbSynTestCreateEnqMsg::_CreateMsgCTOSEnqReq(
	const std::string& strINMsgQFierSub,
	const std::string& strINAgtName,
	const std::string& strINQName,
	ulong64 ulINSeqIDBegin, 
	int nOnceNum,
	ENQREQEX** ppEnqReq,
	ulong64& ulOUTSeqIDNestBegin)
{
	int				nFunRes = 0;
	std::string		strPLTemp;
	int				nSqlStrSize = 0;
	uint32			uDataLen = 0;
	std::string		strQName;
	std::string		strMsgID;
	int				nPriority = 1;
	int				nPayloadType = PAYLOAD_TYPE_OBJ;
	std::string		strPayload_1_MsgFiter;//"INSERT:TRA_OCC,1,0";
	std::string		strPayload_2_Oracle;
	std::string		strPayload_3_Mysql; 
	std::string		strUUID36;
	std::string		strUUID32;
	std::string		strReqPkgID;
	CMsgItemHelper* pMsgItemHelper = NULL;
	TA_AQ_Lib::CMessage* pMsg = NULL;
	ulong64			ulSeqIDMsg = 0;

	int             nMsgIndex = 0;
	ENQREQEX*       pEnqReqCreate = NULL;
	
	std::string     strMsgSeqDest;
	std::string     strMsgTypeSeqDest;
	//std::string strData = CUtilityFun::u64tostr(u64Value);

	
	if (nOnceNum <= 0)
	{
		nFunRes = -1;
		return nFunRes;
	}
	if (NULL == ppEnqReq)
	{
		nFunRes = -1;
		return nFunRes;
	}
	if (NULL != *ppEnqReq)
	{
		_FreeEnqReqEx(ppEnqReq);
	}

	pEnqReqCreate = new ENQREQEX();
	strQName = strINQName;
	strMsgID = "06130f11-1dc8-11e2-ac69-b2070d2cb50a";
	nPriority = 1;
	nPayloadType = PAYLOAD_TYPE_OBJ;

	strPLTemp.clear();
	   
	//INSERT:TRA_DT_001
	//GROUP:TRA_DT_001
	//PUBLIC:TRA_DT_001
	//MMS:TRA_DT_001
	//strPLTemp = "INSERT:" + m_strAgtName;//;MessageQualifier=INSERT:TRA_DT_001
	strPLTemp = strINMsgQFierSub + strINAgtName; //INSERT:TRA_DT_001

	
	//"14MESSAGE_QUALIFIER=INSERT:TRA_OCC127AUDIT_DATA_ORACLE=insert into queue_test_mysql(ID, NAME, TIMESTAMP) values (1,'prc_aq_test_queue_test_mysql_130_TRA_OCC_INSERT_teststr',sysdate) 129AUDIT_DATA_MYSQL=insert into queue_test_mysql(ID, NAME, TIMESTAMP) values (1,'prc_aq_test_queue_test_mysql_130_TRA_OCC_INSERT_teststr',sysdate()) ";
	//std::string strPayload_2_Oracle = "insert into queue_test_mysql(ID, NAME, TIMESTAMP) values (1,'TRA_OCC_41_INSERT_mysql_teststr',sysdate) ";  //for oracle
	//std::string strPayload_3_Mysql = "insert into queue_test_mysql(ID, NAME, TIMESTAMP) values (1,'TRA_OCC_41_INSERT_mysql_teststr',sysdate()) ";	//for mysql

	nSqlStrSize = m_szAuditDataOracle.size();


	strUUID36 = TA_Base_Core::TA_Uuid::generateUUID();
	strUUID32 = CUtilityFun::strRemoveUnNeedChar(strUUID36); 
	strUUID32   = CUtilityFun::strtoupper((char*)strUUID32.c_str());
	strReqPkgID	= strUUID32;

	strPayload_1_MsgFiter = strPLTemp; 


	pMsgItemHelper = new CMsgItemHelper();
	nMsgIndex = 0;
	pEnqReqCreate->m_lstDBMsg.clear();
	while (nMsgIndex < nOnceNum)
	{	
		ulSeqIDMsg   = ulINSeqIDBegin + nMsgIndex;		

		
		pMsg = new TA_AQ_Lib::CMessage(); 							   	

		pMsg->setSender(m_strAgtName);
		pMsg->setQName(strQName);
		pMsg->setMsgSeqID(ulSeqIDMsg);//1 2 3
		pMsg->setMsgID(strMsgID);  //
		pMsg->setMsgPriority(nPriority);
		pMsg->setPayloadType(nPayloadType);
		pMsg->setPayload(defPayload_1_Tag,  strPayload_1_MsgFiter);

		strMsgSeqDest  = CUtilityFun::u64tostr(ulSeqIDMsg);
		strMsgTypeSeqDest = CUtilityFun::u64tostr(m_nMsgTypeSeq);
		
		strPayload_2_Oracle	= m_szAuditDataOracle;
		_ReplaceMsgString(strPayload_2_Oracle, m_strFindReplaceNO, strMsgSeqDest);
		_ReplaceMsgString(strPayload_2_Oracle, m_strFindReplaceRecipient, strINAgtName);
		_ReplaceMsgString(strPayload_2_Oracle, m_strFindReplaceMsgTypSeq, strMsgTypeSeqDest);
		_ReplaceMsgString(strPayload_2_Oracle, m_strFindReplaceMsgTyp, strINMsgQFierSub);		
		pMsg->setPayload(defPayload_2_Tag, strPayload_2_Oracle);

		
		strPayload_3_Mysql	= m_szAuditDataMysql;
		_ReplaceMsgString(strPayload_3_Mysql, m_strFindReplaceNO, strMsgSeqDest);
		_ReplaceMsgString(strPayload_3_Mysql, m_strFindReplaceRecipient, strINAgtName);
		_ReplaceMsgString(strPayload_3_Mysql, m_strFindReplaceMsgTypSeq, strMsgTypeSeqDest);
		_ReplaceMsgString(strPayload_3_Mysql, m_strFindReplaceMsgTyp, strINMsgQFierSub);		
		pMsg->setPayload(defPayload_3_Tag,  strPayload_3_Mysql);

		//pMsg->setOServerID(m_nSRVID); 

		pMsg->setPayload(defMsg_SrvIDTag,  m_nSRVID);
		pMsg->setPayload(defMsg_CenterSIDTag,  0);


		pEnqReqCreate->m_lstDBMsg.push_back(pMsg);
		nMsgIndex++;
		m_nMsgTypeSeq++;
		
		//fix last msg mising
		if (ulSeqIDMsg >= m_ulSeqIDTo)
		{
			break;
		}
	 

	}//while



	pEnqReqCreate->m_strPkgID = strReqPkgID;


	if (nMsgIndex >= 0)
	{
		ulOUTSeqIDNestBegin = ulINSeqIDBegin + nMsgIndex;
	}

	*ppEnqReq = pEnqReqCreate;
	pEnqReqCreate = NULL;


	return nFunRes;
}



void  CDbSynTestCreateEnqMsg::_FreeMsgVec(lstDBMsgConT& rVecMsg)
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
int	CDbSynTestCreateEnqMsg::_FreeEnqReqEx(ENQREQEX** ppEnqReqEx)
{
	int nFunRes = 0;

	if (NULL != ppEnqReqEx && NULL != *ppEnqReqEx)
	{
		_FreeMsgVec((*ppEnqReqEx)->m_lstDBMsg);
		 
		DEF_DELETE(*ppEnqReqEx);
	}
	
	return nFunRes;
}

//////////////////////////////////////////////////////////////////////////





//////////////////////////////////////////////////////////////////////////
int CDbSynTestCreateEnqMsg::_ProcessSynEnqMsgWorkState_StartWork()
{
	int nFunRes = 0;
	m_nSynEnqMsgWorkState = SynEnqMsgWorkState_InitCfgParam;
	m_ulSeqIDCurrent = 0; 
	m_pNewEnqReq = NULL;

	return nFunRes;
}
int CDbSynTestCreateEnqMsg::_ProcessSynEnqMsgWorkState_InitCfgParam()
{
	int nFunRes = 0;
	m_nSynEnqMsgWorkState = SynEnqMsgWorkState_BeginCrestEnqMsg;

	return nFunRes;
}
int CDbSynTestCreateEnqMsg::_ProcessSynEnqMsgWorkState_BeginCrestEnqMsg()
{
	int nFunRes = 0;
	if (NULL == m_pManagerCopy)
	{
		m_nSynEnqMsgWorkState = SynEnqMsgWorkState_EndCrestEnqMsg;

		nFunRes = -1;
		return nFunRes;
	}
	
	//before get SeqIDCurrent, check if there have max limit data in total list
	if (m_pManagerCopy->getMessageCount() > def_int_TotalListNumLimit) 
	{
		sleep(100);
		return nFunRes;
	}


	m_ulSeqIDCurrent = 0;
	m_ulSeqIDCurrentNextBegin = 0;
	
	//if other product thread get   m_ulSeqIDCurrent lock 
	//nFunRes = -1  m_ulSeqIDCurrent = 0;
	nFunRes = m_pManagerCopy->getLockSeqIDCurrent(m_ulSeqIDCurrent);	
	if (0 != nFunRes)
	{
		m_nSynEnqMsgWorkState = SynEnqMsgWorkState_BeginCrestEnqMsg;
		return nFunRes;
	}


	_FreeEnqReqEx(&m_pNewEnqReq);
	m_pNewEnqReq = NULL;


	m_nSynEnqMsgWorkState = SynEnqMsgWorkState_CrestEnqMsgGetLock;

	return nFunRes;
}
int CDbSynTestCreateEnqMsg::_ProcessSynEnqMsgWorkState_CrestEnqMsgGetLock()
{
	int nFunRes = 0;


	m_nSynEnqMsgWorkState = SynEnqMsgWorkState_CrestEnqMsgWorking;

	return nFunRes;
}

int CDbSynTestCreateEnqMsg::_ProcessSynEnqMsgWorkState_CrestEnqMsgWorking()
{
	int nFunRes = 0;  


	m_ulSeqIDCurrentNextBegin = 0; 

	_FreeEnqReqEx(&m_pNewEnqReq);
	m_pNewEnqReq = NULL;
	switch (m_nEnqMsgType)
	{
	case SynEnqMsgMsgType_INSERT:
		{
			nFunRes = _CreateMsgCTOSEnqReq(
				m_strMsgQFierSub, 
				m_strAgtName,
				m_strQName, 
				m_ulSeqIDCurrent, 
				m_nNumberofInsert,
				&m_pNewEnqReq,
				m_ulSeqIDCurrentNextBegin);

			break;
		} 
	case SynEnqMsgMsgType_GROUP:
		{
			nFunRes = _CreateMsgCTOSEnqReq(
				m_strMsgQFierSub, 
				m_strAgtName,
				m_strQName, 
				m_ulSeqIDCurrent, 
				m_nNumberofGroup,
				&m_pNewEnqReq,
				m_ulSeqIDCurrentNextBegin);

			break;
		} 
	case SynEnqMsgMsgType_PUBLIC:
		{
			nFunRes = _CreateMsgCTOSEnqReq(
				m_strMsgQFierSub, 
				m_strAgtName,
				m_strQName, 
				m_ulSeqIDCurrent, 
				m_nNumberofPublic,
				&m_pNewEnqReq,
				m_ulSeqIDCurrentNextBegin);

			break;
		}
	case SynEnqMsgMsgType_MMS:
		{
			nFunRes = _CreateMsgCTOSEnqReq(
				m_strMsgQFierSub, 
				m_strAgtName,
				m_strQName, 
				m_ulSeqIDCurrent, 
				m_nNumberofMMS,
				&m_pNewEnqReq,
				m_ulSeqIDCurrentNextBegin);

			break;
		}		
		 
	default:
		break;
	}
	
	//add new data to Total list
	if (NULL != m_pManagerCopy)
	{
		nFunRes = m_pManagerCopy->addNewDataToTotalList(m_ulSeqIDCurrentNextBegin, &m_pNewEnqReq);
	}
	else
	{
		m_nSynEnqMsgWorkState = SynEnqMsgWorkState_EndCrestEnqMsg;
	}
	
	//check add new data to Total list result
	if (0 != nFunRes)
	{
		m_nSynEnqMsgWorkState = SynEnqMsgWorkState_EndCrestEnqMsg;
	}
	else
	{
		m_nSynEnqMsgWorkState = SynEnqMsgWorkState_CrestEnqMsgFreeLock;				 

	}
		

	_FreeEnqReqEx(&m_pNewEnqReq);
	m_pNewEnqReq = NULL;



	return nFunRes;
}
int CDbSynTestCreateEnqMsg::_ProcessSynEnqMsgWorkState_CrestEnqMsgFreeLock()
{
	int nFunRes = 0;
	


	if (NULL == m_pManagerCopy)
	{
		m_nSynEnqMsgWorkState = SynEnqMsgWorkState_EndCrestEnqMsg;

		nFunRes = -1;
		return nFunRes;
	}
	
	nFunRes = m_pManagerCopy->freeLockSeqIDCurrent(m_ulSeqIDCurrentNextBegin);	
	if (0 != nFunRes)
	{
		m_nSynEnqMsgWorkState = SynEnqMsgWorkState_BeginCrestEnqMsg;
		return nFunRes;
	}

	//nFunRes = m_pManagerCopy->freeLockDataLstDBMsgConT();


	if (m_ulSeqIDCurrentNextBegin > m_ulSeqIDTo)
	{
		m_nSynEnqMsgWorkState = SynEnqMsgWorkState_EndCrestEnqMsg;
		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"[m_nClientIndex=%d][m_strAgtName=%s][m_strQName=%s][m_ulSeqIDCurrentNextBegin=%llu][m_ulSeqIDTo=%llu][m_strMsgQFierSub=%s] EndCrestEnqMsg", 
			m_nClientIndex, 
			m_strAgtName.c_str(), 
			m_strQName.c_str(), 
			m_ulSeqIDCurrentNextBegin, 
			m_ulSeqIDTo,
			m_strMsgQFierSub.c_str());  

	}
	else
	{
		m_nSynEnqMsgWorkState = SynEnqMsgWorkState_BeginCrestEnqMsg;


		switch (m_nEnqMsgType)
		{
		case SynEnqMsgMsgType_INSERT:
			TA_Base_Core::Thread::sleep(m_nIntervalTimeofInsert);	
			break;
		case SynEnqMsgMsgType_GROUP:
			TA_Base_Core::Thread::sleep(m_nIntervalTimeofGroup);	
			break;
		case SynEnqMsgMsgType_PUBLIC:
			TA_Base_Core::Thread::sleep(m_nIntervalTimeofPublic);	
			break;
		case SynEnqMsgMsgType_MMS:
			TA_Base_Core::Thread::sleep(m_nIntervalTimeofMMS);	
			break;

		}


	}



	return nFunRes;
}
int CDbSynTestCreateEnqMsg::_ProcessSynEnqMsgWorkState_EndCrestEnqMsg()
{
	int nFunRes = 0;

	m_nSynEnqMsgWorkState = SynEnqMsgWorkState_FinishWork;


	return nFunRes;
}
int CDbSynTestCreateEnqMsg::_ProcessSynEnqMsgWorkState_FinishWork()
{
	int nFunRes = 0;

	m_nWorkState = WorkState_End;

	return nFunRes;
}



//////////////////////////////////////////////////////////////////////////




int	CDbSynTestCreateEnqMsg::_ReplaceMsgString(
	std::string& strINOUTSring,
	const std::string& strReplaceFind,
	const std::string& strReplaceDest
	)
{
	int nFunRes = 0;  

	std::string strBigSrc;
	std::string strBigSub;
	std::string strReplaceNo;
	std::string strSrc;
	std::string strSrcTmp;

	int         nFind = -1;

	if (true == strINOUTSring.empty())
	{
		nFunRes = -1;
		return nFunRes;
	}

	//AuditDataOracle=insert into queue_test_mysql(ID, NAME, TIMESTAMP) values(#No#, #Recipient#, sysdate)
	strReplaceNo = strReplaceFind;
	strSrc = strINOUTSring;

	while(1)
	{
		strBigSrc = strSrc;
		strBigSub = strReplaceNo;
		
		
		strBigSrc   = CUtilityFun::strtoupper((char*)strBigSrc.c_str());
		strBigSub   = CUtilityFun::strtoupper((char*)strBigSub.c_str());
	
		nFind = -1;
		nFind = (int)strBigSrc.find(strBigSub);
		strSrcTmp = strSrc;

		if (nFind >= 0)
		{
			strSrcTmp.replace(nFind, strReplaceNo.size(), strReplaceDest);
			strSrc = strSrcTmp;
		}
		else
		{
			break;//while(1)
		}

	}//while(1)

	strINOUTSring = strSrc;

	return nFunRes;
} 
//////////////////////////////////////////////////////////////////////////




NS_END(TA_DB_Sync)


























