
/*
; SynSimulatorClient.ini
; ServerIp     -- dbSyn  Server IP
; ServerPort   -- dbSyn  Server ServerPort
; QueueName    -- the name of the queue which used for synchronizing data
; ClientCount  -- the SimulatorClient count
; BeginRecipients  -- the first SimulatorClient name
; EndRecipients  -- the last SimulatorClient name

; StartSeqNum	-- the first AQ data's msg_sid (you can get it by use sql:select * from AQ_ENQ_DUPLICATE_CHECK in Oracle)
; EndSeqNum	-- the last AQ data's msg_sid 

; AuditDataOracle -- the AQ data
; AuditDataMysql -- the AQ data

; PackageSize  -- the maximum package size while traveraling through network (KB)
; IntervalTime -- sleep time between each package  (millsecond)
; PerBatchTimeOut -- the time out for each package, if over time log error (second) 
; TestDuration --  the test duration for sending messages (minute) 

; LogLevel     -- the log level for debugging
; LogFileSize  -- the size of log file
; LogFileCount -- the count of log files
; LogFileName  -- the path & name of log file


;#MESSAGETYPE# ,  
; NumberofInsert --if NumberofInsert is zero, disable the message type. othewise the simulator sends message based on the value in the time interval of IntervalTimeofInsert 
;IntervalTimeofInsert --
;NumberofGroup--
;IntervalTimeofGroup--
;NumberofPublic--
;IntervalTimeofPublic--


*/


#if !defined(__C_DBSYN_TEST_CLIENTS_MANAGER_HH__)
#define __C_DBSYN_TEST_CLIENTS_MANAGER_HH__

#include <sys/timeb.h>
#include <iostream>
#include <fstream>
#include <list>

#include "CommonDef.h"
#include "CommonData.h"

#include "core/utilities/src/DebugUtil.h"

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "IDatabase.h"
#include "IPropagateHandler.h"
#include "IObserverRcvMsg.h"
#include "TimeMonitor.h"

#include "PropagateMsg.h"
#include "..\IDBSyncSimGUIAccess.h"

NS_BEGIN(TA_DB_Sync)
using namespace TA_Base_Core;



class CDbSynTestClientManager :  
	public Thread, 
	public IDBSyncSimGUIAccess
{
private:
	enum EManagerWorkState
	{
		EManager_State_Begin_StartWork,
		
		EManager_State_InitCfgParam,
		EManager_State_InitNewClients,
		EManager_State_MonitorNewClients,
		EManager_State_CheckClientsWorkTime,
		EManager_State_UnInitNewClients,		 

		EManager_State_End_FinishWork,
	};
private:
	typedef  std::map<int, std::string>						mapIntStrConT;
	typedef  std::map<int, std::string>::iterator			mapIntStrConIterT;
	typedef  std::map<int, std::string>::value_type			mapIntStrConVT;
	

	typedef  std::map<uint32, PRO_THD*>						mapQThreadT;
	typedef  std::map<uint32, PRO_THD*>::iterator			mapQThreadIterT;
	typedef  std::map<uint32, PRO_THD*>::value_type         mapQThreadValueT;	

	typedef  std::map<uint32, CPropagateMsg*>               mapQThreadPoolT;
	typedef  std::map<uint32, CPropagateMsg*>::iterator     mapQThreadPoolIterT;
	typedef  std::map<uint32, CPropagateMsg*>::value_type   mapQThreadPoolValueT;


	typedef  std::map<std::string, SimuClientInfo*>						mapSimuClientInfoT;
	typedef  std::map<std::string, SimuClientInfo*>::iterator			mapSimuClientInfoIterT;
	typedef  std::map<std::string, SimuClientInfo*>::value_type         mapSimuClientInfoValueT;	




public:
	CDbSynTestClientManager();
	~CDbSynTestClientManager();

	
public:
	virtual void			run(); 
	virtual void			terminate();

//////////////////////////////////////////////////////////////////////////
public:
	bool                    isFinishWork();
	
private:
	int						_ProcessUserTerminate();  
//////////////////////////////////////////////////////////////////////////
	
public:	
	int						setConFigFile(const std::string& strCfgFile);
	
	

//////////////////////////////////////////////////////////////////////////
private:
	std::string&			_MysysLtrim(std::string& s);
	std::string&			_MysysRtrim(std::string& s);
	std::string&			_MysysTrim(std::string& s);
	int						_MysysStrUpper(std::string& strSrc);
	int						_MysysGetVisableStr(char* Str);
	int						_MysysGetVisableStdStr(std::string& strSrc);
	int						_ReadAllLineFromFile(const std::string& strFileName, VrtLinesConT& vrtAllLines);
	int						_GetNameValueFormCfg(std::string& strName, std::string& strValue);
	int						_ProcessVrtLines(VrtLinesConT& vrtAllLines);
	int						_ConfigFileOpen(const std::string& strConfigFileName);
	int						_ParseLogLevel(const std::string& strValue, DebugUtil::EDebugLevel& nLogLevel);

	

private:
	int						_ProceeParam();
	int						_LogPrintParam();
	int						_CheckCfgParam();
	int						_InitClientAgtName(const std::string& strINAgtFormat, int nINClientIndex, std::string& strOUTAgtName);
	int						_CreateClientAgtNameList();
	int						_InitOracleDbTableParam();
	int						_UpdateTable(TA_AQ_Lib::IDatabase* pINDataBase, std::string& strINLocationKey, std::string& strINQueueName, int& nINSrvID,ulong64 nValueMsgSID);

private:
	int						_MonitorClientThreads();
	

public:
	virtual int						continueWork();
	virtual int						pauseWork();
	virtual int						printStatus();	//write to log
	virtual std::map<std::string, SimuClientInfo*>&   getClientInfos();




private:
	int						_ThreadJob();
	int						_PocessEManagerState();


private: 
	int						_PocessEManagerStateBeginStartWork(); 
	int						_PocessEManagerStateInitCfgParam();
	int						_PocessEManagerStateInitNewClients();
	int						_PocessEManagerStateMonitorNewClients();
	int						_PocessEManagerStateCheckClientsWorkTime();
	int						_PocessEManagerStateUnInitNewClients();
	int						_PocessEManagerStateEndFinishWork();
	int						_TurnToOneLineInAllLine( VrtLinesConT& vrtAllLines );
	int						_RemoveBlackLineInAllLine( VrtLinesConT& vrtAllLines );
	int						_getAllClientAgtNameFromRecipients(
		const std::string& strINRecipients, 
		mapIntStrConT& mapOUTClientAgtName);


private:	
	int CDbSynTestClientManager::_InitAllClientInfo(mapQThreadT& vrtSynClientsInfo);
	void CDbSynTestClientManager::_FreeClientThreadPool(mapQThreadPoolT& rMapPool);
	void CDbSynTestClientManager::_FreeClinetInfoPool(mapQThreadT& rMapThread);
	void CDbSynTestClientManager::_ProcessInitialEnv();
	void CDbSynTestClientManager::_StartUpNewJobs(mapQThreadT& mapClientInfo, mapQThreadPoolT& mapClientThreadPool);
	int CDbSynTestClientManager::_ProceeReplaceConfigString();
	int CDbSynTestClientManager::_ProceeReplaceConfigMsgSeq(std::string& strINOUTSring);
	void CDbSynTestClientManager::_FreeMapSimuClientInfo(mapSimuClientInfoT& rMapSimuClientInfo);
	int CDbSynTestClientManager::_GetALLClientInfo();
	int CDbSynTestClientManager::_GetALLClientInfoEx();
	int CDbSynTestClientManager::_UpdateClientInfoToMap(CPropagateMsg* pClient);

	

private: 	
	//DebugUtil::EDebugLevel  m_DU_LogLevel;
	NonReEntrantThreadLockable	m_lockMapAllClientInfo; 
	mapSimuClientInfoT	  m_mapAllClientInfo; // std::string value agtname	

private:	

private:
	struct timeb			m_fSimulatorWorkBegin;	
	struct timeb			m_fSimulatorWorkEnd;
	int					m_nDiffBeginEnd;//<=m_n_TestDuration	
	std::string			m_strSimulatorWorkBegin;
	std::string			m_strSimulatorWorkEnd;

private:
	mapIntStrConT			m_mapClientIndexAgtName;
	//mapQThreadT				m_vrtSynClients; 

	IObserverRcvMsg*		m_pObserver;
	int						m_nLogIndex;
	CTimeUtility*           m_pTimeMonitor;
private:
	EThreadWorkState		m_nWorkState;
	EManagerWorkState		m_nManagerWorkState;
	bool					m_toTerminate;

private:
	mapQThreadT				m_vrtSynClientsInfo;

	NonReEntrantThreadLockable	m_lockVrtSynClientsThread;
	mapQThreadPoolT         m_vrtSynClientsThread;
	
	
};//class __C_DBSYN_TEST_CLIENTS_MANAGER_HH__


NS_END(TA_DB_Sync) 

#endif//__C_DBSYN_TEST_CLIENTS_MANAGER_HH__




















