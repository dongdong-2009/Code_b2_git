#include "SynSimuCliientInfo.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"


NS_BEGIN(TA_DB_Sync)

SimuClientInfo::SimuClientInfo()
{
	
	AgentName.clear();
	LastEnqSeqID = 0;
	LastDeqSeqID = 0;
	Status = Normal;
	totalDeq = 0;
	totalEnq = 0;
	IsTcpHealth = false;
	IsEnqHealth = false;
	IsDeqHealth = false;

}

SimuClientInfo::~SimuClientInfo()
{
}

SimuClientInfo& SimuClientInfo::operator=(const SimuClientInfo& infoSrc)
{
	this->AgentName = infoSrc.AgentName;
	this->LastEnqSeqID = infoSrc.LastEnqSeqID;
	this->LastDeqSeqID = infoSrc.LastDeqSeqID;
	this->totalDeq = infoSrc.totalDeq;
	this->totalEnq = infoSrc.totalEnq;
	this->Status = infoSrc.Status;
	this->IsTcpHealth = infoSrc.IsTcpHealth;
	this->IsEnqHealth = infoSrc.IsEnqHealth; 		
	this->IsDeqHealth = infoSrc.IsDeqHealth;
	return *this;
} 

std::string SimuClientInfo::getStatus()
{
	std::string strStatusForlog;

	switch (Status)
	{
	case Normal:
		strStatusForlog = "Normal";
		break;
	case Pause:
		strStatusForlog = "Pause";
		break;
	case Error:
		strStatusForlog = "Error";
		break;
	case Finished:
		strStatusForlog = "Finished";
		break;
	default:
		strStatusForlog = "UNKNOWN";  
		break;	
	}
	return strStatusForlog;
}
int SimuClientInfo::logSimuClientInfo()
{
	int nFunRes = 0;
	std::string strStatusForlog;
	std::string strIsTcpHealthForlog;
	std::string strIsEnqHealthForlog;
	std::string strIsDeqHealthForlog;

	strStatusForlog = getStatus();

	if (IsTcpHealth)
	{
		strIsTcpHealthForlog = "OK";
	}
	else
	{
		strIsTcpHealthForlog = "NOTOK";
	}

	if (IsEnqHealth)
	{
		strIsEnqHealthForlog = "OK";
	}
	else
	{
		strIsEnqHealthForlog = "NOTOK";
	}

	if (IsDeqHealth)
	{
		strIsDeqHealthForlog = "OK";
	}
	else
	{
		strIsDeqHealthForlog = "NOTOK";
	}


	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[AgentName=%s][LastEnqSeqID=%llu][LastDeqSeqID=%llu][totalEnq=%llu][totalDeq=%llu][Status=%s][IsTcpHealth=%s][IsEnqHealth=%s][IsDeqHealth=%s]", 
		AgentName.c_str(), LastEnqSeqID, LastDeqSeqID, totalEnq, totalDeq, strStatusForlog.c_str(), 
		strIsTcpHealthForlog.c_str(), strIsEnqHealthForlog.c_str(), strIsDeqHealthForlog.c_str());


	return nFunRes;
}

NS_END(TA_DB_Sync)