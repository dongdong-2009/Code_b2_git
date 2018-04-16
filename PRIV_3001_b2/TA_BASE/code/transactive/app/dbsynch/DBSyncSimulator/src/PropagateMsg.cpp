//////////////////////////////////////////////////////////////////////
///		@file		PropagateMsg.cpp
///		@author		zhilin,ouyang
///		@date		2012-01-29 14:44:41
///
///		@brief	    implementation of class CPropagateMsg.. 
///                 each consumer per thread in the system.
///                 
//////////////////////////////////////////////////////////////////////
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

#if !defined( WIN32 )
#include <sys/socket.h>
#else
#include <winsock2.h>
#endif // !WIN32

#include "TcpClientManager.h"
#include "DBException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DbSyncException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DbConnectionFailed.h"
#include "core/uuid/src/TAuuid.h"

NS_BEGIN(TA_DB_Sync)

static const uint32  s_uDeqCnt = 3000;   // deq 3K once from MySQL DB

CPropagateMsg::CPropagateMsg(IObserverRcvMsg* pObserver)
: m_bTerminate(false)
, m_bPropagated(false)
//, m_pListener(0)
// , m_pMsgConsumer(0)
, m_uPropagateMsgNum(0)
, m_nTcpState(TCP_DISCONNECTED)
, m_pMsgObserver(pObserver)
, m_ulMsgSent(0)
, m_nDBType(enumMysqlDb)
, m_nPropagateFlag(STATE_CODE_INIT_ENV)
// , m_pDeqDatabase(NULL)
, m_pProTHD(NULL)
, m_pConAck(NULL)
, m_pEnqAck(NULL)
, m_pTimer(NULL)
, m_pEnqReqProcessor(NULL)
, m_nLogCount(0)
, m_nDBSrvID(0)
, m_nMsgSrvID(0)
, m_nReconTimes(0)
, m_sendName("")
{	
	m_pMsgItemHelper = new CMsgItemHelper();
	//create enqueue thread, will be started soon
	m_pEnqReqProcessor = new CProcessRcvMsg();	
	m_pTimer = new CTimeMonitor(); //log and connection monitor
	m_nDeqWorkIndex = 0;
	m_nSendWorkIndex = 0;
	m_pDataManager = new CDbSynTestCreateMsgManager();

	m_pEnqReqTotal = new ENQREQEX();
	_FreeMsgCon(m_pEnqReqTotal->m_lstDBMsg);

	m_nDBSrvID = g_SynProgramSimInfo.m_n_Default_SRVID;// test vaule 1
	m_nMsgSrvID = m_nDBSrvID;//test value 1

	m_WorkState = WorkState_Begin;
	m_nControlWorkState =   ControlWorkState_DoingWork;
	m_ullastMsgSeqID = 0;
	_UpdateSimuClientInfo();
}

CPropagateMsg::~CPropagateMsg(void)
{
	if (NULL != m_pDataManager)
	{
		m_pDataManager->stopCreateMsg();
		DEF_DELETE(m_pDataManager);
	}

	if (NULL != m_pEnqReqTotal)
	{
		_FreeMsgCon(m_pEnqReqTotal->m_lstDBMsg);
		DEF_DELETE(m_pEnqReqTotal);
	}


	_ClearUpEnv();	

	m_pEnqReqProcessor->terminateAndWait();	

	//m_strServerID have been removed in run function
	//defRemoveTcpClient(m_strServerID);

	DEF_DELETE(m_pMsgItemHelper);
	DEF_DELETE(m_pTimer);
	DEF_DELETE(m_pEnqReqProcessor);
}
int  CPropagateMsg::setPropagateTHD(PRO_THD* pProThd)
{
	FUNCTION_ENTRY("setPropagateTHD");
	int nRetCode = Ret_Success;

	if (!pProThd)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	m_pProTHD = pProThd;


	m_sendName=pProThd->m_Sender.getName().c_str();
	
	m_pDataManager->m_nClientIndex = m_nClientIndex;
	m_pDataManager->m_strAgtName = m_pProTHD->m_Sender.getName();
	m_pDataManager->setAllParam();
	m_pDataManager->startCreateMsg();
	m_pDataManager->pauseWork();


	_UpdateSimuClientInfo();
	FUNCTION_EXIT;
	return nRetCode;
}

int  CPropagateMsg::processRcvFrame(PCONACK pConAck)
{
	FUNCTION_ENTRY("processRcvFrame");
	int nRetCode = Ret_Success;

	if ( !pConAck )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	{
		ThreadGuard  guard(m_tLockAcks);
		m_pConAck = pConAck;
		m_strAckPkgID = m_pConAck->m_strPkgID;
	}	

	FUNCTION_EXIT;
	return nRetCode;
}

int   CPropagateMsg::processRcvFrame(PENQACK pEnqAck)
{
	FUNCTION_ENTRY("processRcvFrame");
	int nRetCode = Ret_Success;

	if ( !pEnqAck )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
	
	{
		ThreadGuard  guard(m_tLockAcks);
		m_pEnqAck = pEnqAck;
		m_strAckPkgID = m_pEnqAck->m_strPkgID;
	}	

	FUNCTION_EXIT;
	return nRetCode;
}

int   CPropagateMsg::processTcpEvent( int nStateCode )
{
	FUNCTION_ENTRY("processTcpEvent");
	int nRet = Ret_Success;

	if (m_nTcpState != nStateCode)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"Old Tcp Connection State Code: %d; New Tcp connection State Code: %d", m_nTcpState,  nStateCode);
		m_nTcpState = nStateCode;		
	}

	FUNCTION_EXIT;
	return nRet;
}

void  CPropagateMsg::run()
{	
	FUNCTION_ENTRY("run");	  
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		"start one thread: Propagate, [Queue Name: %s][SubID: %d][Receiver Name: %s][Receiver Address: %s]", 
		m_pProTHD->m_szQName,
		m_pProTHD->m_nSubID, 
		m_Receiver.getName().c_str(),
		m_Receiver.getAddr().c_str());
	//start enqueue thread
	m_pEnqReqProcessor->start();
	int uPrintLog = 0;

	while ( !m_bTerminate )
	{	
		_ThreadJob();
		_PrintMsgLog(uPrintLog, m_nPropagateFlag);
		
		//for 
		_UpdateSimuClientInfo();
		
		sleep( defDefaultListenTime );
	}

	//should not delete m_pDataManager here
	//multi thread env will use m_pDataManager in other funciton
	/*
	if (NULL != m_pDataManager)
	{
		m_pDataManager->stopCreateMsg();
		DEF_DELETE(m_pDataManager);
	}*/

	_UpdateSimuClientInfo();
	_PrintDataInfoToLog();

	////fisrt use 	defRemoveTcpClient(m_strServerID);
	defRemoveTcpClient(m_strServerID);
	if (m_bTerminate)
	{		
		m_pMsgObserver->unRegisterHandler(m_strServerID, ComProtocol_STOC_ENQ_REQ);
		m_pMsgObserver->unRegisterHandler(m_strServerID, ComProtocol_STOC_CON_ACK | ComProtocol_STOC_ENQ_ACK);
	}

	FUNCTION_EXIT;
}

int CPropagateMsg::_PrintDataInfoToLog()
{
	int nFunRes = 0;

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"DataInfo[ClientIndex=%d][ServerID=%s][FirstMsgSeqID=%llu][lastMsgSeqID=%llu][TotalSend=%llu][TotalGetAck=%llu]", 
		m_nClientIndex, m_strServerID.c_str(), g_SynProgramSimInfo.m_n_StartSeqNum,
		m_ullastMsgSeqID, m_ullastMsgSeqID - g_SynProgramSimInfo.m_n_StartSeqNum + 1,
		m_ulMsgSent); 	

	return nFunRes;
}

void CPropagateMsg::terminate()
{
	FUNCTION_ENTRY("terminate");	

	m_bTerminate = true;		

	FUNCTION_EXIT;
}

void  CPropagateMsg::_ThreadJob()
{

	if (m_nDeqWorkIndex > SERVER_LIMIT_LOG_VLAUE_MAX
		|| m_nSendWorkIndex > SERVER_LIMIT_LOG_VLAUE_MAX)
	{
		m_nDeqWorkIndex = 0;
		m_nSendWorkIndex = 0;
	}

	switch (m_nPropagateFlag)
	{
	case STATE_CODE_INIT_ENV:
		_ProcessInitEnv();
		break;
	case STATE_CODE_CONNTOSRV:
		//if user pauseWork, CPropagateMsg thread will not work
		if (ControlWorkState_DoingWork != m_nControlWorkState)
		{
			sleep( defDefaultListenTime ); 
		}
		else
		{
			_ProcessCon2Srv();
		}
		break;
	case STATE_CODE_SENDINFO:
		_ProcessSendInfo();
		break;
	case STATE_CODE_INITDEQ:
		_ProcessInitDeq();
		break;
	case STATE_CODE_DEQMSG:
		//if user pauseWork, CPropagateMsg thread will not work
		if (ControlWorkState_DoingWork != m_nControlWorkState)
		{
			sleep( defDefaultListenTime ); 
		}
		else
		{
			_ProcessDeqMsg();
		}
		break;
	case STATE_CODE_SENDMSGREQ:
		//if user pauseWork, CPropagateMsg thread will not work
		if (ControlWorkState_DoingWork != m_nControlWorkState)
		{
			sleep( defDefaultListenTime ); 
		}
		else
		{
			_ProcessSendMsgReq();
		}
		break;
	case STATE_CODE_WAITFORENQACK:
		_ProcessWaitForEnqAck();
		break;
	case STATE_CODE_WAITFORCONACK:
		_ProcessWaitForConAck();
		break;
	case STATE_CODE_RCVENQACK:
		_ProcessRcvEnqAck();
		break;
	case STATE_CODE_RCVCONACK:
		_ProcessRcvConAck();
		break;
	case STATE_CODE_FINISH_TRANSACTION:
		_ProcessFinishTransaction();
		break;
	case STATE_CODE_FINISH_ALLWork:
		break;	
	default:
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Propagate_Code, defErrMsg(Err_Invalid_Propagate_Code));
	}
}

void  CPropagateMsg::_ProcessInitEnv()
{
	_ClearUpEnv();
	_GetSrvID();

	if (m_pProTHD->m_lstRecver.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Receiver_Lst_Null, defErrMsg(Err_Receiver_Lst_Null));
		return;
	}

	m_Receiver = *(m_pProTHD->m_lstRecver.front());
	_ParseSrvAddr(m_Receiver.getAddr());

	if (m_strSrvAddr.empty() || m_strSrvPort.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Addr, defErrMsg(Err_Invalid_Addr));
		return;
	}

	m_strServerID = defCreateNewTcpClient(m_strSrvAddr, m_strSrvPort, m_pProTHD->m_szQNameForServerID);
	if (m_strServerID.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Create_TCPCon_Failed, defErrMsg(Err_Create_TCPCon_Failed));
		return;
	}

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[m_strServerID=%s]", m_strServerID.c_str());


	m_pEnqReqProcessor->setServerID(m_strServerID);	
	m_pEnqReqProcessor->setRcvPara(m_pProTHD->m_szQName, m_Receiver.getName());
	if (!m_pMsgObserver)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	m_pMsgObserver->registerHandler(m_strServerID, ComProtocol_STOC_CON_ACK | ComProtocol_STOC_ENQ_ACK ,this);
	m_pMsgObserver->registerHandler(m_strServerID, ComProtocol_STOC_ENQ_REQ, m_pEnqReqProcessor);

	m_nPropagateFlag = STATE_CODE_CONNTOSRV;
}

void  CPropagateMsg::_ProcessCon2Srv()
{
	if (m_strServerID.empty())
	{
		m_nPropagateFlag = STATE_CODE_INIT_ENV;
	}
	int nRetCode = Ret_Success;
	uint32 uiLog = 0;	


    //connect to server
	nRetCode = defConnect2Srv(m_strServerID);
	if ( Ret_Success != nRetCode )	  //fail to connect to server
	{		
		if (m_nReconTimes > defChannelSrvTryTimes && m_pProTHD->m_lstRecver.size() > 1)
		{
			// find next available receiver address
			_GetWorkAgent();

			// remove the previous client TCP connection and create a new one
			defRemoveTcpClient(m_strServerID);	

			if (!m_pMsgObserver)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
				TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
			}
			m_pMsgObserver->unRegisterHandler(m_strServerID, ComProtocol_STOC_CON_ACK | ComProtocol_STOC_ENQ_ACK);
			m_pMsgObserver->unRegisterHandler(m_strServerID, ComProtocol_STOC_ENQ_REQ);

			_ParseSrvAddr(m_Receiver.getAddr());
			if (m_strSrvAddr.empty() || m_strSrvPort.empty())
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"Error Code: %d, Error message: %s", Err_Invalid_Addr, defErrMsg(Err_Invalid_Addr));
				return;
			}

			m_strServerID = defCreateNewTcpClient(m_strSrvAddr, m_strSrvPort, m_pProTHD->m_szQNameForServerID);
			if (m_strServerID.empty())
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"Error Code: %d, Error message: %s", Err_Create_TCPCon_Failed, defErrMsg(Err_Create_TCPCon_Failed));
				return;
			}

			// remove the previous one first and then register the new one
			m_pMsgObserver->registerHandler(m_strServerID, ComProtocol_STOC_CON_ACK | ComProtocol_STOC_ENQ_ACK ,this);
			m_pMsgObserver->registerHandler(m_strServerID, ComProtocol_STOC_ENQ_REQ, m_pEnqReqProcessor);

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Retry connect to server: %s, Times: %d", m_Receiver.getAddr().c_str(),  m_nReconTimes);
		}

		m_nReconTimes > defChannelSrvTryTimes ? m_nReconTimes = 1 : m_nReconTimes++;

		//connect to server error
		m_pDataManager->pauseWork();
		_UpdateSimuClientInfo();

		sleep(TIME_SECOND_TWO);

		
	}
	else
	{
		m_nReconTimes = 0;
		m_pEnqReqProcessor->setServerID(m_strServerID);							
		m_nPropagateFlag = STATE_CODE_SENDINFO;
	}		

}

void  CPropagateMsg::_ProcessSendInfo()
{
	if (!m_pProTHD || !m_pMsgItemHelper )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	std::string strUUID36 = TA_Base_Core::TA_Uuid::generateUUID();
	std::string strUUID32 = CUtilityFun::strRemoveUnNeedChar(strUUID36); 
	strUUID32   = CUtilityFun::strtoupper((char*)strUUID32.c_str());
	m_strReqPkgID	= strUUID32;

	int  nRetCode = Ret_Failed;
	CONREQ conReq;
	PMSGPKG pMsgPkg      = NULL;
	conReq.m_strAgtName  = (m_pProTHD->m_Sender).getName();
	conReq.m_strQName    = m_pProTHD->m_szQName;
	conReq.m_strHostName = _GetLocHostName();
	conReq.m_strPkgID    = m_strReqPkgID;
//	conReq.m_nOServerID  = 2;   // just for test
	conReq.m_nOServerID  = m_nDBSrvID;
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
		"Send handshake to: %s, Agent Name: %s, Queue Name: %s, PackageID: %s", m_Receiver.getAddr().c_str(), m_pProTHD->m_Sender.getName().c_str(), 
		m_pProTHD->m_szQName, m_strReqPkgID.c_str());	

	m_pMsgItemHelper->setCTOSConReq(&conReq, pMsgPkg);	

	ThreadGuard  guard(m_tLockAcks);
	DEF_DELETE(m_pConAck);  // clean connection ack before sending connection request
	nRetCode = defSendDataToServer(m_strServerID, pMsgPkg);	
	if ( Ret_Success == nRetCode )
	{
		m_nPropagateFlag = STATE_CODE_WAITFORCONACK;
	}
	else
	{
		if (TCP_DISCONNECTED == m_nTcpState)
			m_nPropagateFlag = STATE_CODE_CONNTOSRV;

		sleep(TIME_SECOND_TWO);
	}	

}

void  CPropagateMsg::_ProcessInitDeq()
{
	if (!m_pProTHD)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
	if (NULL == m_pDataManager)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"NULL == m_pDataManager, can't create data ");

		return;
	}

	//connect to server error
	m_pDataManager->continueWork();
	_UpdateSimuClientInfo();



	_InitDeqEnv();	

	m_nPropagateFlag = STATE_CODE_DEQMSG;
}

void  CPropagateMsg::_ProcessDeqMsg()
{
	// 	TA_ASSERT(m_pDeqDatabase, defErrMsg(Err_Invalid_Ptr));
	// 	TA_ASSERT(m_pMsgConsumer, defErrMsg(Err_Invalid_Ptr));
	//TA_ASSERT(m_pListener, defErrMsg(Err_Invalid_Ptr));
	TA_ASSERT(m_pProTHD, defErrMsg(Err_Invalid_Ptr));

	int nFunRes = 0;
	bool bRetCode = false;
	int  nRetCode = Ret_Failed;
	try
	{
		// check if the TCP connected or not
		if (TCP_DISCONNECTED == m_nTcpState)
		{
			m_nPropagateFlag = STATE_CODE_CONNTOSRV;
			return;
		}
		// check if there still something to send from previous
		if (!m_stEnqReq.m_lstDBMsg.empty()) 
		{
			m_nPropagateFlag = STATE_CODE_SENDMSGREQ;
			return;
		}

		//bRetCode = m_pListener->listen();
		bRetCode = true;
		if (bRetCode)
		{
			// 			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			// 				"Begin Deq message for SubID: %d, Queue name: %s",  m_pProTHD->m_nSubID, m_pProTHD->m_szQName);
			m_pTimer->getTimeDiffByMilSecond(PROPAGATE_TIMER_LOG_EACHDEQWORK);
			m_pTimer->getTimeDiffByMilSecond(PROPAGATE_TIMER_LOG_DEQDATA);	

			//  			if ( enumOracleDb == m_nDBType)
			//  				nRetCode = m_pMsgConsumer->deqMsg(m_lstSendMsg, defMaxPackageSize);
			//  			else
			//  				nRetCode = m_pMsgConsumer->deqMsg(m_lstSendMsg, s_uDeqCnt, m_pProTHD->m_nSubID);
			// 			switch ( nRetCode )
			// 			{
			// 			case ER_MYSQL_GONE_AWAY:
			// 				m_nPropagateFlag = STATE_CODE_INITDEQ;
			// 				break;
			// 			case Ret_Success:
			// 				m_nPropagateFlag = STATE_CODE_SENDMSGREQ;	
			// 				break;
			// 			default:
			// 				m_nPropagateFlag = STATE_CODE_DEQMSG;
			// 				break;
			//			}

			m_nStepDeqDataTime = m_pTimer->getTimeDiffByMilSecond(PROPAGATE_TIMER_LOG_DEQDATA);
			m_pTimer->reset(PROPAGATE_TIMER_LOG_DEQDATA);


			ulong64 ulSeqIDFrom = 0;
			ulong64 ulSeqIDTo = 0;
			nFunRes = 0;
			nFunRes = m_pDataManager->getMsgCTOSEnqReq(&m_pEnqReqTotal, ulSeqIDFrom, ulSeqIDTo);
			if (0 != nFunRes)
			{
				m_nPropagateFlag = STATE_CODE_DEQMSG;
			}
			else
			{
				m_nPropagateFlag = STATE_CODE_SENDMSGREQ;

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					"%s [End Deq message for SubID: %d][Message count: %d][Queue name: %s][m_nDeqWorkIndex=%d][m_nStepDeqDataTime=%d]",  
					m_sendName.c_str(), m_pProTHD->m_nSubID,  m_pEnqReqTotal->m_lstDBMsg.size(), m_pProTHD->m_szQName, m_nDeqWorkIndex, m_nStepDeqDataTime);

			}


		}
		else
		{
			m_nPropagateFlag = STATE_CODE_DEQMSG;
		}		
	}
	catch (TA_Base_Core::DataException& )
	{
		//continue to dequeue
		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Data exception...");
	}
	catch (const CDBException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", e.getNativeCode(), e.getReason().c_str());
		int nErrCode = e.getNativeCode();
		if ( ER_MYSQL_GONE_AWAY == nErrCode ||
			ER_MYSQL_CANNT_CONNECT == nErrCode)
			m_nPropagateFlag = STATE_CODE_INITDEQ;
	}
	catch (const DatabaseException& e)
	{
		//continue to dequeue
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error message: %s", e.what());
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception...");
		m_nPropagateFlag = STATE_CODE_INITDEQ;
	}
		
}

void  CPropagateMsg::_ProcessSendMsgReq()
{
	TA_ASSERT(m_pMsgItemHelper, defErrMsg(Err_Invalid_Ptr));

	int  nRet = Ret_Failed;
	uint32 uDataLen = 0;
	
	m_pTimer->getTimeDiffByMilSecond(PROPAGATE_TIMER_LOG_SENDDATA);
	m_nSendWorkIndex++;	

	if (TCP_DISCONNECTED == m_nTcpState)
	{
		m_nPropagateFlag = STATE_CODE_CONNTOSRV;
		return;
	}

	if (NULL == m_pEnqReqTotal || m_pEnqReqTotal->m_lstDBMsg.empty())
	{
		m_nPropagateFlag = STATE_CODE_DEQMSG;
		return;	 
	}
	else
	{
		m_nMsgSrvID = m_pEnqReqTotal->m_lstDBMsg.front()->getOServerID();
	}

	// 	if ( m_lstSendMsg.empty() )
	// 	{
	// 		m_nPropagateFlag = STATE_CODE_DEQMSG;
	// 		return;
	// 	}

	if (m_stEnqReq.m_lstDBMsg.empty())
	{
		lstDBMsgConIterT iter = m_pEnqReqTotal->m_lstDBMsg.begin();
		while (iter != m_pEnqReqTotal->m_lstDBMsg.end())
		{
			//DBSyncSimulator don't need check OServerID; TODO.
			if ((*iter)->getOServerID() != m_nMsgSrvID) break;
			
			if (uDataLen > g_SynProgramSimInfo.m_n_PackageSize) break;
			uDataLen += (*iter)->getPayloadSize();
			m_stEnqReq.m_lstDBMsg.push_back(*iter);

			iter++;
		}		
	}

	std::string strUUID36 = TA_Base_Core::TA_Uuid::generateUUID();
	std::string strUUID32 = CUtilityFun::strRemoveUnNeedChar(strUUID36); 
	strUUID32   = CUtilityFun::strtoupper((char*)strUUID32.c_str());
	m_strReqPkgID	= strUUID32;
	m_stEnqReq.m_strPkgID = m_strReqPkgID;
	PMSGPKG pMsgPkg = NULL;	

	m_pMsgItemHelper->setCTOSEnqReq(&m_stEnqReq, pMsgPkg);

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[start send message count: %ld][m_strPropagateMsgUUID32=%s][m_lstSendMsg.size()=%d]", 
		m_stEnqReq.m_lstDBMsg.size(), m_strReqPkgID.c_str(), m_pEnqReqTotal->m_lstDBMsg.size());

	ThreadGuard  guard(m_tLockAcks);
	DEF_DELETE(m_pEnqAck);   // clean reply ack before sending request
	nRet = defSendDataToServer(m_strServerID, pMsgPkg);
	if ( Ret_Success == nRet )
	{
		m_nPropagateFlag = STATE_CODE_WAITFORENQACK;		
		
		m_nStepSendDataTime = m_pTimer->getTimeDiffByMilSecond(PROPAGATE_TIMER_LOG_SENDDATA);
		m_pTimer->reset(PROPAGATE_TIMER_LOG_SENDDATA);

		LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"[SendWorkIndex=%d][StepSendDataTime=%d]", m_nSendWorkIndex, m_nStepSendDataTime);
	
		m_pTimer->getTimeDiffByMilSecond(PROPAGATE_TIMER_LOG_WAITACK);	
	}
	else
	{
		if (TCP_DISCONNECTED == m_nTcpState)
			m_nPropagateFlag = STATE_CODE_CONNTOSRV;
		else
			m_nPropagateFlag = STATE_CODE_SENDMSGREQ;
	}

	pMsgPkg = NULL;

}

void  CPropagateMsg::_ProcessWaitForEnqAck()
{
	uint32 uiTimeDiff = 0;
	if (TCP_DISCONNECTED == m_nTcpState) 
	{
		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"Tcp connection lost while sending message");

		m_nPropagateFlag = STATE_CODE_CONNTOSRV;
	}
	if (!m_pTimer) return;
	
	uiTimeDiff = m_pTimer->getTimeDiffByMilSecond(PROPAGATE_TIMER_REQACK);
	if (uiTimeDiff > TIME_MIN_FIFTEEN) 
	{
		m_pTimer->reset(PROPAGATE_TIMER_REQACK);
		m_nPropagateFlag = STATE_CODE_CONNTOSRV;	
	}

	if (m_pEnqAck)
	{
		m_nPropagateFlag = STATE_CODE_RCVENQACK;
		
		m_nStepWaitACKTime = m_pTimer->getTimeDiffByMilSecond(PROPAGATE_TIMER_LOG_WAITACK);
		m_pTimer->reset(PROPAGATE_TIMER_LOG_WAITACK); 
		LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"[SendWorkIndex=%d][StepWaitACKTime=%d]", m_nSendWorkIndex, m_nStepWaitACKTime);
	

	}
	
}

void  CPropagateMsg::_ProcessWaitForConAck()
{
	uint32 uiTimeDiff = 0;
	if (TCP_DISCONNECTED == m_nTcpState) 
	{
		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Tcp connection lost while sending handshake connection request info");

		m_nPropagateFlag = STATE_CODE_CONNTOSRV;
	}
	//if (!m_pTimer) return;
	uiTimeDiff = m_pTimer->getTimeDiffByMilSecond(PROPAGATE_TIMER_CONACK);
	if (uiTimeDiff > TIME_MIN_FIVE) 
	{
		m_pTimer->reset(PROPAGATE_TIMER_CONACK);
		m_nPropagateFlag = STATE_CODE_CONNTOSRV;
		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"Handshake time out, reconnect...");		
	}
	ThreadGuard  guard(m_tLockAcks);
	if (m_pConAck)
	{
		m_nPropagateFlag = STATE_CODE_RCVCONACK;
	}
}

void  CPropagateMsg::_ProcessRcvEnqAck()
{
	if ( !m_pEnqAck )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	if ( m_strReqPkgID == m_strAckPkgID && 
		 m_stEnqReq.m_lstDBMsg.size() == m_pEnqAck->m_nEnqReqACK )
	{
		m_ulMsgSent += m_pEnqAck->m_nEnqReqACK;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"[Success propagated to %s,  Message count: %ld][total sent messages: %llu][Package ID: %s]",
			m_Receiver.getName().c_str(), m_pEnqAck->m_nEnqReqACK, m_ulMsgSent, m_strReqPkgID.c_str());
		
		m_nPropagateFlag = STATE_CODE_FINISH_TRANSACTION;
			
		m_pTimer->getTimeDiffByMilSecond(PROPAGATE_TIMER_LOG_DEQCOMMIT);

	}
	else
	{
		switch (m_pEnqAck->m_nEnqReqACK)
		{
		case Err_EnqReqACkCode_DiskFull:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Error Code: %d, Error message: %s, Sent Count: %ld, Package ID: %s", 
				Err_DBServer_DiskFull, defErrMsg(Err_DBServer_DiskFull), m_stEnqReq.m_lstDBMsg.size(), m_strReqPkgID.c_str());

			sleep(TIME_MIN_TWO);
			m_nPropagateFlag = STATE_CODE_SENDMSGREQ;
			break;
		case Err_EnqReqACkCode_DBOffLine:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Error Code: %d, Error message: %s, Sent Count: %ld, Package ID: %s", 
				Err_DBOffLine, defErrMsg(Err_DBOffLine), m_stEnqReq.m_lstDBMsg.size(), m_strReqPkgID.c_str());

			sleep(TIME_SECOND_THIRTY);
			m_nPropagateFlag = STATE_CODE_SENDMSGREQ;
			break;
		case Err_EnqReqACkCode_EnqDisable:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Error Code: %d, Error message: %s, Sent Count: %ld, Package ID: %s", 
				Err_DBEnqOpt_Disable, defErrMsg(Err_DBEnqOpt_Disable), m_stEnqReq.m_lstDBMsg.size(), m_strReqPkgID.c_str());

			sleep(TIME_SECOND_THIRTY);
			m_nPropagateFlag = STATE_CODE_SENDMSGREQ;
			break;
		case Err_EnqReqACkCode_PLTypeInvalid:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Error Code: %d, Error message: %s, Sent Count: %ld, Package ID: %s", 
				Err_DBQ_PLT_Invalid, defErrMsg(Err_DBQ_PLT_Invalid), m_stEnqReq.m_lstDBMsg.size(), m_strReqPkgID.c_str());
			
			sleep(TIME_MIN_TWO);
			m_nPropagateFlag = STATE_CODE_SENDMSGREQ;
			break;
		default:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Error Code: %d, Error message: %s, Sent Count: %ld, Package ID: %s", 
				Err_Unknown, defErrMsg(Err_Unknown), m_stEnqReq.m_lstDBMsg.size(), m_strReqPkgID.c_str());

			sleep(TIME_SECOND_TEN);
			m_nPropagateFlag = STATE_CODE_SENDMSGREQ;
			break;
		}

	}

	if( m_pTimer )
		m_pTimer->reset(PROPAGATE_TIMER_REQACK);

	ThreadGuard  guard(m_tLockAcks);
	DEF_DELETE( m_pEnqAck );   // After processing, clean the reply ack.
}

void  CPropagateMsg::_ProcessRcvConAck()
{
	ThreadGuard  guard(m_tLockAcks);
	if ( !m_pConAck)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	if (m_strReqPkgID == m_strAckPkgID &&
		ConReqAckCode_Succeed == m_pConAck->m_nConReqAck)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"[HandShake success connected to server: %s][Package ID: %s]", m_Receiver.getName().c_str(), m_strReqPkgID.c_str());

		m_nPropagateFlag = STATE_CODE_INITDEQ;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"[HandShake failed,wrong package id,server id: %s][Package ID: %s]", m_Receiver.getName().c_str(),  m_strReqPkgID.c_str());

		m_nPropagateFlag = STATE_CODE_SENDINFO;
	}

	if( m_pTimer )
		m_pTimer->reset(PROPAGATE_TIMER_CONACK);

	 
	DEF_DELETE( m_pConAck );  // After processing, clean the reply ack.
}

void  CPropagateMsg::_ProcessFinishTransaction()
{
	// 	TA_ASSERT(m_pDeqDatabase, defErrMsg(Err_Invalid_Ptr));
	TA_ASSERT(m_pProTHD, defErrMsg(Err_Invalid_Ptr));

	try
	{
		std::string strConsumer;
		nPairT pairSID;
		pairSID.first = m_stEnqReq.m_lstDBMsg.front()->getMsgSeqID();
		pairSID.second = m_stEnqReq.m_lstDBMsg.back()->getMsgSeqID();
		m_ullastMsgSeqID = pairSID.second;
		// 		m_pDeqDatabase->connect();
		// 		m_pDeqDatabase->aqCommit(m_pProTHD->m_szQName, m_pProTHD->m_nSubID, strConsumer, pairSID);
		size_t uSentCnt = m_stEnqReq.m_lstDBMsg.size();
		for (size_t i = 0; i < uSentCnt; i++)
		{			
			if (NULL != m_pEnqReqTotal)
			{
				m_pEnqReqTotal->m_lstDBMsg.pop_front();
			}
			//m_lstSendMsg.pop_front();
		}
		_FreeMsgCon(m_stEnqReq.m_lstDBMsg);

		if (m_pEnqReqTotal->m_lstDBMsg.empty())
		{
			m_nPropagateFlag = STATE_CODE_DEQMSG;
		}
		else
		{
			m_nPropagateFlag = STATE_CODE_SENDMSGREQ;
		}

		m_nStepDeqCommitTime = m_pTimer->getTimeDiffByMilSecond(PROPAGATE_TIMER_LOG_DEQCOMMIT);
		m_pTimer->reset(PROPAGATE_TIMER_LOG_DEQCOMMIT);
		LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"[SendWorkIndex=%d][StepDeqCommitTime=%d]", m_nSendWorkIndex, m_nStepDeqCommitTime); 

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"[Finish the transaction for Queue: %s, Package ID: %s, firstMsgSeqID=%llu, lastMsgSeqID=%llu, message count: %d]", 
			m_pProTHD->m_szQName, m_strReqPkgID.c_str(), pairSID.first, pairSID.second, uSentCnt);


		//check if this client send all data to server
		if (m_ullastMsgSeqID >= g_SynProgramSimInfo.m_n_EndSeqNum)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"[ClientIndex=%d][AgentName=%s][lastMsgSeqID=%llu][m_n_EndSeqNum=%llu] Finish send data",
				m_nClientIndex, m_pProTHD->m_Sender.getName().c_str(),
				m_ullastMsgSeqID, g_SynProgramSimInfo.m_n_EndSeqNum);

			
			m_WorkState = WorkState_End;
			m_nPropagateFlag = STATE_CODE_FINISH_ALLWork;
		}
		_UpdateSimuClientInfo();



		//
		if (STATE_CODE_DEQMSG == m_nPropagateFlag)
		{
			m_nEachDeqWorkUse = m_pTimer->getTimeDiffByMilSecond(PROPAGATE_TIMER_LOG_EACHDEQWORK);
			m_pTimer->reset(PROPAGATE_TIMER_LOG_EACHDEQWORK);
			LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"[DeqWorkIndex=%d][EachDeqWorkUse=%d]", m_nDeqWorkIndex, m_nEachDeqWorkUse); 

			m_nDeqWorkIndex++;
		}

		//before Send nest EnqRequest sleep m_n_IntervalTime time
		//int nIntervalTime  = g_SynProgramSimInfo.m_n_IntervalTime;
		//TA_Base_Core::Thread::sleep(nIntervalTime);


	}	
	catch (const CDBException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
		"Error Code: %d, Error message: %s", e.getNativeCode(), e.getReason().c_str());

		int nErrCode = e.getNativeCode();
		if ( ER_MYSQL_GONE_AWAY == nErrCode ||
			 ER_MYSQL_CANNT_CONNECT == nErrCode)
			_InitDeqEnv();

		m_nPropagateFlag = STATE_CODE_FINISH_TRANSACTION;
	}
	catch (const DatabaseException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error message: %s", e.what());

		m_nPropagateFlag = STATE_CODE_FINISH_TRANSACTION;
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception...");

		_InitDeqEnv();

		m_nPropagateFlag = STATE_CODE_FINISH_TRANSACTION;
	}	
}

void  CPropagateMsg::_ParseSrvAddr(const std::string& strAddr)
{
	if ( !m_pProTHD )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
	if (strAddr.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Server_IP_Addr_Not_Found, defErrMsg(Err_Server_IP_Addr_Not_Found));
		return;
	}

	std::string::size_type nPos = strAddr.find_first_of(defConfig_DbConStrTag);
	if ( std::string::npos == nPos )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Addr, defErrMsg(Err_Invalid_Addr));
		TA_THROW(TA_Base_Core::SrvInfoNotFound(Err_Invalid_Addr, defErrMsg(Err_Invalid_Addr)));
	}
	std::string strSubTemp(strAddr.substr(0, nPos));
	m_strSrvAddr   = trim(strSubTemp);
	strSubTemp     =  strAddr.substr(nPos + 1);
	m_strSrvPort   = trim(strSubTemp);
}

std::string CPropagateMsg::_GetLocHostName()
{
	const int maxNameSize = 1025;
	char unqualifiedHostName[ maxNameSize ];
	int lvError = gethostname( unqualifiedHostName, sizeof( unqualifiedHostName ) );
#if defined( WIN32 )
	if( WSANOTINITIALISED == WSAGetLastError() )
	{
		//
		// The Windows sockets library is not smart enough to figure out
		// that it should be automatically started, therefore we must start
		// Windows sockets and then try again.
		//
		WORD wVersionRequested;
		WSADATA wsaData;
		int err;
		wVersionRequested = MAKEWORD( 2, 2 );
		err = WSAStartup( wVersionRequested, &wsaData );
		if( 0 != err )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Error Code: %d, Error message: %s", Err_Failed_GetHostName, defErrMsg(Err_Failed_GetHostName));
		}

		// Try again.
		lvError = gethostname( unqualifiedHostName, sizeof( unqualifiedHostName ) );
	}
#endif // defined( WIN32 )
	if( 0 != lvError )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Failed_GetHostName, defErrMsg(Err_Failed_GetHostName));
	}
	return std::string( unqualifiedHostName );
}

template<class MsgConT>
void  CPropagateMsg::_FreeMsgCon(MsgConT& rMsgCon)
{
	typename MsgConT::iterator  iter = rMsgCon.begin();

	while ( iter != rMsgCon.end() )
	{
		DEF_DELETE(*iter);
		iter++;
	}
	rMsgCon.clear();
}

void  CPropagateMsg::_ClearUpEnv()
{
	m_strReqPkgID.clear();
	m_strAckPkgID.clear();
	m_stEnqReq.m_lstDBMsg.clear();

	if (NULL != m_pEnqReqTotal)
	{
		_FreeMsgCon(m_pEnqReqTotal->m_lstDBMsg);
		DEF_DELETE(m_pEnqReqTotal);
	}

	DEF_DELETE(m_pConAck);
	DEF_DELETE(m_pEnqAck);
	// 	DEF_DELETE(m_pDeqDatabase);
	// 	DEF_DELETE(m_pMsgConsumer);
	//DEF_DELETE(m_pListener);
}

void  CPropagateMsg::_InitDeqEnv()
{
	//DEF_DELETE(m_pListener);
	// 	DEF_DELETE(m_pMsgConsumer);
	// 	DEF_DELETE(m_pDeqDatabase);

	// 	m_pDeqDatabase = new CDatabaseImpl(g_system_variables.m_SrcDbConStr);
	// 	m_pMsgConsumer = new CConsumerImpl(m_pDeqDatabase);
	//m_pListener    = new CListener(m_pDeqDatabase, m_pProTHD->m_szQName);

	// 	if (!m_pMsgConsumer || !m_pDeqDatabase)
	// 	{
	// 		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
	// 			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
	// 		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	// 	}

	//m_nDBType      = m_pDeqDatabase->getDbServerType();
	m_nDBType = 2;//1-Oracle 2-Mysql

	// 	m_pMsgConsumer->setPayloadType(m_pProTHD->m_nPayloadType);   //MSGITEM_PYTP_OBJ
	// 	m_pListener->setSubID(m_pProTHD->m_nSubID);
	// 	m_pListener->setListenTimeOut(MSG_LISTEN_NOWAIT);
	// 	m_pMsgConsumer->setQueueName(m_pProTHD->m_szQName);
	// 	m_pMsgConsumer->setDeqMode(DEQ_REMOVE);
	// 	m_pMsgConsumer->setPositionOfMsg(DEQ_FIRST_MSG);
	// 	m_pMsgConsumer->setAgent(m_Receiver);
	// 	m_pMsgConsumer->setPayloadType(m_pProTHD->m_nPayloadType);


}

void  CPropagateMsg::_GetWorkAgent()
{
	lstAgentContIterT iter = m_pProTHD->m_lstRecver.begin();
	while (iter != m_pProTHD->m_lstRecver.end())
	{
		if (*(*iter) == m_Receiver) 
		{
			if (m_pProTHD->m_lstRecver.end() == ++iter)					
				m_Receiver = *(m_pProTHD->m_lstRecver.front());					
			else
				m_Receiver = *(*iter);
			break;
		}
		
		iter++;
	}

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
		"Current Receiver: %s, Address: %s, Receiver list size: %d",  m_Receiver.getName().c_str(),  m_Receiver.getAddr().c_str(), m_pProTHD->m_lstRecver.size());

}

void  CPropagateMsg::_GetSrvID()
{
	// 	if (m_pDeqDatabase)
	// 	{
	// 		m_nDBSrvID = m_pDeqDatabase->getDBServerID();
	// 	}
	// 	else
	// 	{		
	// 		m_pDeqDatabase = new CDatabaseImpl(g_system_variables.m_SrcDbConStr);
	// 		m_pDeqDatabase->connect();
	// 		m_nDBSrvID = m_pDeqDatabase->getDBServerID();
	// 	}

	m_nDBSrvID = g_SynProgramSimInfo.m_n_Default_SRVID;// test vaule 1
	m_nMsgSrvID = m_nDBSrvID;//test value 1
}

void  CPropagateMsg::_PrintMsgLog(int& nPrintInterval, int nState)
{
	nPrintInterval++;
	if (nPrintInterval > defDefaultMsgPrintTimes)
	{
		nPrintInterval = 0;
		switch (nState)
		{
		case STATE_CODE_INIT_ENV:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Propagate state: %s, QName: %s, Receiver: %s",  "STATE_CODE_INIT_ENV", m_pProTHD->m_szQName, m_Receiver.getName().c_str());
			break;
		case STATE_CODE_CONNTOSRV:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Propagate state: %s, QName: %s, Receiver: %s",  "STATE_CODE_CONNTOSRV", m_pProTHD->m_szQName, m_Receiver.getName().c_str());
			break;
		case STATE_CODE_SENDINFO:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Propagate state: %s, QName: %s, Receiver: %s",  "STATE_CODE_SENDINFO", m_pProTHD->m_szQName, m_Receiver.getName().c_str());
			break;
		case STATE_CODE_INITDEQ:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Propagate state: %s, QName: %s, Receiver: %s",  "STATE_CODE_INITDEQ", m_pProTHD->m_szQName, m_Receiver.getName().c_str());
			break;
		case STATE_CODE_DEQMSG:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Propagate state: %s, QName: %s, Receiver: %s",  "STATE_CODE_DEQMSG", m_pProTHD->m_szQName, m_Receiver.getName().c_str());
			break;
		case STATE_CODE_SENDMSGREQ:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Propagate state: %s, QName: %s, Receiver: %s",  "STATE_CODE_SENDMSGREQ", m_pProTHD->m_szQName, m_Receiver.getName().c_str());
			break;
		case STATE_CODE_WAITFORENQACK:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Propagate state: %s, QName: %s, Receiver: %s",  "STATE_CODE_WAITFORENQACK", m_pProTHD->m_szQName, m_Receiver.getName().c_str());
			break;
		case STATE_CODE_WAITFORCONACK:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Propagate state: %s, QName: %s, Receiver: %s",  "STATE_CODE_WAITFORCONACK", m_pProTHD->m_szQName, m_Receiver.getName().c_str());
			break;
		case STATE_CODE_RCVENQACK:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Propagate state: %s, QName: %s, Receiver: %s",  "STATE_CODE_RCVENQACK", m_pProTHD->m_szQName, m_Receiver.getName().c_str());
			break;
		case STATE_CODE_RCVCONACK:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Propagate state: %s, QName: %s, Receiver: %s",  "STATE_CODE_RCVCONACK", m_pProTHD->m_szQName, m_Receiver.getName().c_str());
			break;
		case STATE_CODE_FINISH_TRANSACTION:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Propagate state: %s, QName: %s, Receiver: %s",  "STATE_CODE_FINISH_TRANSACTION", m_pProTHD->m_szQName, m_Receiver.getName().c_str());
			break;
		default:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Error Code: %d, Error message: %s", Err_Invalid_Propagate_Code, defErrMsg(Err_Invalid_Propagate_Code));

		}

	}
}

bool CPropagateMsg::isFinishWork()
{
	bool bRes = false;

	bool bPropagateFinish = false;
	bool bDataManagerFinish = false;


	if (WorkState_End == m_WorkState)
	{
		bPropagateFinish = true;
	}
	if (NULL != m_pDataManager)
	{
		bDataManagerFinish = m_pDataManager->isFinishWork();
	}
	else
	{
		bDataManagerFinish = true;
	}

	if (bPropagateFinish && bDataManagerFinish)
	{
		bRes = true;
	}
	else
	{
		bRes = false;
	}

	return bRes;
}


int CPropagateMsg::continueWork()
{
	int nFunRes = 0;
	TA_Base_Core::ThreadGuard guard(m_LockControlWorkState);

	if (NULL != m_pDataManager)
	{
		m_pDataManager->continueWork();
	}
	
	if (NULL != m_pEnqReqProcessor)
	{
		m_pEnqReqProcessor->continueWork();
	}
	
	m_nControlWorkState =   ControlWorkState_DoingWork;
	_UpdateSimuClientInfo();

	
	return nFunRes;
}

int CPropagateMsg::pauseWork()
{
	int nFunRes = 0;
	TA_Base_Core::ThreadGuard guard(m_LockControlWorkState);

	if (NULL != m_pDataManager)
	{
		m_pDataManager->pauseWork();
	}
	
	if (NULL != m_pEnqReqProcessor)
	{
		m_pEnqReqProcessor->pauseWork();
	}
	
	m_nControlWorkState =   ControlWorkState_PauseWork;
	
	_UpdateSimuClientInfo();

	return nFunRes;
}


int CPropagateMsg::getThreadStatus(std::string& strStatus)
{
	int nFunRes = 0;

	switch (m_nPropagateFlag)
	{
	case STATE_CODE_INIT_ENV:
		strStatus = "STATE_CODE_INIT_ENV";
		break;
	case STATE_CODE_CONNTOSRV:
		strStatus = "STATE_CODE_CONNTOSRV";
		break;
	case STATE_CODE_SENDINFO:
		strStatus = "STATE_CODE_SENDINFO";
		break;
	case STATE_CODE_INITDEQ:
		strStatus = "STATE_CODE_INITDEQ";
		break;
	case STATE_CODE_DEQMSG:
		strStatus = "STATE_CODE_DEQMSG";
		break;
	case STATE_CODE_SENDMSGREQ:
		strStatus = "STATE_CODE_SENDMSGREQ";
		break;
	case STATE_CODE_WAITFORENQACK:
		strStatus = "STATE_CODE_WAITFORENQACK";
		break;
	case STATE_CODE_WAITFORCONACK:
		strStatus = "STATE_CODE_WAITFORCONACK";
		break;
	case STATE_CODE_RCVENQACK:
		strStatus = "STATE_CODE_RCVENQACK";
		break;
	case STATE_CODE_RCVCONACK:
		strStatus = "STATE_CODE_RCVCONACK";
		break;
	case STATE_CODE_FINISH_TRANSACTION:
		strStatus = "STATE_CODE_FINISH_TRANSACTION";
		break;
	case STATE_CODE_FINISH_ALLWork:
		strStatus = "STATE_CODE_FINISH_ALLWork";
		break; 
	default:
		strStatus = "UNKNOWN";
		break;
	}

	
	return nFunRes;
}

int CPropagateMsg::getSimuClientInfo(SimuClientInfo& SynSimuClientInfo)
{
	int nFunRes = 0;
	TA_Base_Core::ThreadGuard guard(m_LockSynSimuClientInfo);

	SynSimuClientInfo = m_SynSimuClientInfo;
	return nFunRes;
}


int CPropagateMsg::_UpdateSimuClientInfo()
{
	int nFunRes = 0;
	std::string strPropagateMsgThreadStatus;
	ulong64     LastEnqSeqID = 0;
	ulong64     nTotoalEnq = 0;

	TA_Base_Core::ThreadGuard guard(m_LockSynSimuClientInfo);

	getThreadStatus(strPropagateMsgThreadStatus);

	//get   AgentName
	if (NULL != m_pProTHD)
	{
		m_SynSimuClientInfo.AgentName = m_pProTHD->m_Sender.getName();
	}
	else
	{
		m_SynSimuClientInfo.AgentName = "UNKNOWN_AGTNAME";
	}

	m_SynSimuClientInfo.LastDeqSeqID = m_ullastMsgSeqID;
	m_SynSimuClientInfo.totalDeq = m_ulMsgSent;

	//get   LastEnqSeqID
	if (NULL != m_pEnqReqProcessor)
	{
		m_pEnqReqProcessor->getLastEnqSeqID(LastEnqSeqID);
		m_pEnqReqProcessor->getLastTotoalEnq(nTotoalEnq);
	}
	else
	{
		LastEnqSeqID = 0;
		nTotoalEnq = 0;
	}
	m_SynSimuClientInfo.LastEnqSeqID = LastEnqSeqID;
	m_SynSimuClientInfo.totalEnq = nTotoalEnq;


	//get tcp state
	if (TCP_CONNECTED == m_nTcpState)
	{
		m_SynSimuClientInfo.IsTcpHealth = true;
	}
	else
	{
		m_SynSimuClientInfo.IsTcpHealth = false;
	}

	//get IsDeqHealth
	if (STATE_CODE_DEQMSG == m_nPropagateFlag 
		|| STATE_CODE_SENDMSGREQ == m_nPropagateFlag
		|| STATE_CODE_WAITFORENQACK == m_nPropagateFlag
		|| STATE_CODE_RCVENQACK == m_nPropagateFlag
		|| STATE_CODE_FINISH_TRANSACTION == m_nPropagateFlag)
	{
		m_SynSimuClientInfo.IsDeqHealth = true;
	}
	else
	{
		m_SynSimuClientInfo.IsDeqHealth = false;
	}

	//get   IsEnqHealth
	if (NULL != m_pEnqReqProcessor)
	{
		m_SynSimuClientInfo.IsEnqHealth = m_pEnqReqProcessor->isEnqHealth();
	}
	else
	{
		m_SynSimuClientInfo.IsEnqHealth = false;
	}

	//get Status
	m_SynSimuClientInfo.Status = Normal;

	if (ControlWorkState_PauseWork == m_nControlWorkState)
	{
		m_SynSimuClientInfo.Status = Pause;	
	}
	else if (WorkState_End == m_WorkState)
	{
		m_SynSimuClientInfo.Status = Finished;
	}
	else if (false == m_SynSimuClientInfo.IsTcpHealth
		|| false == m_SynSimuClientInfo.IsDeqHealth
		|| false == m_SynSimuClientInfo.IsEnqHealth)
	{
		m_SynSimuClientInfo.Status = Error;	
	}
	else
	{
		m_SynSimuClientInfo.Status = Normal;
	}



	return nFunRes;
}


NS_END(TA_DB_Sync)
