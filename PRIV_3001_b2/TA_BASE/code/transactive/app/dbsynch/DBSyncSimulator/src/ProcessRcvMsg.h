//////////////////////////////////////////////////////////////////////
///		@file		ProcessRcvMsg.h
///		@author		zhilin,ouyang
///		@date		2012-02-29 14:44:41
///
///		@brief	    the thread to process the received messages from  
///                 remote database/location.
///                 
//////////////////////////////////////////////////////////////////////
#ifndef  PROCESSRCVMSG_H
#define  PROCESSRCVMSG_H

#include <sys/timeb.h>
#include "CommonDef.h"
#include "IPropagateHandler.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"
#include <list>
#include <map>
#include <string>

NS_BEGIN(TA_AQ_Lib)

class IDatabase;
class CProducer;

NS_END(TA_AQ_Lib)

NS_BEGIN(TA_DB_Sync)

class CMsgItemHelper;
class CTimeMonitor;

using TA_Base_Core::ReEntrantThreadLockable;

class CProcessRcvMsg : public TA_Base_Core::Thread
					 , public IPropagateHandler
{
private:
	enum eumRcvMsgProcessState
	{
		STATE_CODE_INITIAL_ENV                       = 0,
		STATE_CODE_INITIAL_ENQ,
		STATE_CODE_WAITFOR_ENQREQ,
		STATE_CODE_RCV_MSG,
		STATE_CODE_GETSID_FROMDB,
		STATE_CODE_ENQ_MSG,
		STATE_CODE_UPDSID_2DB,
		STATE_CODE_SEND_ACK,
		STATE_CODE_FINISH_TRANSACTION
	};
	
	enum enumControlWorkState
	{
		ControlWorkState_DoingWork,	
		ControlWorkState_PauseWork,

	};

private:
	typedef  std::list<ENQREQEX*>					lstEnqReqT;
	typedef  std::list<ENQREQEX*>::iterator			lstEnqReqIterT;
	typedef  std::map<int, ulong64>					mapDupChkSIDT;
	typedef  std::map<int, ulong64>::iterator		mapDupChkSIDIterT;
	typedef  std::map<int, ulong64>::value_type		mapDupChkSIDValueT;

public:
	CProcessRcvMsg();
	~CProcessRcvMsg(void);

	virtual int   processTcpEvent( int nStateCode );
	virtual int   processRcvFrame(ENQREQEX* pEnqReq);

public:
	virtual void run();
	virtual void terminate();

public:
	void  setServerID(const std::string& strServerID);
	void  setRcvPara(const std::string& strQName, const std::string& strRcver);

private:
	void   _ThreadJob();
	void   _ProcessInitEnv();
	void   _ProcessInitEnq();
	void   _ProcessWaitForEnqReq();
	void   _ProcessRcvMsg();
	void   _ProcessGetSIDFromDB();
	void   _ProcessEnqMsg();
	void   _ProcessUpdSID2DB();
	void   _ProcessSendAck();
	void   _ProcessFinishTransaction();

private:

	void   _FreeMsgLst(lstEnqReqT& rLstMsg);
	void   _FreeMsgItem(ENQREQEX*& pEnqReq);
	void   _PrintLog(int& nPrintInterval, int nState);
	void   _InitEnqEnv();
	void   _InitDupChkEnv();	
	void   _Write2ExceptionQ(CMessage* pMsg);

	int    _InitSID2DB();	
	int    _GetCurSIDFromDB();
	int    _UpdCurSID2DB();
	int    _MsgDupChk(int nSrvID, ulong64 ulSID, mapDupChkSIDIterT& rIter);

public:	
	int continueWork();
	int pauseWork();
	int getLastEnqSeqID(ulong64& ulLastEnqSeqID);
	int getLastTotoalEnq(ulong64& ulTotoalEnq);

	bool isEnqHealth();
private:
	bool                     m_bTerminate;
	int                      m_nTcpState;
	std::string              m_strPkgID;
	std::string              m_strServerID;	
	std::string              m_strQName;
	std::string              m_strRcvName;
// 	IDatabase*               m_pEnqDatabase;
// 	IDatabase*               m_pDupChkDb;
// 	CProducer*				 m_pMsgProducer;	
	uint32                   m_nMsgRcvCnt;
	uint32                   m_nEnqMsgCnt;
	ulong64                  m_ulMsgRcvCntAll;
	ulong64                  m_ulTotalEnqMsgCnt;
	lstEnqReqT               m_lstEnqReq;
	eumRcvMsgProcessState    m_nProcessState;
	CMsgItemHelper*          m_pMsgItemHelper;
	ENQACK*                  m_pEnqAck;
	ENQREQEX*                m_pCurItem;
	mapDupChkSIDT            m_mapDupChkSID;

	ReEntrantThreadLockable  m_threadLock;
	TA_Base_Core::Semaphore m_queueSemaphore;
	
	ReEntrantThreadLockable			m_LockControlWorkState;
	enumControlWorkState			m_nControlWorkState;

private:
	uint32					 m_nEachEnqWorkUse;	 	
	size_t                   m_nEnqWorkIndex;
	uint32					 m_nStepEnqDataTime;
	uint32					 m_nStepCommitDataTime;	 	
	uint32					 m_nStepSendACkTime;	
	CTimeMonitor*            m_pTimer;

	ulong64					 m_ulLastEnqSeqID;

};


NS_END(TA_DB_Sync)
#endif /// PROCESSRCVMSG_H
