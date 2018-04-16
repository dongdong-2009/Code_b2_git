#if !defined(__C_DBSYN_TEST_CREATE_MSG_MANAGER_HH__)
#define __C_DBSYN_TEST_CREATE_MSG_MANAGER_HH__

#include <sys/timeb.h>
#include <iostream>
#include <fstream>
#include <list>

#include "CommonDef.h"
#include "CommonData.h"

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include "MsgHelper.h"

#include "dbSynTestCreateEnqMsg.h"

NS_BEGIN(TA_DB_Sync)
using namespace TA_Base_Core;

class CDbSynTestCreateMsgManager 
{

private:
	enum CreateMsgManagerWorkState
	{
		CreateMsgManagerWorkState_StartWork,
		
		CreateMsgManagerWorkState_InitCTOSComInitMsg_Param,
		CreateMsgManagerWorkState_InitCTOSEnqMsg_Param,

		CreateMsgManagerWorkState_StartCreateMsg,
		CreateMsgManagerWorkState_CreateingMsg,
		CreateMsgManagerWorkState_StopCreateMsg,


		CreateMsgManagerWorkState_FinishWork,
	};
	
	
public:
	CDbSynTestCreateMsgManager();
	~CDbSynTestCreateMsgManager();

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

public:
	bool            isFinishWork();
public:
	int				getMsgCTOSConReq(MSGPKG** ppOUTMsgPkg);
	int				getMsgCTOSEnqReq(ENQREQEX** ppOOUTEnqReq, ulong64& ulSeqIDFrom, ulong64& ulSeqIDTo);

	unsigned long   getMessageCount();
public:	
	int				setAllParam();
	int				startCreateMsg();
	int				stopCreateMsg();
	int				getLockSeqIDCurrent(ulong64& ulOUTSeqIDCurrent);
	int				addNewDataToTotalList(ulong64& ulINSeqIDCurrent, ENQREQEX** ppINEnqReq);
	int				freeLockSeqIDCurrent(ulong64& ulINSeqIDCurrent);
	int				_GetDataFromTotalList( int nINPackageSize, ENQREQEX** ppOUTPackage, ulong64& ulSeqIDForm, ulong64& ulSeqIDTo);
   
private:	
	void			_FreeEnqReqEx(ENQREQEX** ppEnqReq);
	void			_FreeMsgVec(lstDBMsgConT& rVecMsg);
	int				_GetOnePackageFromTotalLst( ENQREQEX* pINTotalMsg, int nINPackageSize, ENQREQEX** ppOUTPackage, ulong64& ulSeqIDFrom, ulong64& ulSeqIDTo);
	int				_AddNewEnqDeqMsgToTotalLst(ENQREQEX** ppINEnqReq, ENQREQEX** ppstEnqReqTotal);


public:
	std::string		m_strAgtName;
	std::string     m_strDbConStr;
	std::string     m_strQName;
	std::string     m_strQNameForServerID;

	int				m_nClientIndex;
	std::string		m_strSrvIP;	//192.168.253.130
	std::string		m_strSrvPort;  //2005
	std::string		m_strServerID;   //192.168.253.130_2005_TRA_DT01
	ulong64			m_ulSeqIDFrom;// TRA_DT01  1-100000000;	TRA_DT01 100000001-200000000
	ulong64			m_ulSeqIDTo;
	int             m_nSRVID;

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


private:
	//CDbSynTestCreateComInitMsg*   m_pCreateInitMsg;
	CDbSynTestCreateEnqMsg*       m_pCreateInsertEnqMsgThread;
	CDbSynTestCreateEnqMsg*       m_pCreateGroupEnqMsgThread;
	CDbSynTestCreateEnqMsg*       m_pCreatePublicEnqMsgThread;
	CDbSynTestCreateEnqMsg*       m_pCreateMMSEnqMsgThread;

	CreateMsgManagerWorkState     m_nManagerWorkState;
private:
	NonReEntrantThreadLockable	  m_lockULSeqIDCurrent; 
	ulong64						  m_ulSeqIDCurrent;
	bool						  m_bSeqIDCurrentHaveBeenGet;

	NonReEntrantThreadLockable	  m_lockENQREQEXTotal;
	ENQREQEX*					  m_pstEnqReqTotal;
private:
	CMsgItemHelper*				  m_pMsgItemHelper;

public:
	int CDbSynTestCreateMsgManager::continueWork();
	int CDbSynTestCreateMsgManager::pauseWork();

};//class CDbSynTestClient


NS_END(TA_DB_Sync) 

#endif//__C_DBSYN_TEST_CREATE_MSG_HH__




















