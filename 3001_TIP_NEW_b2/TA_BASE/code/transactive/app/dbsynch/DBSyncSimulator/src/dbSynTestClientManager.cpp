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

//++Noel 
#include "..\GUIUpdateHandler.h"

//#include "core/utilities/src/DebugUtil.h"
using namespace TA_Base_Core;
using TA_Base_Core::DebugUtil;




NS_BEGIN(TA_DB_Sync)


struct LogLevelNameValue
{
	const char*             name;
	DebugUtil::EDebugLevel   value;
};

static LogLevelNameValue logNameValueMapping[] =
{
	{ "FATAL",		DebugUtil::DebugFatal		},
	{ "ERROR",		DebugUtil::DebugError		},
	{ "WARNING",	DebugUtil::DebugWarn		},
	{ "NORMAL",		DebugUtil::DebugNorm		},
	{ "INFO",		DebugUtil::DebugInfo		},
	{ "DEBUG",		DebugUtil::DebugDebug		},
	{ "SQL",        DebugUtil::DebugSQL         },
	{ "CORBA",      DebugUtil::DebugCORBA       },
	{ "MSG",	    DebugUtil::DebugMsg	        },
	{ "MSGPUB",	    DebugUtil::DebugMsgPub	    },
	{ "PERIODIC",   DebugUtil::DebugPeriodic    },
	{ "DATABASE",	DebugUtil::DebugDatabase	},
	{ "EXTERNAL",	DebugUtil::DebugExternal	},
	{ "TRACE",		DebugUtil::DebugTrace		},
	{ NULL,			DebugUtil::DebugInfo		}
};


CDbSynTestClientManager::CDbSynTestClientManager()
{	
	g_SynProgramSimInfo.m_DU_LogLevel = (int)DebugUtil::DebugDebug;
	
	m_strSimulatorWorkBegin = m_pTimeMonitor->getCurrentTime(&m_fSimulatorWorkBegin);
	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
		"Simulator Work Begin  [m_strSimulatorWorkBegin=%s]", 
		m_strSimulatorWorkBegin.c_str());
	m_mapClientIndexAgtName.clear();	
	g_SynProgramSimInfo.m_n_Default_SRVID = 1;   //	Test SRVID value 1
	g_CfgSimInfo.m_strCfg_SrcDbConStr.clear();
	m_toTerminate = false;
	//m_vrtSynClients.clear();	
	m_nLogIndex = 0;
	m_pTimeMonitor = new CTimeUtility();
	m_pObserver = NULL;

	CTcpClientManager::getInstance();
	
	m_nManagerWorkState = EManager_State_Begin_StartWork;
	m_nWorkState = WorkState_Begin;

}


CDbSynTestClientManager::~CDbSynTestClientManager()
{
    m_mapClientIndexAgtName.clear();
	_FreeClientThreadPool(m_vrtSynClientsThread);
	_FreeClinetInfoPool(m_vrtSynClientsInfo);

	CTcpClientManager::removeInstance();
	m_nWorkState = WorkState_End;
	DEF_DELETE(m_pObserver);
	DEF_DELETE(m_pTimeMonitor);
	_FreeMapSimuClientInfo(m_mapAllClientInfo); 
}




 //////////////////////////////////////////////////////////////////////////
 
 

// trim from start(left)
std::string& CDbSynTestClientManager::_MysysLtrim(std::string& s)
{
	s.erase(s.begin(),std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end(right)
std::string& CDbSynTestClientManager::_MysysRtrim(std::string& s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// trim from both end
std::string& CDbSynTestClientManager::_MysysTrim(std::string& s)
{	
	return _MysysLtrim(_MysysRtrim(s));
}

int CDbSynTestClientManager::_MysysStrUpper(std::string& strSrc)
{
	int nFunRes = 0;
	int nSrcSize = 0;
	char* pszSrc = NULL;
	char* pszStart = NULL;
	
	nSrcSize = (int)strSrc.size();
	nSrcSize += def_int_cfguse_Buffer;
	
	pszSrc = new char[nSrcSize];
	memset(pszSrc, 0, nSrcSize);
	sprintf(pszSrc, "%s", strSrc.c_str());
	
	pszStart = pszSrc; 	
	
	for (; *pszStart != '\0'; pszStart++)
	{	
		*pszStart = toupper(*pszStart);	
	}
	pszStart = NULL;
	
	strSrc.clear();
	strSrc = pszSrc;
    
    if (NULL != pszSrc)
    {
		delete[] pszSrc;
		pszSrc = NULL;
    }

	return nFunRes;
}

int CDbSynTestClientManager::_MysysGetVisableStr(char* Str)
{
	unsigned char Prechar = ' ';
	unsigned char *Temp = NULL;
	unsigned int Len = 0,i = 0;

	if (Str == NULL)
	{
		return -1;
	}

	if (Str[0] == 0)
	{
		return 0;
	}

	Temp = (unsigned char*)Str;

	while (Temp[i] != '\0')
	{
		//  if (Temp[i] < 0x21 || Temp[i] > 0x7e)
		if (Temp[i] < 0x21 /*|| Temp[i] > 0x7e*/)
		{
			if (Prechar == ' ')
				;
			else
			{
				Prechar = ' ';
				Str[Len++] = Prechar ;
			}
		}
		else
		{
			Prechar = Temp[i];
			Str[Len++] = Prechar;
		}
		i++;
	}

	if (Len > 0 && Str[Len - 1] == ' ')
	{
		Str[Len - 1] = '\0';
		return (int) (Len - 1);
	}
	else
	{
		Str[Len] = '\0';
		return Len;
	}
}

int CDbSynTestClientManager::_MysysGetVisableStdStr(std::string& strSrc)
{
	int nFunRes = 0;
	char* pszStrCopy = NULL;
	int   nszLen= 0;

	nszLen = (int)strSrc.length();
	if (0 == nszLen)
	{
		nFunRes = -1;
		return nFunRes;
	}

	nszLen += def_int_cfguse_Buffer;

	pszStrCopy = new char[nszLen];
	memset(pszStrCopy, 0, nszLen);
	sprintf(pszStrCopy, "%s", strSrc.c_str());
	strSrc.clear();

	_MysysGetVisableStr(pszStrCopy);

	strSrc = pszStrCopy;
	if (NULL != pszStrCopy)
	{
		delete[] (pszStrCopy);
		pszStrCopy = NULL;
	}
	return nFunRes;
}

int	CDbSynTestClientManager::_ReadAllLineFromFile(const std::string& strFileName, VrtLinesConT& vrtAllLines)
{
	int nFunRes = 0;

	std::ifstream IfstreamSrcFile;
	char*  pOneLine = NULL;
	int nFilelines = 0;
	char* chPtr = NULL;

	if (strFileName.empty())
	{
		nFunRes = -1;
		return nFunRes;
	}


	IfstreamSrcFile.open(strFileName.c_str());
	if (IfstreamSrcFile.fail())
	{
		nFunRes = -1;
		return nFunRes;
	}


	pOneLine = new char[def_int_cfguse_OneLinLen];
	memset(pOneLine, 0, def_int_cfguse_OneLinLen);

	// Read in all lines and add them to a vector
	while ( IfstreamSrcFile.getline ( pOneLine, def_int_cfguse_OneLinLen ) )
	{

		nFilelines++;
		chPtr = NULL;
		chPtr = strchr( pOneLine, def_char_cfguse_lineEnd );
		if( NULL != chPtr )
		{
			*chPtr = '\0';
		}
		_MysysGetVisableStr(pOneLine);
		std::string getOneLine = pOneLine;
		_MysysTrim(getOneLine);
		if (def_char_cfguse_lineCommont != pOneLine[0]
			&& def_str_cfguse_BlockBegin_char != pOneLine[0])
		{
			if (strlen(pOneLine) > 0)
			{
				vrtAllLines.push_back(getOneLine);
			}	
		}

		memset(pOneLine, 0, def_int_cfguse_OneLinLen);


	}//while

	// close file
	if (IfstreamSrcFile.is_open())
	{
		IfstreamSrcFile.close();
	}

	if (NULL != pOneLine)
	{
		delete[] (pOneLine);
		pOneLine = NULL;
	}
	chPtr = NULL;

	nFunRes = 0;

	return nFunRes;
}

int CDbSynTestClientManager::_GetNameValueFormCfg(std::string& strName, std::string& strValue)
{
	int nFunRes = 0;
	std::string strNameSrc = strName;
	std::string strValueSrc = strValue;
	std::string strNameDefine;
	

	_MysysStrUpper(strNameSrc);
	
	
	
	
	
	strNameDefine = defConfig_ServerIP;
	_MysysStrUpper(strNameDefine);
	if (std::string::npos != strNameSrc.find(strNameDefine))
	{
		g_CfgSimInfo.m_strCfg_ServerIP = strValueSrc;
		return nFunRes;
	}
	
	strNameDefine = defConfig_ServerPort;
	_MysysStrUpper(strNameDefine);
	if (std::string::npos != strNameSrc.find(strNameDefine))
	{
		g_CfgSimInfo.m_strCfg_ServerPort = strValueSrc;
		return nFunRes;
	}
	
	
	strNameDefine = defConfig_DBCONSTR;
	_MysysStrUpper(strNameDefine);
	if (std::string::npos != strNameSrc.find(strNameDefine))
	{
		//SrcDbConStr = Oracle:TRA_OCC:IT271350_5:IT271350_5
		g_CfgSimInfo.m_strCfg_SrcDbConStr = strValueSrc;
		return nFunRes;
	}

	
	strNameDefine = defConfig_QueueName;
	_MysysStrUpper(strNameDefine);
	if (std::string::npos != strNameSrc.find(strNameDefine))
	{
		g_CfgSimInfo.m_strCfg_QueueName = strValueSrc;
		return nFunRes;
	}

	
	strNameDefine = defConfig_ClientCount;
	_MysysStrUpper(strNameDefine);
	if (std::string::npos != strNameSrc.find(strNameDefine))
	{
		g_CfgSimInfo.m_strCfg_ClientCount = strValueSrc;
		return nFunRes;
	}
	
	   	
	strNameDefine = defConfig_Recipients;
	_MysysStrUpper(strNameDefine);
	if (std::string::npos != strNameSrc.find(strNameDefine))
	{
		g_CfgSimInfo.m_strCfg_Recipients = strValueSrc;
		return nFunRes;
	}
	
	
	strNameDefine = defConfig_StartSeqNum;
	_MysysStrUpper(strNameDefine);
	if (std::string::npos != strNameSrc.find(strNameDefine))
	{
		g_CfgSimInfo.m_strCfg_StartSeqNum = strValueSrc;
		return nFunRes;
	}
	
	strNameDefine = defConfig_EndSeqNum;
	_MysysStrUpper(strNameDefine);
	if (std::string::npos != strNameSrc.find(strNameDefine))
	{
		g_CfgSimInfo.m_strCfg_EndSeqNum = strValueSrc;
		return nFunRes;
	}
	

	strNameDefine = defConfig_NumberofInsert;
	_MysysStrUpper(strNameDefine);
	if (std::string::npos != strNameSrc.find(strNameDefine))
	{
		g_CfgSimInfo.m_strCfg_NumberofInsert = strValueSrc;
		return nFunRes;
	}
		  	
	
	
	strNameDefine = defConfig_IntervalTimeofInsert;
	_MysysStrUpper(strNameDefine);
	if (std::string::npos != strNameSrc.find(strNameDefine))
	{
		g_CfgSimInfo.m_strCfg_IntervalTimeofInsert = strValueSrc;
		return nFunRes;
	}
	
	strNameDefine = defConfig_NumberofGroup;
	_MysysStrUpper(strNameDefine);
	if (std::string::npos != strNameSrc.find(strNameDefine))
	{
		g_CfgSimInfo.m_strCfg_NumberofGroup = strValueSrc;
		return nFunRes;
	}
	  	
	strNameDefine = defConfig_IntervalTimeofGroup;
	_MysysStrUpper(strNameDefine);
	if (std::string::npos != strNameSrc.find(strNameDefine))
	{
		g_CfgSimInfo.m_strCfg_IntervalTimeofGroup = strValueSrc;
		return nFunRes;
	}
	
	strNameDefine = defConfig_NumberofPublic;
	_MysysStrUpper(strNameDefine);
	if (std::string::npos != strNameSrc.find(strNameDefine))
	{
		g_CfgSimInfo.m_strCfg_NumberofPublic = strValueSrc;
		return nFunRes;
	}

	
	strNameDefine = defConfig_IntervalTimeofPublic;
	_MysysStrUpper(strNameDefine);
	if (std::string::npos != strNameSrc.find(strNameDefine))
	{
		g_CfgSimInfo.m_strCfg_IntervalTimeofPublic = strValueSrc;
		return nFunRes;
	}
	
	strNameDefine = defConfig_NumberofMMS;
	_MysysStrUpper(strNameDefine);
	if (std::string::npos != strNameSrc.find(strNameDefine))
	{
		g_CfgSimInfo.m_strCfg_NumberofMMS = strValueSrc;
		return nFunRes;
	}
	
	
	strNameDefine = defConfig_IntervalTimeofMMS;
	_MysysStrUpper(strNameDefine);
	if (std::string::npos != strNameSrc.find(strNameDefine))
	{
		g_CfgSimInfo.m_strCfg_IntervalTimeofMMS = strValueSrc;
		return nFunRes;
	}



	
	
	//#define  defConfig_Replace_NO                 "#No#"
	//#define  defConfig_Replace_Recipient          "#Recipient#"
	
	strNameDefine = defConfig_AuditDataOracle;
	_MysysStrUpper(strNameDefine);
	if (std::string::npos != strNameSrc.find(strNameDefine))
	{
		g_CfgSimInfo.m_strCfg_AuditDataOracle = strValueSrc;
		return nFunRes;
	}
	
	
	strNameDefine = defConfig_AuditDataMysql;
	_MysysStrUpper(strNameDefine);
	if (std::string::npos != strNameSrc.find(strNameDefine))
	{
		g_CfgSimInfo.m_strCfg_AuditDataMysql = strValueSrc;
		return nFunRes;
	}
	

	
	
	
	
	strNameDefine = defConfig_PackageSize;
	_MysysStrUpper(strNameDefine);
	if (std::string::npos != strNameSrc.find(strNameDefine))
	{
		g_CfgSimInfo.m_strCfg_PackageSize = strValueSrc;
		return nFunRes;
	}

	
	
	
	
	strNameDefine = defConfig_IntervalTime;
	_MysysStrUpper(strNameDefine);
	if (std::string::npos != strNameSrc.find(strNameDefine))
	{
		g_CfgSimInfo.m_strCfg_IntervalTime = strValueSrc;
		return nFunRes;
	}
	
	strNameDefine = defConfig_PerBatchTimeOut;
	_MysysStrUpper(strNameDefine);
	if (std::string::npos != strNameSrc.find(strNameDefine))
	{
		g_CfgSimInfo.m_strCfg_PerBatchTimeOut = strValueSrc;
		return nFunRes;
	}
	
	strNameDefine = defConfig_TestDuration;
	_MysysStrUpper(strNameDefine);
	if (std::string::npos != strNameSrc.find(strNameDefine))
	{
		g_CfgSimInfo.m_strCfg_TestDuration = strValueSrc;
		return nFunRes;
	}

	
	
	
	strNameDefine = defConfig_LogLevel;
	_MysysStrUpper(strNameDefine);
	if (std::string::npos != strNameSrc.find(strNameDefine))
	{
		g_CfgSimInfo.m_strCfg_loglevel = strValueSrc;
		return nFunRes;
	}
	
	strNameDefine = defConfig_LogFileSize;
	_MysysStrUpper(strNameDefine);
	if (std::string::npos != strNameSrc.find(strNameDefine))
	{
		g_CfgSimInfo.m_strCfg_logfilesize = strValueSrc;
		return nFunRes;
	}


	strNameDefine =  defConfig_LogFileCount;
	_MysysStrUpper(strNameDefine);
	if (std::string::npos != strNameSrc.find(strNameDefine))
	{
		g_CfgSimInfo.m_strCfg_logfilecount = strValueSrc;
		return nFunRes;
	}
	
	strNameDefine = defConfig_LogFileName;
	_MysysStrUpper(strNameDefine);
	if (std::string::npos != strNameSrc.find(strNameDefine))
	{
		g_CfgSimInfo.m_strCfg_logfilename = strValueSrc;
		return nFunRes;
	}

   

	return nFunRes;
}

int CDbSynTestClientManager::_TurnToOneLineInAllLine( VrtLinesConT& vrtAllLines )
{
	int nFunRet = 0;

	std::string strGetLine;
	std::string strTmp;

	VrtLinesConIterT iterAllLine = vrtAllLines.begin();
	VrtLinesConIterT iterTmp = vrtAllLines.begin();

	bool bFindJia = false;

	//int nLineCont = vrtAllLines.size();

	strGetLine.clear();
	strTmp.clear();

	while ( iterAllLine != vrtAllLines.end() )
	{
		strGetLine.clear();
		strGetLine = (*iterAllLine);

		_MysysGetVisableStdStr(strGetLine);
		_MysysTrim(strGetLine);

		if (!strGetLine.empty())
		{ 		
			bFindJia = false;
			if ('+' == strGetLine[0])  //#define LINE_ADD '+'
			{
				bFindJia = true;
			} 	

			if (bFindJia)
			{
				//strTmp += "\\";
				//strTmp += "\n";
				strTmp +=strGetLine.substr(1);   //'+' -- '\'  '\r\n'
				(*iterAllLine) = "";
			}
			else
			{
				if (!strTmp.empty())
				{
					//strTmp += "\r\n";
					(*iterTmp) = strTmp;
					strTmp.erase();
				}
				iterTmp = iterAllLine;
				strTmp +=strGetLine;
			}
		}//if (!strGetLine.empty()) 

		iterAllLine++;
	}//while

	//last one line
	if ((iterAllLine == vrtAllLines.end()) && (iterTmp != vrtAllLines.end()))
	{
		if (!strTmp.empty())
		{
			//strTmp += "\r\n";
			(*iterTmp) = strTmp;
			strTmp.erase();
			strGetLine.clear();	 
		}
	}


	nFunRet = 0;
	return nFunRet;
}
int CDbSynTestClientManager::_ProcessVrtLines( VrtLinesConT& vrtAllLines )
{
	int nFunRes = 0;

	std::string::size_type nFindTmp = std::string::npos;

	int								nLinesCount = -1;		 
	VrtLinesConIterT				vtrLineIter;
	std::string	  strAttriName;
	std::string	  strAttriVal;
	std::string   strAttriMiddle;
	
	
	if (vrtAllLines.empty())
	{
		nFunRes = -1;
		return nFunRes;
	} 
	strAttriMiddle = def_str_cfguse_AttriMiddle;

	nLinesCount = (int)vrtAllLines.size();
	vtrLineIter = vrtAllLines.begin();
	while ( vtrLineIter != vrtAllLines.end() )
	{
		std::string strOneLine; 
		strOneLine = (*vtrLineIter);

		if (strOneLine.empty())
		{
			vtrLineIter++;
			continue;
		}
		
		if (std::string::npos != strOneLine.find(def_str_cfguse_AttriMiddle))
		{
			strAttriName = strOneLine.substr(0, strOneLine.find(def_str_cfguse_AttriMiddle));
			strAttriVal = strOneLine.substr(strOneLine.find(def_str_cfguse_AttriMiddle) + strAttriMiddle.length());

			_MysysGetVisableStdStr(strAttriName);
			_MysysGetVisableStdStr(strAttriVal);
			
			_MysysTrim(strAttriName);
			_MysysTrim(strAttriVal);
			_GetNameValueFormCfg(strAttriName, strAttriVal);

		}
		

		vtrLineIter++;

	}//while 


	return nFunRes;
}


int CDbSynTestClientManager::_RemoveBlackLineInAllLine( VrtLinesConT& vrtAllLines )
{
	int nFunRet = 0;

	int nSizeCount = -1;

	VrtLinesConIterT vtrLineIter = vrtAllLines.begin();

	nSizeCount = vrtAllLines.size();
	vtrLineIter = vrtAllLines.begin();
	while (vtrLineIter != vrtAllLines.end())
	{
		if ( (*vtrLineIter).length() < 1)
		{
			vrtAllLines.erase(vtrLineIter);
			nSizeCount = vrtAllLines.size();
			vtrLineIter = vrtAllLines.begin();
		}
		else
		{
			vtrLineIter++;	
		}  			
	}//while	  


	return nFunRet;
}
int	CDbSynTestClientManager::_ConfigFileOpen(const std::string& strConfigFileName)
{
	int nFunRes = 0;
	VrtLinesConT vrtAllLines;

	g_CfgSimInfo.m_strConfigFileName = strConfigFileName;

	_ReadAllLineFromFile(strConfigFileName, vrtAllLines);
	_TurnToOneLineInAllLine(vrtAllLines);
	_RemoveBlackLineInAllLine(vrtAllLines);
	_ProcessVrtLines(vrtAllLines);

	vrtAllLines.clear();

	return nFunRes;
} 


int  CDbSynTestClientManager::_ParseLogLevel(const std::string& strValue, DebugUtil::EDebugLevel& nLogLevel)
{
	int  nRet = 0;
	LogLevelNameValue* nv = logNameValueMapping;

	while (nv->name != NULL)
	{
		if (!CUtilityFun::stricmp_(strValue.c_str(), nv->name))
		{
			nLogLevel = nv->value;
			break;
		}
		nv++;
	}

	return nRet;
}
 //////////////////////////////////////////////////////////////////////////

int	CDbSynTestClientManager::_ProceeReplaceConfigString()
{
	int nFunRes = 0;
	std::string   strFormat;
	int nFind = -1;
	std::string   strSrc;
	std::string   strReplaceNo;
	std::string   strReplaceRecipient;
	std::string strSubStrTemp;


	if (false == g_CfgSimInfo.m_strCfg_AuditDataOracle.empty())
	{
		std::string strBigSrc;
		std::string strBigSub;
		//AuditDataOracle=insert into queue_test_mysql(ID, NAME, TIMESTAMP) values(#No#, #Recipient#, sysdate)
		strFormat = "%llu";
		strReplaceNo = defConfig_Replace_NO;
		strSrc = g_CfgSimInfo.m_strCfg_AuditDataOracle;


		strBigSrc = strSrc;
		strBigSub = strReplaceNo;
		_MysysStrUpper(strBigSrc);
		_MysysStrUpper(strBigSub);
		nFind = (int)strBigSrc.find(strBigSub);
		if (nFind >= 0)
		{
			strSrc.replace(nFind, strReplaceNo.size(), strFormat);
		}

		//nFind = (int)strSrc.find(strReplaceNo);
		g_SynProgramSimInfo.m_szFormat_AuditDataOracle = strSrc;

		strFormat = "'%s'";
		strReplaceRecipient = defConfig_Replace_Recipient;
		strSrc = g_SynProgramSimInfo.m_szFormat_AuditDataOracle;

		strBigSrc = strSrc;
		strBigSub = strReplaceRecipient;
		_MysysStrUpper(strBigSrc);
		_MysysStrUpper(strBigSub);
		nFind = (int)strBigSrc.find(strBigSub);
		if (nFind >= 0)
		{
			strSrc.replace(nFind, strReplaceRecipient.size(), strFormat);
		}

		//nFind = (int)strSrc.find(strReplaceRecipient);
		//strSrc.replace(nFind, strReplaceRecipient.size(), strFormat);
		//AuditDataOracle=insert into queue_test_mysql(ID, NAME, TIMESTAMP) values(%llu, '%s', sysdate)
		g_SynProgramSimInfo.m_szFormat_AuditDataOracle = strSrc;
		//g_CfgSimInfo.m_strCfg_AuditDataOracle
	}
	else
	{
		g_SynProgramSimInfo.m_szFormat_AuditDataOracle.clear();
	}


	if (false == g_CfgSimInfo.m_strCfg_AuditDataMysql.empty())
	{
		std::string strBigSrc;
		std::string strBigSub;

		//AuditDataMysql=insert into queue_test_mysql(ID, NAME, TIMESTAMP) values(#No#, #Recipient#, sysdate())
		strFormat = "%llu";
		strReplaceNo = defConfig_Replace_NO;
		strSrc = g_CfgSimInfo.m_strCfg_AuditDataMysql;

		strBigSrc = strSrc;
		strBigSub = strReplaceNo;
		_MysysStrUpper(strBigSrc);
		_MysysStrUpper(strBigSub);
		nFind = (int)strBigSrc.find(strBigSub);
		if (nFind >= 0)
		{
			strSrc.replace(nFind, strReplaceNo.size(), strFormat);
		}

		//nFind = (int)strSrc.find(strReplaceNo);
		//strSrc.replace(nFind, strReplaceNo.size(), strFormat);
		g_SynProgramSimInfo.m_szFormat_AuditDataMysql = strSrc;


		strFormat = "'%s'";
		strReplaceRecipient = defConfig_Replace_Recipient;
		strSrc = g_SynProgramSimInfo.m_szFormat_AuditDataMysql;

		strBigSrc = strSrc;
		strBigSub = strReplaceRecipient;
		_MysysStrUpper(strBigSrc);
		_MysysStrUpper(strBigSub);
		nFind = (int)strBigSrc.find(strBigSub);
		if (nFind >= 0)
		{
			strSrc.replace(nFind, strReplaceRecipient.size(), strFormat);
		}

		//nFind = (int)strSrc.find(strReplaceRecipient);
		//strSrc.replace(nFind, strReplaceRecipient.size(), strFormat);
		//AuditDataMysql=insert into queue_test_mysql(ID, NAME, TIMESTAMP) values(%llu, '%s', sysdate())
		g_SynProgramSimInfo.m_szFormat_AuditDataMysql = strSrc;
		//g_CfgSimInfo.m_strCfg_AuditDataMysql
	}
	else
	{
		g_SynProgramSimInfo.m_szFormat_AuditDataMysql.clear();
	}

	return nFunRes;
}
 
 
int	CDbSynTestClientManager::_ProceeParam()
{
	int nFunRes = 0; 
	std::string   strFormat;
	int nFind = -1;
	std::string   strSrc;
	std::string   strReplaceNo;
	std::string   strReplaceRecipient;
	std::string strSubStrTemp;

	
	if (false == g_CfgSimInfo.m_strCfg_ClientCount.empty())
	{
		g_SynProgramSimInfo.m_n_ClientCount = atoi(g_CfgSimInfo.m_strCfg_ClientCount.c_str());
	}
	else
	{
		g_SynProgramSimInfo.m_n_ClientCount = -1;
	}
	
	if (false == g_CfgSimInfo.m_strCfg_StartSeqNum.empty())
	{
		//m_n_StartNum = atoi(m_strCfg_StartNum.c_str());
		g_SynProgramSimInfo.m_n_StartSeqNum = CUtilityFun::atoui64(g_CfgSimInfo.m_strCfg_StartSeqNum);
	}
	else
	{
		g_SynProgramSimInfo.m_n_StartSeqNum = 0;
	}
	

	if (false == g_CfgSimInfo.m_strCfg_EndSeqNum.empty())
	{
		g_SynProgramSimInfo.m_n_EndSeqNum = CUtilityFun::atoui64(g_CfgSimInfo.m_strCfg_EndSeqNum);
	}
	else
	{
		//m_n_EndSeqNum = m_n_StartSeqNum + 5000*10000;
		g_SynProgramSimInfo.m_n_EndSeqNum = 0;
	}
	 
	
	if (false == g_CfgSimInfo.m_strCfg_NumberofInsert.empty())
	{
		g_SynProgramSimInfo.m_n_NumberofInsert = atoi(g_CfgSimInfo.m_strCfg_NumberofInsert.c_str());
	}
	else
	{
		g_SynProgramSimInfo.m_n_NumberofInsert = 0;
	}
	
	if (false == g_CfgSimInfo.m_strCfg_NumberofGroup.empty())
	{
		g_SynProgramSimInfo.m_n_NumberofGroup = atoi(g_CfgSimInfo.m_strCfg_NumberofGroup.c_str());
	}
	else
	{
		g_SynProgramSimInfo.m_n_NumberofGroup = 0;
	}
	
	if (false == g_CfgSimInfo.m_strCfg_NumberofPublic.empty())
	{
		g_SynProgramSimInfo.m_n_NumberofPublic = atoi(g_CfgSimInfo.m_strCfg_NumberofPublic.c_str());
	}
	else
	{
		g_SynProgramSimInfo.m_n_NumberofPublic = 0;
	}
	
	
	if (false == g_CfgSimInfo.m_strCfg_NumberofMMS.empty())
	{
		g_SynProgramSimInfo.m_n_NumberofMMS = atoi(g_CfgSimInfo.m_strCfg_NumberofMMS.c_str());
	}
	else
	{
		g_SynProgramSimInfo.m_n_NumberofMMS = 0;
	}
	
	
	
	
	if (false == g_CfgSimInfo.m_strCfg_IntervalTimeofInsert.empty())
	{
		g_SynProgramSimInfo.m_n_IntervalTimeofInsert = atoi(g_CfgSimInfo.m_strCfg_IntervalTimeofInsert.c_str());
	}
	else
	{
		g_SynProgramSimInfo.m_n_IntervalTimeofInsert = 1000;
	}
	
	
	
	if (false == g_CfgSimInfo.m_strCfg_IntervalTimeofGroup.empty())
	{
		g_SynProgramSimInfo.m_n_IntervalTimeofGroup = atoi(g_CfgSimInfo.m_strCfg_IntervalTimeofGroup.c_str());
	}
	else
	{
		g_SynProgramSimInfo.m_n_IntervalTimeofGroup = 1000;
	}
	
	if (false == g_CfgSimInfo.m_strCfg_IntervalTimeofPublic.empty())
	{
		g_SynProgramSimInfo.m_n_IntervalTimeofPublic = atoi(g_CfgSimInfo.m_strCfg_IntervalTimeofPublic.c_str());
	}
	else
	{
		g_SynProgramSimInfo.m_n_IntervalTimeofPublic = 1000;
	}
	
	
	if (false == g_CfgSimInfo.m_strCfg_IntervalTimeofMMS.empty())
	{
		g_SynProgramSimInfo.m_n_IntervalTimeofMMS = atoi(g_CfgSimInfo.m_strCfg_IntervalTimeofMMS.c_str());
	}
	else
	{
		g_SynProgramSimInfo.m_n_IntervalTimeofMMS = 1000;
	}


	


	if (false == g_CfgSimInfo.m_strCfg_PackageSize.empty())
	{
		g_SynProgramSimInfo.m_n_PackageSize = atoi(g_CfgSimInfo.m_strCfg_PackageSize.c_str());
		g_SynProgramSimInfo.m_n_PackageSize = g_SynProgramSimInfo.m_n_PackageSize * 1024;//kB
	}
	else
	{
		g_SynProgramSimInfo.m_n_PackageSize = 0;
	}

	
	if (false == g_CfgSimInfo.m_strCfg_IntervalTime.empty())
	{
		//millsecond to	 millsecond
		g_SynProgramSimInfo.m_n_IntervalTime = atoi(g_CfgSimInfo.m_strCfg_IntervalTime.c_str());
	}
	else
	{
		g_SynProgramSimInfo.m_n_IntervalTime = 1000;
	}
	
	if (false == g_CfgSimInfo.m_strCfg_PerBatchTimeOut.empty())
	{
		//second to	 millsecond
		 g_SynProgramSimInfo.m_n_PerBatchTimeOut = atoi(g_CfgSimInfo.m_strCfg_PerBatchTimeOut.c_str());
		 g_SynProgramSimInfo.m_n_PerBatchTimeOut = g_SynProgramSimInfo.m_n_PerBatchTimeOut * 1000;
	}
	else
	{
		g_SynProgramSimInfo.m_n_PerBatchTimeOut = 1;
	}
	
	if (false == g_CfgSimInfo.m_strCfg_TestDuration.empty())
	{
		//minute to	 millsecond
		g_SynProgramSimInfo.m_n_TestDuration = atol(g_CfgSimInfo.m_strCfg_TestDuration.c_str());
		g_SynProgramSimInfo.m_n_TestDuration = g_SynProgramSimInfo.m_n_TestDuration * 1000 * 60;
	}
	else
	{
		g_SynProgramSimInfo.m_n_TestDuration = -1;
	}
	 	
	if (false == g_CfgSimInfo.m_strCfg_loglevel.empty())
	{
		DebugUtil::EDebugLevel nLogLevel;
		_ParseLogLevel(g_CfgSimInfo.m_strCfg_loglevel, nLogLevel);
		g_SynProgramSimInfo.m_DU_LogLevel = (int)nLogLevel;

	}
	else
	{
		g_SynProgramSimInfo.m_DU_LogLevel = (int)DebugUtil::DebugDebug;
	}

	if (false == g_CfgSimInfo.m_strCfg_logfilecount.empty())
	{
		g_SynProgramSimInfo.m_n_logfilecount = atoi(g_CfgSimInfo.m_strCfg_logfilecount.c_str());
	}
	else
	{
		g_SynProgramSimInfo.m_n_logfilecount = 100;
	}

	if (false == g_CfgSimInfo.m_strCfg_logfilesize.empty())
	{
		g_SynProgramSimInfo.m_n_logfilesize = atoi(g_CfgSimInfo.m_strCfg_logfilesize.c_str());
	}
	else
	{
		g_SynProgramSimInfo.m_n_logfilesize = 20*1024*1024;
	}
	//_ProceeReplaceConfigString();

	g_SynProgramSimInfo.m_szFormat_AuditDataOracle = g_CfgSimInfo.m_strCfg_AuditDataOracle;

	g_SynProgramSimInfo.m_szFormat_AuditDataMysql = g_CfgSimInfo.m_strCfg_AuditDataMysql;


	return nFunRes;
}


int	CDbSynTestClientManager::_LogPrintParam()
{
	int nFunRes = 0; 

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[m_strCfg_ServerIP=%s]", g_CfgSimInfo.m_strCfg_ServerIP.c_str());
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[m_strCfg_ServerPort=%s]", g_CfgSimInfo.m_strCfg_ServerPort.c_str());
		
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[m_strCfg_SrcDbConStr=%s]", g_CfgSimInfo.m_strCfg_SrcDbConStr.c_str());
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[m_strCfg_QueueName=%s]", g_CfgSimInfo.m_strCfg_QueueName.c_str());
   

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[m_n_ClientCount=%d]", g_SynProgramSimInfo.m_n_ClientCount);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[m_strCfg_Recipients=%s]", g_CfgSimInfo.m_strCfg_Recipients.c_str());
		
	if (0)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"[m_n_Default_SRVID=%d]", g_SynProgramSimInfo.m_n_Default_SRVID);  

	}//if (0)
	
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[m_n_StartSeqNum=%llu]", g_SynProgramSimInfo.m_n_StartSeqNum);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[m_n_EndSeqNum=%llu]", g_SynProgramSimInfo.m_n_EndSeqNum);
		
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[m_n_NumberofInsert=%d]", g_SynProgramSimInfo.m_n_NumberofInsert);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[m_n_NumberofGroup=%d]", g_SynProgramSimInfo.m_n_NumberofGroup);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[m_n_NumberofPublic=%d]", g_SynProgramSimInfo.m_n_NumberofPublic);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[m_n_NumberofMMS=%d]", g_SynProgramSimInfo.m_n_NumberofMMS);

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[m_n_IntervalTimeofInsert=%d]", g_SynProgramSimInfo.m_n_IntervalTimeofInsert);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[m_n_IntervalTimeofGroup=%d]", g_SynProgramSimInfo.m_n_IntervalTimeofGroup);	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[m_n_IntervalTimeofPublic=%d]", g_SynProgramSimInfo.m_n_IntervalTimeofPublic);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[m_n_IntervalTimeofMMS=%d]", g_SynProgramSimInfo.m_n_IntervalTimeofMMS);

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[m_szFormat_AuditDataOracle=%s]", g_SynProgramSimInfo.m_szFormat_AuditDataOracle.c_str());
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[m_szFormat_AuditDataMysql=%s]", g_SynProgramSimInfo.m_szFormat_AuditDataMysql.c_str());
		 
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			 "[m_n_PackageSize=%d]", g_SynProgramSimInfo.m_n_PackageSize);
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[m_n_IntervalTime=%d]", g_SynProgramSimInfo.m_n_IntervalTime);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[m_n_PerBatchTimeOut=%d]", g_SynProgramSimInfo.m_n_PerBatchTimeOut);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[m_n_TestDuration=%d]", g_SynProgramSimInfo.m_n_TestDuration);



	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[m_strCfg_loglevel=%s]", g_CfgSimInfo.m_strCfg_loglevel.c_str());
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[m_n_logfilecount=%d]", g_SynProgramSimInfo.m_n_logfilecount);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[m_n_logfilesize=%d]", g_SynProgramSimInfo.m_n_logfilesize);  


	return nFunRes;
}


int	CDbSynTestClientManager::_CheckCfgParam()
{
	int nFunRes = 0;
	
	if (true == g_CfgSimInfo.m_strCfg_ServerIP.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"error! [true == m_strCfg_ServerIP.empty()][m_strCfg_ServerIP=%s]", 
			g_CfgSimInfo.m_strCfg_ServerIP.c_str());
		nFunRes = -1;
		return nFunRes;
	}
	
	if (true == g_CfgSimInfo.m_strCfg_ServerPort.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"error! [true == m_strCfg_ServerPort.empty()][m_strCfg_ServerPort=%s]", 
			g_CfgSimInfo.m_strCfg_ServerPort.c_str());
		nFunRes = -1;
		return nFunRes;
	}
	
	if (true == g_CfgSimInfo.m_strCfg_QueueName.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"error! [true == m_strCfg_QueueName.empty()][m_strCfg_QueueName=%s]", 
			g_CfgSimInfo.m_strCfg_QueueName.c_str());
		nFunRes = -1;
		return nFunRes;
	}
	

	  
	if (0)
	{
		if (0 >= g_SynProgramSimInfo.m_n_ClientCount)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"error! [0 <= m_n_ClientCount][m_n_ClientCount=%d]", g_SynProgramSimInfo.m_n_ClientCount);
			nFunRes = -1;
			return nFunRes;
		}
	
		
		
	
	}//if (0)
		
	
	

	
	
	if (0 > g_SynProgramSimInfo.m_n_StartSeqNum)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"error! [0 > m_n_StartNum][m_n_StartNum=%d]", 
			g_SynProgramSimInfo.m_n_StartSeqNum);
		nFunRes = -1;
		return nFunRes;
	}
	
	if (0 > g_SynProgramSimInfo.m_n_EndSeqNum)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"error! [0 > m_n_EndNum][m_n_EndNum=%d]", 
			g_SynProgramSimInfo.m_n_EndSeqNum);
		nFunRes = -1;
		return nFunRes;
	}

	if (0 > g_SynProgramSimInfo.m_n_NumberofInsert)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"error! [0 > m_n_NumberofInsert][m_n_NumberofInsert=%d]", 
			g_SynProgramSimInfo.m_n_NumberofInsert);
		nFunRes = -1;
		return nFunRes;
	}
	
	if (0 > g_SynProgramSimInfo.m_n_NumberofGroup)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"error! [0 > m_n_NumberofGroup][m_n_NumberofGroup=%d]", 
			g_SynProgramSimInfo.m_n_NumberofGroup);
		nFunRes = -1;
		return nFunRes;
	}
	
	if (0 > g_SynProgramSimInfo.m_n_NumberofPublic)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"error! [0 > m_n_NumberofPublic][m_n_NumberofPublic=%d]", 
			g_SynProgramSimInfo.m_n_NumberofPublic);
		nFunRes = -1;
		return nFunRes;
	}
	
	if (0 > g_SynProgramSimInfo.m_n_NumberofMMS)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"error! [0 > m_n_NumberofMMS][m_n_NumberofMMS=%d]", 
			g_SynProgramSimInfo.m_n_NumberofMMS);
		nFunRes = -1;
		return nFunRes;
	}


	
	if (0 >= g_SynProgramSimInfo.m_n_IntervalTimeofInsert)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"error! [0 >= m_n_InsertTypeMsgIntervalTime][m_n_InsertTypeMsgIntervalTime=%d]", 
			g_SynProgramSimInfo.m_n_IntervalTimeofInsert);
		nFunRes = -1;
		return nFunRes;
	}
	
	
	if (0 >= g_SynProgramSimInfo.m_n_IntervalTimeofGroup)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"error! [0 >= m_n_GroupTypeMsgIntervalTime][m_n_GroupTypeMsgIntervalTime=%d]", 
			g_SynProgramSimInfo.m_n_IntervalTimeofGroup);
		nFunRes = -1;
		return nFunRes;
	}
	
	if (0 >= g_SynProgramSimInfo.m_n_IntervalTimeofPublic)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"error! [0 >= m_n_PublicTypeMsgIntervalTime][m_n_PublicTypeMsgIntervalTime=%d]", 
			g_SynProgramSimInfo.m_n_IntervalTimeofPublic);
		nFunRes = -1;
		return nFunRes;
	}
	
	if (0 >= g_SynProgramSimInfo.m_n_IntervalTimeofMMS)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"error! [0 >= m_n_IntervalTimeofMMS][m_n_IntervalTimeofMMS=%d]", 
			g_SynProgramSimInfo.m_n_IntervalTimeofMMS);
		nFunRes = -1;
		return nFunRes;
	}


	if (true == g_SynProgramSimInfo.m_szFormat_AuditDataOracle.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"error! [true == m_szFormat_AuditDataOracle.empty()][m_szFormat_AuditDataOracle=%s]",
			g_SynProgramSimInfo.m_szFormat_AuditDataOracle.c_str());
		nFunRes = -1;
		return nFunRes;
	}
	
	if (true == g_SynProgramSimInfo.m_szFormat_AuditDataMysql.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"error! [true == m_szFormat_AuditDataMysql.empty()][m_szFormat_AuditDataMysql=%s]", 
			g_SynProgramSimInfo.m_szFormat_AuditDataMysql.c_str());
		nFunRes = -1;
		return nFunRes;
	}


	
	if (0 >= g_SynProgramSimInfo.m_n_PackageSize)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"error! [0 >= m_n_PackageSize][m_n_PackageSize=%d]", 
			g_SynProgramSimInfo.m_n_PackageSize);
		nFunRes = -1;
		return nFunRes;
	}
	
	if (0 > g_SynProgramSimInfo.m_n_IntervalTime)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"error! [0 > m_n_IntervalTime][m_n_IntervalTime=%d]", 
			g_SynProgramSimInfo.m_n_IntervalTime);
		nFunRes = -1;
		return nFunRes;
	}
	if (0 > g_SynProgramSimInfo.m_n_PerBatchTimeOut)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"error! [0 > m_n_PerBatchTimeOut][m_n_PerBatchTimeOut=%d]", 
			g_SynProgramSimInfo.m_n_PerBatchTimeOut);
		nFunRes = -1;
		return nFunRes;
	}
	
	if (0 > g_SynProgramSimInfo.m_n_TestDuration)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"error! [0 > m_n_TestDuration][m_n_TestDuration=%d]", 
			g_SynProgramSimInfo.m_n_TestDuration);
		nFunRes = -1;
		return nFunRes;
	}


	return nFunRes;
}

int	CDbSynTestClientManager::_getAllClientAgtNameFromRecipients(
											const std::string& strINRecipients, 
											mapIntStrConT& mapOUTClientAgtName)
{
	int nFunRes = 0;
	std::string	 strRecs;
	std::string	 strSubString;
	std::string	 strGetOneAgtName;
	int nIndex = 0;

	int nFindOne = 0;
	
	if (strINRecipients.empty())
	{
		nFunRes = -1;
		return nFunRes;
	}
	mapOUTClientAgtName.clear();
	
	strRecs = strINRecipients;
	nIndex = 0;
	while (1)
	{
		nFindOne = -1;
		strSubString.clear();
		strGetOneAgtName.clear();


		nFindOne = (int)strRecs.find(",");
		if (nFindOne >= 0)
		{
			//TRA_DT_001,TRA_DT_002,
			strSubString = strRecs.substr(0, nFindOne);
			strGetOneAgtName = strSubString;
			
			//TRA_DT_001
			//TRA_DT_002
			_MysysTrim(strGetOneAgtName);			
			if (false == strGetOneAgtName.empty())
			{
				nIndex++;
				m_mapClientIndexAgtName.insert(mapIntStrConVT(nIndex, strGetOneAgtName));
			}
			
			//TRA_DT_002,
			//""
			strSubString = strRecs.substr(nFindOne+1);
			strRecs = strSubString;			
		}
		else
		{
			if (false == strRecs.empty())
			{
				strGetOneAgtName = strRecs;
				_MysysTrim(strGetOneAgtName);			
				if (false == strGetOneAgtName.empty())
				{
					nIndex++;
					m_mapClientIndexAgtName.insert(mapIntStrConVT(nIndex, strGetOneAgtName));
				} 
			}
			
			break;//while (1)
		}
	
	}//while (1)
	
	
	return nFunRes; 
}

int	CDbSynTestClientManager::_CreateClientAgtNameList()
{
	int nFunRes = 0;
	

	return nFunRes;	
}

int CDbSynTestClientManager::_InitClientAgtName(const std::string& strINAgtFormat, int nINClientIndex, std::string& strOUTAgtName)
{
	int nFunRes = 0;
	char* pszAgtTmep = NULL;
	std::string strAgtNameDest;

	pszAgtTmep = new char[def_int_cfguse_Buffer];
	memset(pszAgtTmep, 0, def_int_cfguse_Buffer);

	strAgtNameDest =  strINAgtFormat;

	if ((nINClientIndex >=0) && (nINClientIndex <=9))
	{
		sprintf(pszAgtTmep, "_00%d", nINClientIndex);	   
	}
	else if ((nINClientIndex >=10) && (nINClientIndex <=99))
	{
		sprintf(pszAgtTmep, "_0%d", nINClientIndex);	   
	}
	else 
	{
		sprintf(pszAgtTmep, "_%d", nINClientIndex);	   
	}

	strAgtNameDest += pszAgtTmep;
	strOUTAgtName = strAgtNameDest;		
	return nFunRes;	
}



int	CDbSynTestClientManager::_UpdateTable(TA_AQ_Lib::IDatabase* pINDataBase, 
std::string& strINLocationKey, std::string& strINQueueName, int& nINSrvID,ulong64 nValueMsgSID)
{  
	int					nFunRes = 0;
	std::string			strOracleSQL;
	char*               pszStrTemp= NULL;
	std::string         strMsgSIDValue; 	
	std::string			strSRVIDValue; 

	//m_strDbConStr
	pszStrTemp = new char[def_int_cfguse_Buffer];
	memset(pszStrTemp, 0, def_int_cfguse_Buffer);
	sprintf(pszStrTemp, "%llu", nValueMsgSID);
	strMsgSIDValue = pszStrTemp;
	
	memset(pszStrTemp, 0, def_int_cfguse_Buffer);
	sprintf(pszStrTemp, "%d", nINSrvID);
	strSRVIDValue = pszStrTemp;


    //UPDATE AQ_ENQ_DUPLICATE_CHECK set MSG_SID=1 where UPPER(LOCATION)=UPPER('TRA_DT01') and UPPER(Q_NAME)=UPPER('AUDIT_DATA_QUEUE') and SRV_ID = 1;
	//strOracleSQL = pINDataBase->prepareSQLStatement(ENQ_SQL_SUPDATE_GET_MSGSID_7000,strINLocationKey.c_str(), strINQueueName.c_str());
	strOracleSQL = "UPDATE AQ_ENQ_DUPLICATE_CHECK set MSG_SID = ";
	strOracleSQL += strMsgSIDValue;
	strOracleSQL += "where UPPER(LOCATION)=UPPER('";
	strOracleSQL += strINLocationKey;
	strOracleSQL += "') and UPPER(Q_NAME)=UPPER('";
	strOracleSQL += strINQueueName;
	strOracleSQL += "') and SRV_ID = ";
	strOracleSQL += strSRVIDValue;

	try
	{
		pINDataBase->connect();
		pINDataBase->executeModification(strOracleSQL);
		pINDataBase->commitTransaction();
	}
	catch (...)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 	"error! [strOracleSQL=%s]", strOracleSQL.c_str());
		nFunRes = -1;
	}


	DEF_DELETE_ARRAY(pszStrTemp);
	return nFunRes;
} 
int	CDbSynTestClientManager::_InitOracleDbTableParam()
{
	int nFunRes = 0;
	std::string strSrcDbConStr;
	TA_AQ_Lib::IDatabase* pDataBase = NULL;
	ulong64 nSetMsgSID = 0;
	mapIntStrConIterT iterMap;
	int nClientIndex = 0;
	std::string strClientName;

	
	strSrcDbConStr = g_CfgSimInfo.m_strCfg_SrcDbConStr;
	if (strSrcDbConStr.empty())
	{
		strSrcDbConStr = "ORACLE:TRA_OCC:IT271350_5:IT271350_5";	
	}
	 
	try
	{
		pDataBase = new CDatabaseImpl(strSrcDbConStr.c_str());
		if (NULL != pDataBase)
		{
			pDataBase->connect();
		}
	}
	catch (...)
	{
		nFunRes = -1;
		return nFunRes;
	}

	nSetMsgSID = 0;
	iterMap = m_mapClientIndexAgtName.begin();
	while (iterMap != m_mapClientIndexAgtName.end())
	{
		nClientIndex = (*iterMap).first;
		strClientName = (*iterMap).second;

		_UpdateTable(pDataBase, strClientName, g_CfgSimInfo.m_strCfg_QueueName, g_SynProgramSimInfo.m_n_Default_SRVID, nSetMsgSID);
	
		iterMap++;
	}


	DEF_DELETE(pDataBase);

	return nFunRes;
}

int CDbSynTestClientManager::setConFigFile(const std::string& strCfgFile)
{
	int nFunRes = 0;
	
	if (strCfgFile.empty())
	{
		g_CfgSimInfo.m_strConfigFileName = defconfig_Cfg_FileName;	
	}
	else
	{
		g_CfgSimInfo.m_strConfigFileName = strCfgFile;		
	} 	
	
	return nFunRes;
}


//////////////////////////////////////////////////////////////////////////
 

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

bool    CDbSynTestClientManager::isFinishWork()
{
	bool bIsFinish = false;
	
	if (WorkState_End == m_nWorkState)
	{
		bIsFinish = true;
	}
	
	return bIsFinish;
}

int	CDbSynTestClientManager::_GetALLClientInfo()
{
	int nFunRes = 0;
	CPropagateMsg* pGetClient = NULL;

	mapQThreadPoolIterT iterDbSynTC;
	ThreadGuard guardData(m_lockVrtSynClientsThread);

	_FreeMapSimuClientInfo(m_mapAllClientInfo); 


	iterDbSynTC = m_vrtSynClientsThread.begin();
	while (iterDbSynTC != m_vrtSynClientsThread.end())
	{
		pGetClient = (*iterDbSynTC).second;

		std::string strAgtName;
		SimuClientInfo* pNewClientInfo = NULL;
		pNewClientInfo = new SimuClientInfo();

		pGetClient->getSimuClientInfo(*pNewClientInfo);
		strAgtName = pNewClientInfo->AgentName;

		m_mapAllClientInfo.insert(mapSimuClientInfoValueT(strAgtName, pNewClientInfo));

		iterDbSynTC++;

	} 

	return nFunRes;
}

int	CDbSynTestClientManager::_GetALLClientInfoEx()
{
	int nFunRes = 0;
	CPropagateMsg* pGetClient = NULL;
	SimuClientInfo* pNewClientInfo = NULL;
	mapSimuClientInfoIterT iterMapFind = m_mapAllClientInfo.begin();
	SimuClientInfo* pFindClientInfo = NULL;

	mapQThreadPoolIterT iterDbSynTC;
	ThreadGuard guardData(m_lockVrtSynClientsThread);

	iterDbSynTC = m_vrtSynClientsThread.begin();
	while (iterDbSynTC != m_vrtSynClientsThread.end())
	{
		pGetClient = (*iterDbSynTC).second;

		std::string strAgtName;
		
		if (NULL == pNewClientInfo)
		{
			pNewClientInfo = new SimuClientInfo();
		}
		pGetClient->getSimuClientInfo(*pNewClientInfo);
		strAgtName = pNewClientInfo->AgentName;
		
		iterMapFind = m_mapAllClientInfo.find(strAgtName);
		if (iterMapFind != m_mapAllClientInfo.end())
		{
			//find ok
			pFindClientInfo = iterMapFind->second;
			*pFindClientInfo = *pNewClientInfo;			
		}
		else
		{
			//not find
			m_mapAllClientInfo.insert(mapSimuClientInfoValueT(strAgtName, pNewClientInfo));
			pNewClientInfo = NULL;
		} 

		iterDbSynTC++;

	} 
	
	DEF_DELETE(pNewClientInfo);

	return nFunRes;
}
std::map<std::string, SimuClientInfo*>& CDbSynTestClientManager::getClientInfos()
{		
	ThreadGuard guard(m_lockMapAllClientInfo);
				
	//_GetALLClientInfo();
	_GetALLClientInfoEx();
	return m_mapAllClientInfo;
}

int	CDbSynTestClientManager::continueWork()
{
	int nFunRes = 0;
	CPropagateMsg* pGetClient = NULL;

	mapQThreadPoolIterT iterDbSynTC;
	ThreadGuard guardData(m_lockVrtSynClientsThread); 


	iterDbSynTC = m_vrtSynClientsThread.begin();
	while (iterDbSynTC != m_vrtSynClientsThread.end())
	{
		pGetClient = (*iterDbSynTC).second;

		pGetClient->continueWork();
		iterDbSynTC++;

	}

	return nFunRes;	 
}
int	CDbSynTestClientManager::pauseWork()
{  
	int nFunRes = 0;
	CPropagateMsg* pGetClient = NULL;

	mapQThreadPoolIterT iterDbSynTC;
	ThreadGuard guardData(m_lockVrtSynClientsThread); 


	iterDbSynTC = m_vrtSynClientsThread.begin();
	while (iterDbSynTC != m_vrtSynClientsThread.end())
	{
		pGetClient = (*iterDbSynTC).second;
		
		pGetClient->pauseWork();
		iterDbSynTC++;

	}

	return nFunRes;	 
}



int CDbSynTestClientManager::printStatus()
{
	int nFunRes = 0;
	ThreadGuard guard(m_lockMapAllClientInfo);
	
	_GetALLClientInfoEx();
	
	SimuClientInfo* pGetInfo = NULL;
	mapSimuClientInfoIterT iterInner = m_mapAllClientInfo.begin();
	while (iterInner != m_mapAllClientInfo.end())
	{
		pGetInfo = iterInner->second;

		pGetInfo->logSimuClientInfo();
		iterInner++;
	}


	return nFunRes;	 
}


int CDbSynTestClientManager::_ProcessUserTerminate()
{
	int nFunRes = 0;

	nFunRes = _PocessEManagerStateUnInitNewClients();

	nFunRes = _PocessEManagerStateEndFinishWork();


	return nFunRes;
}


void	CDbSynTestClientManager::run() 
{ 
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"start CDbSynTestClientManager::run()");	
   	
	while (false == m_toTerminate)
	{
		_ThreadJob();
		TA_Base_Core::Thread::sleep(SleepValue_MonitorThread_Sleep);		
	}
	
	pauseWork();
	_ProcessUserTerminate();
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"end CDbSynTestClientManager::run()");	

}


void	CDbSynTestClientManager::terminate()
{
	m_toTerminate = true;
}




int CDbSynTestClientManager::_ThreadJob()
{ 
	int nFunRes = 0;
	
	nFunRes = _PocessEManagerState();
	
	return nFunRes;
}

int CDbSynTestClientManager::_UpdateClientInfoToMap(CPropagateMsg* pClient)
{
	int nFunRes = 0;
	std::string strAgtName;
	SimuClientInfo* pNewClientInfo = NULL;
	mapSimuClientInfoIterT   iterMapFind;
	SimuClientInfo* pFindClientInfo = NULL;

	if (NULL == pClient)
	{
		nFunRes = -1;
		return nFunRes;
	}  

	pNewClientInfo = new SimuClientInfo();
	pClient->getSimuClientInfo(*pNewClientInfo);
	pNewClientInfo->IsTcpHealth = false;
	pNewClientInfo->IsEnqHealth = false;
	pNewClientInfo->IsDeqHealth = false;
	strAgtName = pNewClientInfo->AgentName;

	iterMapFind = m_mapAllClientInfo.find(strAgtName);
	if (iterMapFind != m_mapAllClientInfo.end())
	{
		//find ok
		pFindClientInfo = iterMapFind->second;
		*pFindClientInfo = *pNewClientInfo;			
	}
	else
	{
		//not find
		m_mapAllClientInfo.insert(mapSimuClientInfoValueT(strAgtName, pNewClientInfo));
		pNewClientInfo = NULL;
	} 



	DEF_DELETE(pNewClientInfo);


	return nFunRes;
}


int CDbSynTestClientManager::_MonitorClientThreads()
{ 
	int nFunRes = 0;
	int nClientIndex = 0;
	std::string strClientAgtName;
	CPropagateMsg* pGetClient = NULL;

	mapQThreadPoolIterT iterDbSynTC;
	
	ThreadGuard guard(m_lockMapAllClientInfo);

	ThreadGuard guardData(m_lockVrtSynClientsThread); 


	iterDbSynTC = m_vrtSynClientsThread.begin();
	while (iterDbSynTC != m_vrtSynClientsThread.end())
	{
		pGetClient = (*iterDbSynTC).second;
		if (true == pGetClient->isFinishWork())
		{
			
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"[m_nClientIndex=%d] this client FinishWork, begin delete this client", 
				pGetClient->m_nClientIndex);  
			
			//before  terminateAndWait this client, get the last info into map
			_UpdateClientInfoToMap(pGetClient);

			pGetClient->terminateAndWait();

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"[m_nClientIndex=%d] this client FinishWork, end delete this client", 
				pGetClient->m_nClientIndex);  

			DEF_DELETE(pGetClient);

			m_vrtSynClientsThread.erase(iterDbSynTC);
			iterDbSynTC = m_vrtSynClientsThread.begin();

		}
		else
		{
			iterDbSynTC++;
		}
	}
	 
	return nFunRes;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

int	CDbSynTestClientManager::_PocessEManagerState()
{
	int nFunRes = 0;

	switch (m_nManagerWorkState)
	{
	case EManager_State_Begin_StartWork:
		_PocessEManagerStateBeginStartWork();
		break;
	case EManager_State_InitCfgParam:
		_PocessEManagerStateInitCfgParam();
		break;
	case EManager_State_InitNewClients:
		_PocessEManagerStateInitNewClients();
		break;
	case EManager_State_MonitorNewClients:
		_PocessEManagerStateMonitorNewClients();
		break;
	case EManager_State_CheckClientsWorkTime:
		_PocessEManagerStateCheckClientsWorkTime();
		break;
	case EManager_State_UnInitNewClients:
		_PocessEManagerStateUnInitNewClients();
		break;
	case EManager_State_End_FinishWork:
		_PocessEManagerStateEndFinishWork();
		break;
	default:
		break;		
	}


	return nFunRes;
}


int	CDbSynTestClientManager::_PocessEManagerStateBeginStartWork()
{
	int nFunRes = 0;
	
	m_nManagerWorkState = EManager_State_InitCfgParam;

	
	return nFunRes;
}
int	CDbSynTestClientManager::_PocessEManagerStateInitCfgParam()
{
	int nFunRes = 0;	

	if (g_CfgSimInfo.m_strConfigFileName.empty())
	{
		nFunRes = -1;
		return nFunRes;
	}
	_ConfigFileOpen(g_CfgSimInfo.m_strConfigFileName);
	_ProceeParam();
	
	m_mapClientIndexAgtName.clear();
	_getAllClientAgtNameFromRecipients(g_CfgSimInfo.m_strCfg_Recipients, m_mapClientIndexAgtName);
	g_SynProgramSimInfo.m_n_ClientCount =   m_mapClientIndexAgtName.size();

	g_system_variables.m_MsgCompress =   true; // for zip  

	DebugUtil::getInstance();
	DebugUtil::getInstance().setLevel((TA_Base_Core::DebugUtil::EDebugLevel)g_SynProgramSimInfo.m_DU_LogLevel);
	DebugUtil::getInstance().setFile(g_CfgSimInfo.m_strCfg_logfilename.c_str());
	DebugUtil::getInstance().setMaxSize(g_SynProgramSimInfo.m_n_logfilesize);
	DebugUtil::getInstance().setMaxFiles(g_SynProgramSimInfo.m_n_logfilecount);

	_LogPrintParam();
	nFunRes = _CheckCfgParam();
	if (0 != nFunRes)
	{
		m_nManagerWorkState = EManager_State_End_FinishWork;
		m_nWorkState = WorkState_End;
		nFunRes = -1;
		return nFunRes;
	}


	//_CreateClientAgtNameList();

	if (false == g_CfgSimInfo.m_strCfg_SrcDbConStr.empty())
	{
		_InitOracleDbTableParam();
	}

	//++Noel Notify GUI to setup client list
	CGUIUpdateHandler::getInstance().notifySetupClients(m_mapClientIndexAgtName);

	m_nManagerWorkState = EManager_State_InitNewClients;

	return nFunRes;

}


void  CDbSynTestClientManager::_ProcessInitialEnv()
{
	if (NULL == m_pObserver)
	{
		m_pObserver = new CMsgDispatcher();
	}

	

	int nRet = defRegisterObserver(m_pObserver);
	if ( Ret_Success != nRet )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Register_Observer_Failed, defErrMsg(Err_Register_Observer_Failed));
	}

}


//////////////////////////////////////////////////////////////////////////


void  CDbSynTestClientManager::_FreeClientThreadPool(mapQThreadPoolT& rMapPool)
{

	mapQThreadPoolIterT iterInner = rMapPool.begin();
	while (iterInner != rMapPool.end())
	{
		CPropagateMsg *& pThreadJob = iterInner->second;
		pThreadJob->terminateAndWait();

		DEF_DELETE(pThreadJob);

		iterInner++;
	}

	rMapPool.clear();


}

void  CDbSynTestClientManager::_FreeClinetInfoPool(mapQThreadT& rMapThread)
{  
	mapQThreadIterT iterInner = rMapThread.begin();
	while (iterInner != rMapThread.end())
	{
		PRO_THD*& pThd= iterInner->second;
		lstAgentContIterT iterThdRcv = pThd->m_lstRecver.begin();
		while (iterThdRcv != pThd->m_lstRecver.end())
		{
			DEF_DELETE(*iterThdRcv);
			iterThdRcv++;
		}

		pThd->m_lstRecver.clear();
		DEF_DELETE( pThd );

		iterInner++;
	}

	rMapThread.clear();  

}


void  CDbSynTestClientManager::_FreeMapSimuClientInfo(mapSimuClientInfoT& rMapSimuClientInfo)
{  
	SimuClientInfo* pGetInfo = NULL;
	mapSimuClientInfoIterT iterInner = rMapSimuClientInfo.begin();
	while (iterInner != rMapSimuClientInfo.end())
	{
		pGetInfo = iterInner->second;

		DEF_DELETE( pGetInfo );

		iterInner++;
	}

	rMapSimuClientInfo.clear();  

}

int	CDbSynTestClientManager::_InitAllClientInfo(mapQThreadT& vrtSynClientsInfo)
{
	int nFunRes = 0;
	int nClientIndex = 0;
	std::string strClientAgtName;
	mapIntStrConIterT    iterMap;
	std::string strServerAddress;

	
	strServerAddress = g_CfgSimInfo.m_strCfg_ServerIP + ":" + g_CfgSimInfo.m_strCfg_ServerPort;
	vrtSynClientsInfo.clear(); 
	if (NULL == m_pObserver)
	{
		nFunRes = -1;
		return nFunRes;
	}


	//
	iterMap = m_mapClientIndexAgtName.begin();
	while (iterMap != m_mapClientIndexAgtName.end())
	{
		nClientIndex = (*iterMap).first;
		strClientAgtName = (*iterMap).second;	 		

		CAgent* pserverAgt = new CAgent();// name="TRANSACT"	address= ("192.168.253.248:2005")
		PRO_THD* pThd = new PRO_THD();

		pThd->m_nClientIndex = nClientIndex;
		pThd->m_nPayloadType = MSGITEM_PYTP_OBJ;
		pThd->m_nSubID = 1;	
		
		//old serverID  "192.168.253.248:2005:AUDIT_DATA_QUEUE"
		//Test serverID  "192.168.253.248:2005:AUDIT_DATA_QUEUE:TRA_OCC"
		memset(pThd->m_szQName, 0, defMaxNameLen);
		memset(pThd->m_szQNameForServerID, 0, defMaxNameLen);

		sprintf(pThd->m_szQName,"%s", 
				g_CfgSimInfo.m_strCfg_QueueName.c_str());
				
		sprintf(pThd->m_szQNameForServerID,"%s:%s", 
			g_CfgSimInfo.m_strCfg_QueueName.c_str(),
			strClientAgtName.c_str());	
			
		//memcpy(pThd->m_szQName,  g_CfgSimInfo.m_strCfg_QueueName.c_str(), defMaxNameLen -1);
		
		//set sever info
		pserverAgt->setName("TRANSACT");
		pserverAgt->setAddress(strServerAddress);//("192.168.253.248:2005");
		pThd->m_lstRecver.push_back(pserverAgt);
		pserverAgt = NULL;
							
		pThd->m_Sender.setName(strClientAgtName);// = g_system_variables.m_LocalAgent;
	
		vrtSynClientsInfo.insert(mapQThreadValueT(nClientIndex, pThd));
		pThd = NULL;			   


		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"new one client info [ClientIndex=%d][strClientAgtName=%s]", 
			nClientIndex, strClientAgtName.c_str());  


		iterMap++;
	}//while
		

	return nFunRes;
}

void  CDbSynTestClientManager::_StartUpNewJobs(mapQThreadT& mapClientInfo, mapQThreadPoolT& mapClientThreadPool)
{
	uint32 uiSubID = 0;
	CPropagateMsg* pPropagateMsg = NULL;

	mapQThreadIterT iterOri = mapClientInfo.begin();
	while ( iterOri != mapClientInfo.end() )
	{
		uiSubID = iterOri->first;
		PRO_THD*& pProThd = iterOri->second;

		pPropagateMsg = new CPropagateMsg(m_pObserver);
		pPropagateMsg->m_nClientIndex = uiSubID;
		pPropagateMsg->setPropagateTHD(pProThd);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"start one thread: pPropagateMsg [uiSubID=%d][m_szQName=%s][ClientIndex=%d][strClientAgtName=%s]", 
			uiSubID, pProThd->m_szQName,
			uiSubID, pProThd->m_Sender.getName().c_str());
		pPropagateMsg->start();

		mapClientThreadPool.insert(mapQThreadPoolValueT(uiSubID, pPropagateMsg));

		iterOri++;
	}
}


int	CDbSynTestClientManager::_PocessEManagerStateInitNewClients()
{
	int nFunRes = 0;
	ThreadGuard guardData(m_lockVrtSynClientsThread); 

	_FreeClientThreadPool(m_vrtSynClientsThread);
	_FreeClinetInfoPool(m_vrtSynClientsInfo);

	_ProcessInitialEnv();
	_InitAllClientInfo(m_vrtSynClientsInfo);
	
	_StartUpNewJobs(m_vrtSynClientsInfo, m_vrtSynClientsThread);
		   

	m_nManagerWorkState = EManager_State_MonitorNewClients;

	return nFunRes;
}
int	CDbSynTestClientManager::_PocessEManagerStateMonitorNewClients()
{
	int nFunRes = 0;

	nFunRes = _MonitorClientThreads();

	//
	if (0 == m_vrtSynClientsThread.size())
	{
		m_nManagerWorkState = EManager_State_UnInitNewClients;
	}
	else
	{
		m_nManagerWorkState = EManager_State_CheckClientsWorkTime;
	}
	
	return nFunRes;
}
int	CDbSynTestClientManager::_PocessEManagerStateCheckClientsWorkTime()
{
	int nFunRes = 0;

	//
	m_strSimulatorWorkEnd = m_pTimeMonitor->getCurrentTime(&m_fSimulatorWorkEnd);
	m_nDiffBeginEnd = m_pTimeMonitor->getDiffTimeRes(&m_fSimulatorWorkBegin, &m_fSimulatorWorkEnd);
	//
	if (m_nDiffBeginEnd >= g_SynProgramSimInfo.m_n_TestDuration)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"[m_nDiffBeginEnd=%d]>=[m_n_TestDuration=%d] SynSimulator must stop work", 
			m_nDiffBeginEnd, g_SynProgramSimInfo.m_n_TestDuration);
			
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
			"Simulator Work End  [m_strSimulatorWorkEnd=%s]", 
			m_strSimulatorWorkEnd.c_str());
	 
			 
		m_nManagerWorkState = EManager_State_UnInitNewClients;
	}
	else
	{
		m_nManagerWorkState = EManager_State_MonitorNewClients;	
	}

	return nFunRes;
}
int	CDbSynTestClientManager::_PocessEManagerStateUnInitNewClients()
{
	int nFunRes = 0;
	ThreadGuard guardData(m_lockVrtSynClientsThread); 

	//the thread in m_vrtSynClientsThread till using data in m_vrtSynClientsInfo
	//so we must _FreeClientThreadPool(m_vrtSynClientsThread); first
	//_FreeClinetInfoPool(m_vrtSynClientsInfo);
	_FreeClientThreadPool(m_vrtSynClientsThread);
	_FreeClinetInfoPool(m_vrtSynClientsInfo);

	m_nManagerWorkState = EManager_State_End_FinishWork;

	return nFunRes;
}
int	CDbSynTestClientManager::_PocessEManagerStateEndFinishWork()
{
	int nFunRes = 0;

	m_nWorkState = WorkState_End;
	return nFunRes;
}


//////////////////////////////////////////////////////////////////////////
CfgSimInfo				g_CfgSimInfo;
SynProgramSimInfo		g_SynProgramSimInfo;


//////////////////////////////////////////////////////////////////////////

NS_END(TA_DB_Sync)


























