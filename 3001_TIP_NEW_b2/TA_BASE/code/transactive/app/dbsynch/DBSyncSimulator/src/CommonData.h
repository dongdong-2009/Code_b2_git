//////////////////////////////////////////////////////////////////////
///		@file		CommonData.h
///		@author		zhilin,ouyang
///		@date		2012-01-05 11:44:41
///
///		@brief	    to define some common use data or structure which 
///                 used in the DB synchronization engine.
//////////////////////////////////////////////////////////////////////
#ifndef  COMMONDATA_H
#define  COMMONDATA_H
#include "CommonDef.h"
#include "Agent.h"

#include <iostream>
#include <memory.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include <time.h>
#include <set>

NS_BEGIN(TA_AQ_Lib)
class CMessage;

typedef std::list<CAgent*>                          lstAgentContT;
typedef std::list<CAgent*>::iterator                lstAgentContIterT;
typedef std::list<CMessage*>						lstDBMsgConT;
typedef std::list<CMessage*>::iterator				lstDBMsgConIterT;
typedef std::list<lstDBMsgConT*>					lstDBMsgsConT;   
typedef std::list<lstDBMsgConT*>::iterator		    lstDBMsgsConIterT;                        

NS_END(TA_AQ_Lib)

NS_BEGIN(TA_DB_Sync)

class IMsgItem;

// the information for each propagation thread
// to propagate message
typedef struct st_pro_thd_tag
{
	int        m_nClientIndex;
	// the name of the queue being propagated
	char       m_szQName[defMaxNameLen];
	char       m_szQNameForServerID[defMaxNameLen];

	// the ID of the subscriber for this propagation channel
	uint32     m_nSubID;

	// the type of the payload for the specified queue.
	uint32     m_nPayloadType;    

	// sender of the propagation
	CAgent     m_Sender;

	// list of receiver for the propagation channel
	// and the format of the receiver is: ip1:port1;ip2:port2...
	//CAgent     m_Recver;
	lstAgentContT  m_lstRecver;

	st_pro_thd_tag()
	: m_nSubID(0)
	, m_nPayloadType(MSGITEM_PR_NORMAL)
	{
		memset(m_szQName, 0, defMaxNameLen);
	}

}PRO_THD, *PPRO_THD;

//////////////////////////////////////////////////////////////////////////
typedef struct st_aqwork_info_tag 
{
	std::string m_strQName;
	std::string m_strServerAgtName;
	std::string m_strClientAgtName;
	int  m_nINSRVID;
	std::string m_strClientID;
}AQWorkInfo, *PAQWorkInfo;

///////////////////////////////////////////////////
// the global configuration parameters
typedef struct st_config_parameter
{
	// whether need compress or not for the message to be sent
	bool  m_MsgCompress; 
	// the max package size for TCP/IP transfer
	uint32  m_PackageSize;

	// the db connection string of the source/sender database
	// for the source db is MySQL, and the ip address can be more 
	// than one. however they share the same schema. the purpose is
	// that the MySQL cluster at one location can be more than one
	// data node. usually the best solution is that there is a virtual
	// ip for all the cluster node at one location. the format like:
	// 192.168.0.1;192.168.0.2
	// for example: MySQL:TRA_OCC:IT271350_5:IT271350_5:192.168.0.1;192.168.0.2
	char  m_SrcDbConStr[defConfig_OneLineMaxLen];
	char  m_SkipErrors[defMaxNameLen];

	// the type of the working DB, Oracle or MySQL.
	// this configuration still need to confirm whether use this one to
	// identify the working DB is at center or station
	int   m_nDBType;

	// the agent name for the local, currently it's use the database
	// name for the agent name at each station
	CAgent  m_LocalAgent;

	// the center database name used for get the address of the center db
	// and also the agent/subscriber name for the center database queue
	char  m_CenterDBName[defMaxNameLen];

	// the port in which listen to
	char  m_szPort[defServerPortLen];

	// the running type of the application. refer to the enRunTypeCode (Server: 0; Client: 1)
	int  m_nRunType;
		 
	int  m_nEnqTimeOut;//(millsecs) defaule value	TIME_MIN_FIVE
	int  m_nDeqTimeOut;//(millsecs) defaule value	TIME_MIN_FIVE
	int  m_nDeqGetACKTimeOut;//(millsecs) defaule value	TIME_MIN_FIFTEEN
	int  m_nHandShareTimeOut;//(millsecs) defaule value	TIME_SECOND_TEN
	int  m_nReleaseActorTimeOut;//(millsecs) defaule value	TIME_MIN_FIVE

	// variables for output log
	int     m_LogLevel;
	uint32  m_LogFileSize;
	uint32  m_LogFileCount;
	char    m_LogFileName[defMaxFileLen];

}g_config_parameter, *pg_config_parameter;




// the structure of the message data
typedef struct st_msgitem_header
{
	unsigned char		ucMsgMT; 	// the type of the message
	unsigned char		ucszMsgQN[MSGITEM_LEN_QN];	 	// the queue name which the message belong to
	unsigned char		ucszMsgOMI[MSGITEM_LEN_OMI];   	// message id
	unsigned char		ucszMsgOServerID[MSGITEM_LEN_OServerID];  //Original DB Server ID	
	unsigned char		ucszMsgSID[MSGITEM_LEN_SID];
	// priority of the message.
	// 0 - immediately; 1 - fix time  
	unsigned char		ucPriority;	
	 	
	unsigned char		ucPYTP;	 	

	unsigned char		ucszMsgDL[MSGITEM_LEN_MDL];   // length of the message data
	//MSGHEAD_TOTAL_LEN 74=1+32+36+1+4
	unsigned char		*pFrameStartPtr;//
	unsigned char		*pMsgPos;

	unsigned char		ucszMsgQNValue[MSGITEM_QN_VALUE_BUFFER_LEN];
	ulong64     uInt64SIDValue;	 	
	int			ucszMsgDLValue;	 // length of the message data
	unsigned char		ucszMsgOMIValue[MSGITEM_OMI_VALUE_BUFFER_LEN];   	// message id
	int			ucszMsgOServerIDValue;

	st_msgitem_header() 
	{
		ucMsgMT = ComProtocol_CTOS_CON_REQ;
		memset(ucszMsgQN, 0, MSGITEM_LEN_QN);
		memset(ucszMsgOMI, 0, MSGITEM_LEN_OMI);
		memset(ucszMsgOServerID, 0, MSGITEM_LEN_OServerID);
		ucszMsgOServerIDValue = 0;
		uInt64SIDValue = 0;				 
		memset(ucszMsgSID, 0, MSGITEM_LEN_SID);

		ucPriority = MSGITEM_PR_NORMAL;
		ucPYTP   = MSGITEM_PYTP_OBJ;
		memset(ucszMsgDL, 0, MSGITEM_LEN_MDL);

		ucszMsgDLValue = 0;	
		pFrameStartPtr = NULL;
		pMsgPos = NULL;
		memset(ucszMsgQNValue, 0, MSGITEM_QN_VALUE_BUFFER_LEN);
		memset(ucszMsgOMIValue, 0, MSGITEM_OMI_VALUE_BUFFER_LEN);
	}

	~st_msgitem_header() 
	{ 
		if (NULL != pFrameStartPtr)
		{
			delete []pFrameStartPtr;
			pFrameStartPtr = NULL;
		}
		pMsgPos = NULL;		
	}

}MSGITEM_HEADER, *PMSGITEM_HEADER;


// the structure of the message data
typedef struct st_msg_package_header
{
	unsigned char		ucMsgFrameSTX;
	unsigned char		ucszMsgFramePL[MSGPACKAGE_LEN_PL];
	unsigned char		ucszMsgFramePUUID[MSGPACKAGE_LEN_PUUID];
	unsigned char		ucMsgFramePT;
	unsigned char		ucMsgFrameDC;
	unsigned char		ucszMsgFrameDL[MSGPACKAGE_LEN_UZDL];
	unsigned char		ucMsgFrameETX;

	unsigned char		*pMsgFrameStartPtr;// STX PL PT DC DL  data     add one ETX


	int			ucszMsgFrameDLValue;
	int			nMsgFrameDataLen;
	int			ucszMsgFramePLValue;
	unsigned char		ucszMsgFramePUUIDValue[MSGPACKAGE_PUUID_BUFFER_LEN];

	st_msg_package_header() 
	{
		ucMsgFrameSTX = MSGPACKAGE_STX_VALUE;
		memset(ucszMsgFramePL, 0, MSGPACKAGE_LEN_PL);
		memset(ucszMsgFramePUUID, 0, MSGPACKAGE_LEN_PUUID);

		ucMsgFramePT = 0;
		ucMsgFrameDC = 0;
		memset(ucszMsgFrameDL, 0, MSGPACKAGE_LEN_UZDL);
		pMsgFrameStartPtr = NULL;
		ucMsgFrameETX = MSGPACKAGE_ETX_VALUE;
		ucszMsgFrameDLValue = 0;
		nMsgFrameDataLen = 0;
		ucszMsgFramePLValue = 0;
	}

	~st_msg_package_header() 
	{ 
		if (NULL != pMsgFrameStartPtr)
		{
			delete []pMsgFrameStartPtr;
			pMsgFrameStartPtr = NULL;
		}		
	}

}MSG_PACKAGE_HEADER, *PMSG_PACKAGE_HEADER;


// flags use for identifying system state code
typedef struct st_sys_flags
{
	st_sys_flags() 
	{
		memset(codeBits, 0, sizeof(codeBits));
	}
	inline void setCodeFlag(unsigned uCodePos)
	{
		codeBits[uCodePos >> defItemIndexMask] |= (1 << (uCodePos & defCharBitMask));
	}
	inline void unSetCodeFlag(unsigned uCodePos)
	{
		codeBits[uCodePos >> defItemIndexMask] &= ~(1 << (uCodePos & defCharBitMask));
	}
	inline bool isCodeBitSet(unsigned uCodePos)
	{
		return 	((codeBits[uCodePos >> defItemIndexMask] & (1 << (uCodePos & defCharBitMask))) != 0);
	}	

private:
	unsigned char codeBits[defFlagNum];  // code bit flags	
}g_sys_code_flags, *pg_sys_code_flags;


//============================================================================
typedef std::list<IMsgItem*>						lstMsgItemConT;
typedef std::list<IMsgItem*>::iterator				lstMsgItemConIterT;

typedef std::list<lstMsgItemConT*>					lstMsgItemsConT;   //MsgItem's order is same to recv Buffer
typedef std::list<lstMsgItemConT*>::iterator		lstMsgItemsConIterT;

typedef struct st_msg_pkg_tag 
{
	std::string   m_strPkgUUID32;
	unsigned char		  m_nPkgType;
	bool          m_bZip;   //DC
	lstMsgItemsConT  m_lstAllMsgItems;
	 
}MSGPKG, *PMSGPKG;

typedef std::list<MSGPKG*>							lstMSGPKGConT;
typedef std::list<MSGPKG*>::iterator				lstMSGPKGConIterT;

//ComProtocol_CTOS_CON_REQ
typedef struct st_ctos_con_req_tag 
{
	std::string m_strAgtName;
	std::string m_strQName;
	std::string m_strHostName;
	std::string m_strIP;
	std::string m_strPkgID;
	int         m_nOServerID;
}CONREQ, *PCONREQ;

//ComProtocol_STOC_CON_ACK
typedef struct st_stoc_con_ack_tag 
{
	std::string m_strQName;
	int			m_nConReqAck;
	std::string m_strPkgID;

}CONACK, *PCONACK;

//ComProtocol_STOC_ENQ_ACK
//ComProtocol_CTOS_ENQ_ACK
typedef struct st_enq_ack_tag 
{
	std::string m_strQName;
	std::string m_strPkgID;
	int  m_nEnqReqACK;

}ENQACK, *PENQACK;

typedef std::list<ENQACK*>							lstEnqACKConT;
typedef std::list<ENQACK*>::iterator				lstEnqACKConIterT;

//////////////////////////////////////////////////////////////////////////
typedef struct  st_enq_req_ex_tag
{
	lstDBMsgConT m_lstDBMsg;
	std::string m_strPkgID;
}ENQREQEX, *PENQREQEX;
	
typedef std::list<ENQREQEX*>						lstEnqReqExConT;
typedef std::list<ENQREQEX*>::iterator				lstEnqReqExConIterT;


//////////////////////////////////////////////////////////////////////////

typedef struct st_WorkTimeInfo_tag 
{
public:
	std::string m_strClientID;
	int m_nHandSharkNotGet;//(millsecs) not get HandShark msg time
	int m_nEnqNotWork;//(millsecs) EnqNotWork time
	int m_nDeqNotWork;//(millsecs) DeqNotWork time
	int m_nDeqACKNotGet;//(millsecs) DeqACKNotGet time
	int m_nReleaseActorNotWork;//(millsecs) ReleaseActor time
public:
	st_WorkTimeInfo_tag()
	{
		m_strClientID.clear();
		m_nHandSharkNotGet = 0;
		m_nEnqNotWork = 0;
		m_nDeqNotWork = 0;
		m_nDeqACKNotGet = 0;
		m_nReleaseActorNotWork = 0;
	}
}WorkTimeInfo, *PWorkTimeInfo;





//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////





typedef struct st_Cfg_SynSimulatorClient_info 
{

public:
	std::string				m_strConfigFileName;

	std::string				m_strCfg_SrcDbConStr;
	std::string				m_strCfg_QueueName;
	std::string				m_strCfg_ServerIP;
	std::string				m_strCfg_ServerPort;
	std::string				m_strCfg_ClientCount;//5
	std::string				m_strCfg_Recipients; //TRA_OCC,TRA_DT01
	std::string				m_strCfg_StartSeqNum;
	std::string				m_strCfg_EndSeqNum;


	std::string				m_strCfg_NumberofInsert;
	std::string				m_strCfg_NumberofGroup;
	std::string				m_strCfg_NumberofPublic;
	std::string				m_strCfg_NumberofMMS;

	std::string				m_strCfg_IntervalTimeofInsert;
	std::string				m_strCfg_IntervalTimeofGroup;
	std::string				m_strCfg_IntervalTimeofPublic;
	std::string				m_strCfg_IntervalTimeofMMS;


	std::string				m_strCfg_AuditDataOracle;
	std::string				m_strCfg_AuditDataMysql;

	std::string				m_strCfg_PackageSize;
	std::string				m_strCfg_IntervalTime;
	std::string				m_strCfg_PerBatchTimeOut;
	std::string				m_strCfg_TestDuration;
	std::string				m_strCfg_loglevel;
	std::string				m_strCfg_logfilesize;
	std::string				m_strCfg_logfilecount;
	std::string				m_strCfg_logfilename;

// public:
// 	st_Cfg_SynSimulatorClient_info()
// 	{
// 
// 	}
// 	~st_Cfg_SynSimulatorClient_info()
// 	{
// 
// 	}	

}CfgSimInfo, *PCfgSimInfo;




typedef struct st_SynProgram_SynSimulatorClient_info 
{ 
public:
	int						m_n_ClientCount;
	ulong64					m_n_StartSeqNum;
	ulong64					m_n_EndSeqNum;
	int						m_n_IntervalTimeofInsert;
	int						m_n_IntervalTimeofGroup; 
	int						m_n_IntervalTimeofPublic;
	int						m_n_IntervalTimeofMMS;

	int						m_n_NumberofInsert;
	int						m_n_NumberofGroup;
	int						m_n_NumberofPublic;
	int						m_n_NumberofMMS;

	std::string				m_szFormat_AuditDataOracle;
	std::string				m_szFormat_AuditDataMysql;

	int                     m_n_PackageSize;
	int						m_n_IntervalTime;	 //millsecond
	int						m_n_PerBatchTimeOut; //millsecond
	unsigned long			m_n_TestDuration;	  //millsecond

	int						m_n_logfilesize;
	int						m_n_logfilecount;
	int						m_DU_LogLevel;

	int						m_n_Default_SRVID;//AQ_ENQ_DUPLICATE_CHECK.SRV_ID	1
	std::string				m_szFormat_Recipient_End;//"TRA_DT_"

// public:
// 	st_SynProgram_SynSimulatorClient_info()
// 	{
// 
// 	}
// 	~st_SynProgram_SynSimulatorClient_info()
// 	{
// 
// 	}
}SynProgramSimInfo, *PSynProgramSimInfo; //CfgSimInfo







//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
// declare the global system variables
extern g_config_parameter g_system_variables;
extern g_sys_code_flags   g_sys_status_mask;


extern CfgSimInfo				g_CfgSimInfo;
extern SynProgramSimInfo		g_SynProgramSimInfo;


NS_END(TA_DB_Sync)


















#endif  // COMMONDATA_H








