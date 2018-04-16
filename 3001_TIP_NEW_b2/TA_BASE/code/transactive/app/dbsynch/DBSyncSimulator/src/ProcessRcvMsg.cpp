//////////////////////////////////////////////////////////////////////
///		@file		ProcessRcvMsg.cpp
///		@author		zhilin,ouyang
///		@date		2012-02-29 14:44:41
///
///		@brief	    the thread to process the received messages from  
///                 remote database/location.
///                 
//////////////////////////////////////////////////////////////////////
#include "ProcessRcvMsg.h"
#include "TimeMonitor.h"
#include "Message.h"
#include "MsgHelper.h"
#include "Producer.h"
#include "DatabaseImpl.h"
#include "TcpClientManager.h"
#include "UtilityFun.h"
#include "IDbData.h"
#include "DBException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DbSyncException.h"
#include "core/synchronisation/src/ThreadGuard.h"

NS_BEGIN(TA_DB_Sync)

CProcessRcvMsg::CProcessRcvMsg()
: m_bTerminate(false)
, m_nTcpState(TCP_CONNECTED)
, m_nMsgRcvCnt(0)
, m_nEnqMsgCnt(0)
, m_ulMsgRcvCntAll(0)
, m_ulTotalEnqMsgCnt(0)
, m_nProcessState(STATE_CODE_INITIAL_ENV)
, m_pMsgItemHelper(NULL)
, m_pEnqAck(NULL)
, m_pCurItem(NULL)
, m_queueSemaphore(0)
{	
// 	m_pEnqDatabase = new CDatabaseImpl(g_system_variables.m_SrcDbConStr);
	//m_pMsgProducer = new CProducer(m_pEnqDatabase);
	//m_pDupChkDb = new CDatabaseImpl(g_system_variables.m_SrcDbConStr);
	m_strPkgID.clear();
	m_strServerID.clear();
	m_strQName.clear();
	m_mapDupChkSID.clear();
	_FreeMsgLst(m_lstEnqReq);
	m_nEnqWorkIndex = 0;
	m_pTimer = new CTimeMonitor();
	m_nControlWorkState =   ControlWorkState_DoingWork;
	m_ulLastEnqSeqID = 0;
}

CProcessRcvMsg::~CProcessRcvMsg()
{
	m_strPkgID.clear();
	m_strServerID.clear();
	m_strQName.clear();

	_FreeMsgLst(m_lstEnqReq);
// 	DEF_DELETE(m_pMsgProducer);
// 	DEF_DELETE(m_pEnqDatabase);
// 	DEF_DELETE(m_pDupChkDb);
	DEF_DELETE(m_pTimer);
}


int   CProcessRcvMsg::processTcpEvent( int nStateCode )
{
	FUNCTION_ENTRY("processTcpEvent");

	int  nRetCode = Ret_Success;

	if (m_nTcpState != nStateCode)
	{
		m_nTcpState = nStateCode;
	}

	FUNCTION_EXIT;
	return nRetCode;
}

int   CProcessRcvMsg::processRcvFrame(ENQREQEX* pEnqReq)
{
	FUNCTION_ENTRY("processRcvFrame");
	int  nRetCode = Ret_Success;

	if (!pEnqReq)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		return Ret_Failed;
	}

	{
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		m_lstEnqReq.push_back( pEnqReq );
	}	

	if (STATE_CODE_WAITFOR_ENQREQ == m_nProcessState)
	{
		m_queueSemaphore.post();
	}

	FUNCTION_EXIT;
	return nRetCode;
}

void   CProcessRcvMsg::run()
{
	FUNCTION_ENTRY("run");

	while ( !m_bTerminate )
	{
		_ThreadJob();

		sleep( defDefaultEnqTime );
	}

	FUNCTION_EXIT;
}

void   CProcessRcvMsg::terminate()
{
	FUNCTION_ENTRY("terminate");

	m_bTerminate = true;
	if (STATE_CODE_WAITFOR_ENQREQ == m_nProcessState)
	{
		m_queueSemaphore.post();
	}

	FUNCTION_EXIT;
}

void  CProcessRcvMsg::setServerID(const std::string& strServerID)
{
	FUNCTION_ENTRY("setServerID");

	if (strServerID.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Client2Server_ID_Invalid, defErrMsg(Err_Client2Server_ID_Invalid));
		return ;
	}

	m_strServerID = strServerID;

	FUNCTION_EXIT;
}

void  CProcessRcvMsg::setRcvPara(const std::string& strQName, const std::string& strRcver)
{
	FUNCTION_ENTRY("setRcvName");

	if (strQName.empty() || strRcver.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Client2Server_ID_Invalid, defErrMsg(Err_Client2Server_ID_Invalid));
		return ;
	}

	m_strQName = strQName;
	m_strRcvName = strRcver;

	FUNCTION_EXIT;
}


void   CProcessRcvMsg::_ThreadJob()
{

	if (ControlWorkState_DoingWork != m_nControlWorkState)
	{
		sleep( defDefaultEnqTime );
		return;
	}

	if (m_nEnqWorkIndex > SERVER_LIMIT_LOG_VLAUE_MAX)
	{
		m_nEnqWorkIndex = 0;
	}
	
	switch (m_nProcessState)
	{
	case STATE_CODE_INITIAL_ENV:
		_ProcessInitEnv();
		break;
	case STATE_CODE_INITIAL_ENQ:
		_ProcessInitEnq();
		break;
	case STATE_CODE_WAITFOR_ENQREQ:
		_ProcessWaitForEnqReq();
		break;
	case STATE_CODE_RCV_MSG:
		_ProcessRcvMsg();
		break;
	case STATE_CODE_GETSID_FROMDB:
		_ProcessGetSIDFromDB();
		break;
	case STATE_CODE_ENQ_MSG:
		_ProcessEnqMsg();
		break;
	case STATE_CODE_UPDSID_2DB:
		_ProcessUpdSID2DB();
		break;
	case STATE_CODE_SEND_ACK:
		_ProcessSendAck();
		break;
	case STATE_CODE_FINISH_TRANSACTION:
		_ProcessFinishTransaction();
		break;
	default:
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Propagate_Code, defErrMsg(Err_Invalid_Propagate_Code));
	}
}

void   CProcessRcvMsg::_ProcessInitEnv()
{
	m_strPkgID.clear();
	_FreeMsgLst(m_lstEnqReq);
	DEF_DELETE(m_pEnqAck);
	DEF_DELETE(m_pMsgItemHelper);

	m_nProcessState = STATE_CODE_INITIAL_ENQ;
}

void   CProcessRcvMsg::_ProcessInitEnq()
{
	if (!m_pMsgItemHelper)
		m_pMsgItemHelper = new CMsgItemHelper();
	if (!m_pEnqAck)
		m_pEnqAck = new ENQACK();

	_InitEnqEnv();

	if (!m_lstEnqReq.empty())
	{
		m_nProcessState = STATE_CODE_ENQ_MSG;
	}
	else
	{
		m_nProcessState = STATE_CODE_WAITFOR_ENQREQ;
	}	
}

void   CProcessRcvMsg::_ProcessWaitForEnqReq()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"Current State: %s", "Wait for enqueue message request.");

	m_queueSemaphore.wait();

	m_nProcessState = STATE_CODE_RCV_MSG;
}

void   CProcessRcvMsg::_ProcessRcvMsg()
{
	// if the received message count is 0, just return.		
	if ( m_lstEnqReq.empty() ) 
	{
		m_nProcessState = STATE_CODE_WAITFOR_ENQREQ;
		return;
	}
	m_pCurItem = m_lstEnqReq.front();
	m_nMsgRcvCnt = (uint32)(m_pCurItem->m_lstDBMsg).size();
	m_ulMsgRcvCntAll += m_nMsgRcvCnt;
	
	m_pTimer->getTimeDiffByMilSecond(PROPAGATE_TIMER_LOG_EACHENQWORK);
	m_pTimer->getTimeDiffByMilSecond(PROPAGATE_TIMER_LOG_ENQDATA);

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"%s  [%s]  Received Message Count: %ld, Total received Message Count: %llu, Package ID: %s", 
		m_strRcvName.c_str(),m_strQName.c_str(),m_nMsgRcvCnt, m_ulMsgRcvCntAll, m_strPkgID.c_str());

	m_nProcessState = STATE_CODE_GETSID_FROMDB;
}

void   CProcessRcvMsg::_ProcessGetSIDFromDB()
{
	int nRetCode = Ret_Failed;	

	//nRetCode = _GetCurSIDFromDB();  // get current Max SID
// 	if ( Ret_Success != nRetCode ) 
// 	{
// 		return;	
// 	}

	ulong64 ulSID = 0;
	ulSID = 1;	//Test Value
	m_mapDupChkSID[1] = ulSID;

	m_nProcessState = STATE_CODE_ENQ_MSG;
}

void   CProcessRcvMsg::_ProcessEnqMsg()
{
// 	if( !m_pEnqDatabase || !m_pMsgProducer)
// 	{
// 		m_nProcessState = STATE_CODE_INITIAL_ENQ;
// 		return;
// 	}

	if ( TCP_DISCONNECTED == m_nTcpState )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Tcp_Disconnected_Enq, defErrMsg(Err_Tcp_Disconnected_Enq));

		{
			TA_Base_Core::ThreadGuard guard(m_threadLock);
			_FreeMsgLst(m_lstEnqReq);
		}		

		m_nProcessState = STATE_CODE_WAITFOR_ENQREQ;
		return;
	}	

	TA_Base_Core::ThreadGuard guard(m_threadLock);
	uint32 nEnqMsgCnt = 0;
	int nRetCode = Ret_Success;
	int nSrvID = 0;
	ulong64 ulSeqID = 0;
	std::string strMsgData;
	int nDupChkRet = Ret_Success;
	
	lstEnqReqIterT iterLst = m_lstEnqReq.begin();
	mapDupChkSIDIterT iterDup = m_mapDupChkSID.begin();
	while (iterLst != m_lstEnqReq.end())
	{
		lstDBMsgConIterT iterMsg = (*iterLst)->m_lstDBMsg.begin();
		while (iterMsg != (*iterLst)->m_lstDBMsg.end())
		{
			if (m_bTerminate) return;

			if (m_nTcpState == TCP_DISCONNECTED) 
			{ 
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"Error Code: %d, Error message: %s", Err_Tcp_Disconnected_Enq, defErrMsg(Err_Tcp_Disconnected_Enq));
			    {
				    TA_Base_Core::ThreadGuard guard(m_threadLock);
				    _FreeMsgLst(m_lstEnqReq);
			    }
				m_nProcessState = STATE_CODE_WAITFOR_ENQREQ;
				return;
			}
			if ( m_strQName.empty() )
				m_strQName = (*iterMsg)->getQName();
			nSrvID  = (*iterMsg)->getOServerID();
			ulSeqID = (*iterMsg)->getMsgSeqID();
			m_ulLastEnqSeqID = ulSeqID;
			// check duplicate
			//nDupChkRet = _MsgDupChk(nSrvID, ulSeqID, iterDup);
// 			switch (nDupChkRet)
// 			{
// 			case Ret_Failed:  // error occurred
// 				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
// 					"Message original srv id: %d cannot found in config DB table", nSrvID);
// 				_Write2ExceptionQ(*iterMsg);
// 				iterMsg++; 
// 				continue;
// 				break;
// 			case Ret_Exit: // duplicate
// 				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
// 					"Message duplicated with SID: %llu, message ID: %s",(*iterMsg)->getMsgSeqID(), (*iterMsg)->getMsgID().c_str());
// 				_Write2ExceptionQ(*iterMsg);
// 				iterMsg++; nEnqMsgCnt++;
// 				continue;
// 				break;
// 			default: // successful
// 				nRetCode = m_pMsgProducer->enqueueMsg( (*iterMsg) );
// 				break;
// 			}
			nRetCode = Ret_Success;//test value
			if ( Ret_Success != nRetCode ) 
			{
				switch ( nRetCode )
				{
				case ER_MYSQL_GONE_AWAY:
				case ER_MYSQL_CANNT_CONNECT:
					sleep(TIME_BASE_SECOND);
					m_nProcessState = STATE_CODE_INITIAL_ENQ;
					return;	
				case ER_MYSQL_READ_ONLY:
					sleep(TIME_SECOND_FIVE);
					LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "ER_MYSQL_READ_ONLY(1290) == EnqRes");
					g_sys_status_mask.setCodeFlag(STATE_CODE_RUNNING_MODE);
					return;				
				default:
					sleep(TIME_SECOND_TWO);
					continue;
					break;
				}				
			}
			nEnqMsgCnt++;
			iterDup->second = ulSeqID;

			if (TA_Base_Core::DebugUtil::DebugMsg == g_system_variables.m_LogLevel )
			{
				(*iterMsg)->getPayload(strMsgData);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugMsg,
					"Message ID: %s, Sequence ID: %llu",(*iterMsg)->getMsgID().c_str(), ulSeqID);
				LOGLARGESTRING(SourceInfo, TA_Base_Core::DebugUtil::DebugMsg, strMsgData);
			}				

			iterMsg++;
		}

		iterLst++;
	}

	m_nEnqMsgCnt = nEnqMsgCnt, m_ulTotalEnqMsgCnt += nEnqMsgCnt;	

	m_nProcessState = STATE_CODE_UPDSID_2DB;
	
}

void   CProcessRcvMsg::_ProcessUpdSID2DB()
{
	int nRetCode = _UpdCurSID2DB();
	if (ER_MYSQL_GONE_AWAY == nRetCode) return;

	m_nStepEnqDataTime = m_pTimer->getTimeDiffByMilSecond(PROPAGATE_TIMER_LOG_ENQDATA);
	m_pTimer->reset(PROPAGATE_TIMER_LOG_ENQDATA);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[EnqWorkIndex=%d][StepEnqDataTime=%d]", m_nEnqWorkIndex, m_nStepEnqDataTime);

	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"Enqueue Message Count: %ld, Total Enqueue Message Count: %llu, Queue Name: %s", m_nEnqMsgCnt, m_ulTotalEnqMsgCnt, m_strQName.c_str());

	m_nProcessState = STATE_CODE_SEND_ACK;
	m_pTimer->getTimeDiffByMilSecond(PROPAGATE_TIMER_LOG_SNEDACK);
	
}

void   CProcessRcvMsg::_ProcessSendAck()
{
	int nRetCode = Ret_Failed;
	if (!m_pMsgItemHelper || !m_pEnqAck)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));

		m_nProcessState = STATE_CODE_INITIAL_ENV;
		return ;
	}

	if (m_nTcpState == TCP_DISCONNECTED) 
	{ 
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Tcp_Disconnected_Enq, defErrMsg(Err_Tcp_Disconnected_Enq));

		{
			TA_Base_Core::ThreadGuard guard(m_threadLock);
			_FreeMsgLst(m_lstEnqReq);
		}
		m_nProcessState = STATE_CODE_WAITFOR_ENQREQ;
		return;
	}

	if ( m_nEnqMsgCnt )
	{		
		m_pEnqAck->m_strPkgID = m_pCurItem->m_strPkgID;
		m_pEnqAck->m_strQName = m_strQName;
		m_pEnqAck->m_nEnqReqACK = m_nEnqMsgCnt;

		MSGPKG*  pMsgPkg = NULL;
		m_pMsgItemHelper->setCTOSEnqACK(m_pEnqAck, pMsgPkg);

		nRetCode = defSendDataToServer(m_strServerID,  pMsgPkg);	
		if (nRetCode != Ret_Success)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"Tcp send message failed, error code: %d.", nRetCode);

			return;
		}
	}

	m_nStepSendACkTime = m_pTimer->getTimeDiffByMilSecond(PROPAGATE_TIMER_LOG_SNEDACK);
	m_pTimer->reset(PROPAGATE_TIMER_LOG_SNEDACK);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[EnqWorkIndex=%d][StepSendACkTime=%d]", m_nEnqWorkIndex, m_nStepSendACkTime);
	
	
	m_nEachEnqWorkUse = m_pTimer->getTimeDiffByMilSecond(PROPAGATE_TIMER_LOG_EACHENQWORK);
	m_pTimer->reset(PROPAGATE_TIMER_LOG_EACHENQWORK);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[EnqWorkIndex=%d][EachEnqWorkUse=%d]", m_nEnqWorkIndex, m_nEachEnqWorkUse);

	m_nEnqWorkIndex++;

	m_nProcessState = STATE_CODE_FINISH_TRANSACTION;
}


void   CProcessRcvMsg::_ProcessFinishTransaction()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"Finish transaction for queue: %s, SeqID from %llu to %llu, Package ID: %s",
		m_strQName.c_str(),m_pCurItem->m_lstDBMsg.front()->getMsgSeqID(), m_pCurItem->m_lstDBMsg.back()->getMsgSeqID(), m_pCurItem->m_strPkgID.c_str());

	_FreeMsgItem(m_pCurItem);

	{
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		m_lstEnqReq.pop_front();
	}	

	if ( !m_lstEnqReq.empty() )
	{
		m_nProcessState = STATE_CODE_RCV_MSG;
	}
	else
	{
		m_nProcessState = STATE_CODE_WAITFOR_ENQREQ;
	}	
}

void   CProcessRcvMsg::_FreeMsgLst(lstEnqReqT& rLstMsg)
{
	lstEnqReqIterT  iter = rLstMsg.begin();

	while ( iter != rLstMsg.end() )
	{
		lstDBMsgConIterT iterDBMsg = (*iter)->m_lstDBMsg.begin();
		while ( iterDBMsg != (*iter)->m_lstDBMsg.end() )
		{
			DEF_DELETE(*iterDBMsg);
			iterDBMsg++;
		}
		(*iter)->m_lstDBMsg.clear();

		DEF_DELETE(*iter);
		iter++;
	}
	rLstMsg.clear();

	m_nEnqMsgCnt = 0;
	m_nMsgRcvCnt = 0;
}

void   CProcessRcvMsg::_FreeMsgItem(ENQREQEX*& pEnqReq)
{
	lstDBMsgConIterT iterDBMsg = pEnqReq->m_lstDBMsg.begin();
	while ( iterDBMsg != pEnqReq->m_lstDBMsg.end() )
	{
		DEF_DELETE(*iterDBMsg);
		iterDBMsg++;
	}
	pEnqReq->m_lstDBMsg.clear();

	DEF_DELETE(pEnqReq);

	m_nEnqMsgCnt = 0;
	m_nMsgRcvCnt = 0;
}

void   CProcessRcvMsg::_PrintLog(int& nPrintInterval, int nState)
{
	nPrintInterval++;
	if (nPrintInterval > defDefaultMsgPrintTimes)
	{
		nPrintInterval = 0;
		switch (nState)
		{
		case STATE_CODE_INITIAL_ENV:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Current process enqueue request state: %s",  "STATE_CODE_INITIAL_ENV");
			break;
		case STATE_CODE_INITIAL_ENQ:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Current process enqueue request state: %s",  "STATE_CODE_INITIAL_ENQ");
			break;
		case STATE_CODE_WAITFOR_ENQREQ:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Current process enqueue request state: %s",  "STATE_CODE_WAITFOR_ENQREQ");
			break;
		case STATE_CODE_RCV_MSG:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Current process enqueue request state: %s",  "STATE_CODE_RCV_MSG");
			break;
		case STATE_CODE_ENQ_MSG:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Current process enqueue request state: %s",  "STATE_CODE_ENQ_MSG");
			break;
		case STATE_CODE_SEND_ACK:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Current process enqueue request state: %s",  "STATE_CODE_SEND_ACK");
			break;
		case STATE_CODE_FINISH_TRANSACTION:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Current process enqueue request state: %s",  "STATE_CODE_FINISH_TRANSACTION");
			break;
		default:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Error Code: %d, Error message: %s", Err_Invalid_Propagate_Code, defErrMsg(Err_Invalid_Propagate_Code));
		}

	}
}

void   CProcessRcvMsg::_InitEnqEnv()
{
// 	DEF_DELETE(m_pEnqDatabase);
// 	DEF_DELETE(m_pMsgProducer);

// 	m_pEnqDatabase = new CDatabaseImpl(g_system_variables.m_SrcDbConStr);
	//m_pMsgProducer = new CProducer(m_pEnqDatabase);

// 	if (!m_pMsgProducer || !m_pEnqDatabase)
// 	{
// 		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
// 			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
// 		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
// 	}
}

void   CProcessRcvMsg::_InitDupChkEnv()
{
// 	DEF_DELETE(m_pDupChkDb);

// 	m_pDupChkDb = new CDatabaseImpl(g_system_variables.m_SrcDbConStr);

// 	if (!m_pDupChkDb)
// 	{
// 		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
// 			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
// 		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
// 	}
}

int   CProcessRcvMsg::_GetCurSIDFromDB()
{
	uint32  nSrvID = 0;
	uint32  ulRowNums = 0;
	ulong64 ulSID = 0;
	std::string strSql, strSID;
	IDbData*  pData = NULL;
	int nRetCode = Ret_Success;
//  	DEF_ERROR_CHECK(m_pDupChkDb);
// 
// 	std::vector<std::string> columnNames;
// 	columnNames.push_back("SRV_ID");
// 	columnNames.push_back("MSG_SID");
// 
//  	strSql = m_pDupChkDb->prepareSQLStatement(ENQ_SQL_SUPDATE_GET_MSGSID_7000, m_strRcvName, m_strQName);	
// 
// 	try
// 	{
// 		m_pDupChkDb->connect();
// 		m_pDupChkDb->beginTransaction();
// 		pData = m_pDupChkDb->executeQuery(strSql, columnNames);
// 
// 		ulRowNums = pData->getNumRows();
// 		if (ulRowNums > 1)
// 		{
// 			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
// 				"error! get MysqlServerPort error! [strSql=%s][nRowNums=%d]", 
// 				strSql.c_str(), ulRowNums);
// 			return Ret_Failed;
// 		}
// 		if (!ulRowNums)
// 		{
// 			return _InitSID2DB();
// 		}
// 		nSrvID = pData->getIntegerData(0, columnNames[0]);
// 		strSID = pData->getStringData(0, columnNames[1]);
// 		ulSID  = CUtilityFun::atoui64(strSID);
// 
// 		m_mapDupChkSID[nSrvID] = ulSID;
// 	}
// 	catch (const CDBException& e)
// 	{
// 		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
// 			"Database exception, Error Code: %d; Message: %s", e.getNativeCode(), e.getReason().c_str() );
// 		nRetCode = Ret_Failed;
// 		if (ER_MYSQL_GONE_AWAY == e.getNativeCode() ||
// 			ER_MYSQL_CANNT_CONNECT == e.getNativeCode())
// 		{
// 			nRetCode = ER_MYSQL_GONE_AWAY;
// 			_InitDupChkEnv();
// 		}
// 		sleep(TIME_SECOND_FIVE);
// 
// 	}	
// 	catch (...)
// 	{
// 		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
// 			"Error Code: %d; Message: %s", Err_Unknown, defErrMsg(Err_Unknown) );	
// 
// 		nRetCode = ER_MYSQL_GONE_AWAY;
// 		sleep(TIME_SECOND_FIVE);
// 		_InitDupChkEnv();
// 	}
// 	DEF_DELETE(pData);
	return nRetCode;	
}

int   CProcessRcvMsg::_UpdCurSID2DB()
{
	int nRetCode = Ret_Success;
// 	DEF_ERROR_CHECK(m_pDupChkDb);
// 	DEF_ERROR_CHECK(!m_mapDupChkSID.empty());
// 
// 	mapDupChkSIDIterT iter = m_mapDupChkSID.begin();
// 	std::string strSql = m_pDupChkDb->prepareSQLStatement(ENQ_SQL_UPDATE_SET_NEWMSGSID_7001, iter->second, m_strRcvName, iter->first, m_strQName);
// 
// 	try
// 	{
// 		m_pDupChkDb->connect();
// 		m_pDupChkDb->executeModification(strSql);
// 		m_pDupChkDb->commitTransaction();
// 	}
// 	catch (const CDBException& e)
// 	{
// 		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
// 			"Database exception, Error Code: %d; Message: %s", e.getNativeCode(), e.getReason().c_str() );
// 		
// 		nRetCode = ER_MYSQL_GONE_AWAY;
// 		sleep(TIME_SECOND_TWO);
// 		_InitDupChkEnv();
// 	}	
// 	catch (...)
// 	{
// 		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
// 			"Error Code: %d; Message: %s", Err_Unknown, defErrMsg(Err_Unknown) );	
// 
// 		nRetCode = ER_MYSQL_GONE_AWAY;
// 		sleep(TIME_SECOND_TWO);
// 		_InitDupChkEnv();
// 	}	

	return nRetCode;
}

int    CProcessRcvMsg::_InitSID2DB()
{	
	int nSrvID = 0;
	ulong64 ulSID = 0;
	int nRetCode = Ret_Success;
	
// 	if (!m_pDupChkDb) 
// 	{
// 		nRetCode = Ret_Failed;
// 		return nRetCode;
// 	}
// 
// 
// 	std::string strSql = m_pDupChkDb->prepareSQLStatement(ENQ_SQL_INSERT_ADD_MSGSID_7002,  m_strRcvName,  nSrvID, m_strQName, ulSID);
// 	
// 	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
// 		"_InitSID2DB() [strSql=%s]", strSql.c_str());
// 
// 	try
// 	{
// 		m_pDupChkDb->connect();
// 		m_pDupChkDb->executeModification(strSql);	
// 		
// 		m_mapDupChkSID[nSrvID] = ulSID;	
// 	}
// 	catch (const CDBException& e)
// 	{
// 		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
// 			"Database exception, Error Code: %d; Message: %s", e.getNativeCode(), e.getReason().c_str() );
// 
// 		nRetCode = e.getNativeCode();
// 		sleep(TIME_SECOND_FIVE);			
// 	}	
// 	catch (...)
// 	{
// 		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
// 			"Error Code: %d; Message: %s", Err_Unknown, defErrMsg(Err_Unknown) );	
// 
// 		nRetCode = ER_MYSQL_GONE_AWAY;
// 		sleep(TIME_SECOND_FIVE);			
// 		_InitDupChkEnv();
// 	}
	
	return nRetCode;	
}

int   CProcessRcvMsg::_MsgDupChk(int nSrvID, ulong64 ulSID, mapDupChkSIDIterT& rIter)
{
	int nRetCode = Ret_Success;

	// check message server id
	rIter = m_mapDupChkSID.find(nSrvID);
	if (rIter == m_mapDupChkSID.end())
	{
		return Ret_Failed;
	}
	if (ulSID <= rIter->second)
	{
		nRetCode = Ret_Exit;
	}

	return nRetCode;
}

// insert into aq_sys_queue_e(q_name, msg_sid, msgid, priority, user_data) values ('%s', %s, '%s', %s, '%s'); 
void   CProcessRcvMsg::_Write2ExceptionQ(CMessage* pMsg)
{
// 	if (!m_pEnqDatabase) return;
// 	std::string strMsgID, strData;
// 	ulong64 ulSID = 0, nPriority = 1;
// 
// 	ulSID = pMsg->getMsgSeqID();
// 	strMsgID = pMsg->getMsgID();
// 	pMsg->getPayload(strData);
// 
// 	char* pEscapedPayload = CUtilityFun::escapeSQLStr((char*)strData.c_str(), "'", "''");
// 	strData = pEscapedPayload; free(pEscapedPayload); pEscapedPayload = 0;
// 
// 	std::string strSql = m_pEnqDatabase->prepareSQLStatement(ENQ_SQL_INSERT_ADD_MSGSID_7003,  m_strQName, ulSID, strMsgID, nPriority, strData);
// 
// 	try
// 	{
// 		m_pEnqDatabase->connect();
// 		m_pEnqDatabase->executeModification(strSql);
// 	}
// 	catch (const CDBException& e)
// 	{
// 		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
// 			"Database exception, Error Code: %d; Message: %s", e.getNativeCode(), e.getReason().c_str() );
// 	}	
// 	catch (...)
// 	{
// 		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
// 			"Error Code: %d; Message: %s", Err_Unknown, defErrMsg(Err_Unknown) );
// 	}
}



int CProcessRcvMsg::continueWork()
{
	int nFunRes = 0;
	TA_Base_Core::ThreadGuard guard(m_LockControlWorkState);

	m_nControlWorkState =   ControlWorkState_DoingWork;


	return nFunRes;
}

int CProcessRcvMsg::pauseWork()
{
	int nFunRes = 0;
	TA_Base_Core::ThreadGuard guard(m_LockControlWorkState);

	m_nControlWorkState =   ControlWorkState_PauseWork;

	return nFunRes;
}

int CProcessRcvMsg::getLastEnqSeqID(ulong64& ulLastEnqSeqID)
{
	int nFunRes = 0;
	
	ulLastEnqSeqID = m_ulLastEnqSeqID;
	
	return nFunRes;
}
int CProcessRcvMsg::getLastTotoalEnq(ulong64& ulTotoalEnq)
{
	int nFunRes = 0;

	ulTotoalEnq = m_ulTotalEnqMsgCnt;

	return nFunRes;
}

bool CProcessRcvMsg::isEnqHealth()
{
	bool bRes  = false;	


	if (STATE_CODE_WAITFOR_ENQREQ == m_nProcessState
		|| STATE_CODE_RCV_MSG == m_nProcessState
		|| STATE_CODE_GETSID_FROMDB == m_nProcessState
		|| STATE_CODE_ENQ_MSG == m_nProcessState
		|| STATE_CODE_UPDSID_2DB == m_nProcessState
		|| STATE_CODE_SEND_ACK == m_nProcessState
		|| STATE_CODE_FINISH_TRANSACTION == m_nProcessState
		)
	{
		bRes = true;
	}
	else
	{
		bRes = false;
	}

	return bRes;
}




NS_END(TA_DB_Sync)



