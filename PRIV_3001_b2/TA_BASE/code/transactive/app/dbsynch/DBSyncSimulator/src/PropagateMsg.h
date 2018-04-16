//////////////////////////////////////////////////////////////////////
///		@file		PropagateMsg.h
///		@author		zhilin,ouyang
///		@date		2012-01-29 14:44:41
///
///		@brief	    the thread to synch data based on the consumer name. 
///                 each consumer per thread in the system.
///                 
//////////////////////////////////////////////////////////////////////
#ifndef  PROPAGATEMSG_H
#define  PROPAGATEMSG_H

#include <sys/timeb.h>

#include "CommonData.h"
#include "IPropagateHandler.h"
#include "dbSynTestCreateMsgManager.h"

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"
#include "SynSimuCliientInfo.h"


using TA_Base_Core::ReEntrantThreadLockable;

NS_BEGIN(TA_AQ_Lib)

class IDatabase;
class CListener;
class CConsumerImpl;
class CMessage;

NS_END(TA_AQ_Lib)

NS_BEGIN(TA_DB_Sync)

class IObserverRcvMsg;
class CMsgItemHelper;
class CProcessRcvMsg;
class CTimeMonitor;
class CDeqMsgJob;

class CPropagateMsg : public  TA_Base_Core::Thread
					, public IPropagateHandler
{
private:
	enum eumPropagateState
	{
		STATE_CODE_INIT_ENV                             = 0,
		STATE_CODE_CONNTOSRV,
		STATE_CODE_SENDINFO,
		STATE_CODE_INITDEQ,
		STATE_CODE_DEQMSG,
		STATE_CODE_SENDMSGREQ,
		STATE_CODE_WAITFORENQACK,
		STATE_CODE_WAITFORCONACK,
		STATE_CODE_RCVENQACK,
		STATE_CODE_RCVCONACK,
		STATE_CODE_FINISH_TRANSACTION,
		STATE_CODE_FINISH_ALLWork,
	};
	
	enum enumControlWorkState
	{
		ControlWorkState_DoingWork,	
		ControlWorkState_PauseWork,

	};


public:
	CPropagateMsg(IObserverRcvMsg* pObserver);
	~CPropagateMsg(void);

public:
	virtual int   setPropagateTHD(PRO_THD* pProThd);
	virtual int   processRcvFrame(PCONACK pConAck);
	virtual int   processRcvFrame(PENQACK pEnqAck);
	virtual int   processTcpEvent( int nStateCode );

public:
	virtual void run();
	virtual void terminate();

private:
	void  _ThreadJob();

	void  _ProcessInitEnv();
	void  _ProcessCon2Srv();
	void  _ProcessSendInfo();
	void  _ProcessInitDeq();
	void  _ProcessDeqMsg();
	void  _ProcessSendMsgReq();
	void  _ProcessWaitForEnqAck();
	void  _ProcessWaitForConAck();
	void  _ProcessRcvEnqAck();
	void  _ProcessRcvConAck();
	void  _ProcessFinishTransaction();

	void  _ParseSrvAddr(const std::string& strAddr);
	std::string _GetLocHostName();

	template<class MsgConT>
	void  _FreeMsgCon(MsgConT& rMsgCon);

	void  _ClearUpEnv();
	void  _InitDeqEnv();
	void  _GetWorkAgent();
	void  _GetSrvID();
	void  _PrintMsgLog(int& nPrintInterval, int nState);
private:
	bool                     m_bTerminate;
	bool                     m_bPropagated;
	//IDatabase*               m_pDeqDatabase;
	//CListener*               m_pListener;
	//CConsumerImpl*			 m_pMsgConsumer;
	//lstDBMsgConT             m_lstSendMsg;
	ENQREQEX*				 m_pEnqReqTotal;

	std::string              m_strServerID;
	size_t                   m_uPropagateMsgNum;
	int                      m_nTcpState;
	int                      m_nDBType;
	ulong64                  m_ulMsgSent;
	IObserverRcvMsg*         m_pMsgObserver;
	eumPropagateState        m_nPropagateFlag;
	PRO_THD*                 m_pProTHD;
	CAgent                   m_Receiver;
	CONACK*                  m_pConAck;
	ENQACK*                  m_pEnqAck;
	ENQREQEX                 m_stEnqReq;
	CMsgItemHelper*          m_pMsgItemHelper;
	std::string              m_strReqPkgID;
	std::string              m_strAckPkgID;
	std::string              m_strSrvAddr;
	std::string              m_strSrvPort;
	std::string              m_sendName;

	CProcessRcvMsg*          m_pEnqReqProcessor;
	CTimeMonitor*            m_pTimer;
    ReEntrantThreadLockable  m_tLockAcks;                        // the thread lock for acknowledge reply (connection & enq request)
	int                      m_nLogCount;
	int                      m_nDBSrvID;
    int                      m_nMsgSrvID;                        // the message server ID of the first item
	uint32                   m_nReconTimes;
	uint32					 m_nEachDeqWorkUse;	 	
	size_t					 m_nDeqWorkIndex;
	size_t					 m_nSendWorkIndex;

	uint32					 m_nStepDeqDataTime;
	uint32					 m_nStepSendDataTime;	
	uint32					 m_nStepWaitACKTime;	
	uint32					 m_nStepDeqCommitTime;	
	
	
private:
	ulong64					m_ullastMsgSeqID;
	
	ReEntrantThreadLockable			m_LockControlWorkState;
	enumControlWorkState			m_nControlWorkState;
	
	ReEntrantThreadLockable			m_LockSynSimuClientInfo;
	SimuClientInfo					m_SynSimuClientInfo;

public:
	int                     m_nClientIndex;
	EThreadWorkState        m_WorkState;
	CDbSynTestCreateMsgManager* m_pDataManager;
public:
	bool                    isFinishWork();	
	int						continueWork();
	int						pauseWork();
	int						getSimuClientInfo(SimuClientInfo& SynSimuClientInfo);
	int						getThreadStatus(std::string& strStatus);

private:
	int						_PrintDataInfoToLog();
	int						_UpdateSimuClientInfo();

};

NS_END(TA_DB_Sync)

#endif  // PROPAGATEMSG_H
