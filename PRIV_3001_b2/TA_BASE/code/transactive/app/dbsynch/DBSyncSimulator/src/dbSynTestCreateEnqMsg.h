#if !defined(__C_DBSYN_TEST_CREATE_ENQ_MSG_HH__)
#define __C_DBSYN_TEST_CREATE_ENQ_MSG_HH__

#include <sys/timeb.h>
#include <iostream>
#include <fstream>
#include <list>

#include "CommonDef.h"
#include "CommonData.h"

#include "MsgHelper.h"

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"


NS_BEGIN(TA_DB_Sync)
using namespace TA_Base_Core;
class CDbSynTestCreateMsgManager;

class CDbSynTestCreateEnqMsg : public Thread
{

private:
	enum SynEnqMsgWorkState
	{
		SynEnqMsgWorkState_StartWork,
		
		SynEnqMsgWorkState_InitCfgParam,
		
		SynEnqMsgWorkState_BeginCrestEnqMsg,
		SynEnqMsgWorkState_CrestEnqMsgGetLock,
		SynEnqMsgWorkState_CrestEnqMsgWorking,
		SynEnqMsgWorkState_CrestEnqMsgFreeLock,
		SynEnqMsgWorkState_EndCrestEnqMsg,
		
		
		
		SynEnqMsgWorkState_FinishWork,
	};
	
	enum SynControlWorkState
	{
		SynControlWorkState_DoingWork,	
		SynControlWorkState_PauseWork,

	};
	

	
	
public:
	CDbSynTestCreateEnqMsg();
	~CDbSynTestCreateEnqMsg();

//////////////////////////////////////////////////////////////////////////

public:
	virtual void	run(); 
	virtual void	terminate();  

public:
	bool            isFinishWork();
private:
	int             _ProcessUserTerminate();
	int				_ThreadJob();
	int				_PocessSynMsgWorkState();
private:
	bool            m_toTerminate;
//////////////////////////////////////////////////////////////////////////


private:									
	int				_CreateMsgCTOSEnqReq( const std::string& strINMsgQFierSub, const std::string& strINAgtName, const std::string& strINQName, ulong64 ulINSeqIDBegin, int nOnceNum, ENQREQEX** ppEnqReq, ulong64& ulOUTSeqIDNestBegin);

									
public:
	int             m_nClientIndex;	

	std::string     m_strAgtName;
	std::string     m_strQName;

	
	int				m_nIntervalTimeofInsert;
	int				m_nIntervalTimeofGroup; 
	int				m_nIntervalTimeofPublic;
	int				m_nIntervalTimeofMMS;

	int				m_nNumberofInsert;
	int				m_nNumberofGroup;
	int				m_nNumberofPublic;
	int				m_nNumberofMMS;

	std::string		m_szAuditDataOracle;   
	std::string		m_szAuditDataMysql; 
	int             m_nPackageSize;
	int             m_nSRVID;
public:
	std::string     m_strMsgQFierSub;//"INSERT:","GROUP:","PUBLIC:"
	SynEnqMsgMsgType     m_nEnqMsgType;// 1-  2- 3-   "INSERT:","GROUP:","PUBLIC:"

private:
	ulong64			m_ulSeqIDCurrent;
	ENQREQEX*       m_pNewEnqReq;
	ulong64         m_ulSeqIDCurrentNextBegin;
	ulong64			m_ulSeqIDTo;
	
	std::string     m_strFindReplaceNO;	 //"#No#"
	std::string     m_strFindReplaceRecipient;   //"#Recipient#"
	std::string     m_strFindReplaceMsgTypSeq;	//"#MsgTypSeq#"
	std::string     m_strFindReplaceMsgTyp;	//"#MsgTyp#"

private:
	CMsgItemHelper*				m_pMsgItemHelper;
	SynEnqMsgWorkState			m_nSynEnqMsgWorkState;
	EThreadWorkState			m_nWorkState;
	ulong64						m_nMsgTypeSeq;
	
	NonReEntrantThreadLockable	m_lockSynControlWorkState;
	SynControlWorkState			m_nSynControlWorkState;    

public:
	CDbSynTestCreateMsgManager* m_pManagerCopy;


private:
	int _ProcessSynEnqMsgWorkState_StartWork();
	int _ProcessSynEnqMsgWorkState_InitCfgParam();
	int _ProcessSynEnqMsgWorkState_BeginCrestEnqMsg();
	int _ProcessSynEnqMsgWorkState_CrestEnqMsgGetLock();
	int _ProcessSynEnqMsgWorkState_CrestEnqMsgWorking();
	int _ProcessSynEnqMsgWorkState_CrestEnqMsgFreeLock();
	int _ProcessSynEnqMsgWorkState_EndCrestEnqMsg();
	int _ProcessSynEnqMsgWorkState_FinishWork();

public:
	int CDbSynTestCreateEnqMsg::continueWork();
	int CDbSynTestCreateEnqMsg::pauseWork();
	
	
private:
	int _FreeEnqReqEx(ENQREQEX** ppEnqReqEx);
	void _FreeMsgVec(lstDBMsgConT& rVecMsg);
	int _ReplaceMsgString( std::string& strINOUTSring, const std::string& strReplaceFind, const std::string& strReplaceDest );
	int CDbSynTestCreateEnqMsg::_PocessSynControlWorkState();
};//class CDbSynTestClient


NS_END(TA_DB_Sync) 

#endif//__C_DBSYN_TEST_CREATE_ENQ_MSG_HH__




















