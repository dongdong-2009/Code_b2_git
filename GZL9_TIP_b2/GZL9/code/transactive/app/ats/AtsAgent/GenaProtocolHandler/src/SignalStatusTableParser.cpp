
#include "app/ats/AtsAgent/GenaProtocolHandler/src/SignalStatusTableParser.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaAtsDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/SignalDataNotification.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/types/public_types/Constants.h"
#include "boost/algorithm/string.hpp"

namespace TA_IRS_App
{
	const char * SignalStatus[] = { "UNKNOWN" , "RED" , "GREEN", "YELLOW", "RED_AND_YELLOW" };
	enum SigStatusEnum { SIGDISPLYUNKNOWN = 0, RED , GREEN, YELLOW, RED_AND_YELLOW};
	#define SIGSTATUSLENGTH 5

	const char * HealthStatus[] = { "HEALTH" , "FAILED" , "UNKNOWN"}; 
	enum HealthStatusEnum { HEALTH = 0, FAILED, SIGHEALTHUNKNOWN};
	#define HEALTHSTATUSLENGTH	3
	
	SignalStatusTableParser::SignalStatusTableParser()
	{
		FUNCTION_ENTRY("SignalStatusTableParser");
		FUNCTION_EXIT;
	}

	SignalStatusTableParser::~SignalStatusTableParser()
	{
		FUNCTION_ENTRY("~SignalStatusTableParser");
		FUNCTION_EXIT;
	}

	void SignalStatusTableParser::parseGenaAtsTable(TA_IRS_App::GenaRawTable& rawTable)
	{
		FUNCTION_ENTRY("importAtsTable");

		m_signalInfoVec.clear();
		if(rawTable.getTopicData()!= NULL)
		{
			if(boost::to_upper_copy(rawTable.getTopicData()->getTopicName())=="SIGNAL")
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Start parse Ats Table %s message", rawTable.getTopicData()->getUpdateType().c_str());
				importRecord(rawTable);
			}
		}
		//NOTE: If the Server Status Quality is BAD, it means there is no Data to be processed 
		// instead only DataPoints are need to be updated into Bad Quality State.

		//Check if received Signal Info data or data point bad quality message, 
		//and then send notification to related processor
		if((!m_signalInfoVec.empty())|| (rawTable.getDataPointQuality() == TA_Base_Core::ITA_QUALITY_BAD))
		{
			//Notify for Signal Data Event
			TA_IRS_Bus::SignalDataNotificationPtr sigDataPtr(new TA_IRS_Bus::SignalDataNotification(&m_signalInfoVec));
			sigDataPtr->setDataQuality(rawTable.getDataPointQuality());
			m_atsNotification->notifyAtsDataEvent(sigDataPtr);
		}
		
		FUNCTION_EXIT;
	}

	void SignalStatusTableParser::importRecord(TA_IRS_App::GenaRawTable& rawTable)
	{
		FUNCTION_ENTRY("importRecord");

		TA_Base_Bus::GENATopicDataPtr signalStatusData = rawTable.getTopicData();

		//Parse Signal Information
		TA_IRS_Bus::SignalInfo signalInfo;

		constructEmptySigInfo(signalInfo);

		signalInfo.SignalId = signalStatusData->getObjectID();

		std::string tmpData;
		//Signal Display Status
		tmpData.clear();
		tmpData = signalStatusData->getAttributeValue(STATUS);
		if(tmpData.compare(""))
		{				
			signed short displayStatus = convertStrToEmun(SignalStatus, tmpData, SIGSTATUSLENGTH);

			signalInfo.Status = displayStatus;
			signalInfo.isNeedUpdateStatus = true;
		}
		//Signal Health Status
		tmpData.clear();
		tmpData = signalStatusData->getAttributeValue(HEALTHSTATUS);
		if(tmpData.compare(""))
		{				
			signed short healthStatus = convertStrToEmun(HealthStatus, tmpData, HEALTHSTATUSLENGTH);
			signalInfo.healthStatus = healthStatus;
			signalInfo.isNeedUpdateMode = true;
		}

		m_signalInfoVec.push_back(signalInfo);

		FUNCTION_EXIT;
	}

	void SignalStatusTableParser::constructEmptySigInfo(TA_IRS_Bus::SignalInfo& sigInfo)
	{
		FUNCTION_ENTRY("constructEmptySigInfo");

		sigInfo.SignalId =	TA_IRS_Bus::EMPTY_VAL;
		sigInfo.Status =	SIGDISPLYUNKNOWN;
		sigInfo.healthStatus = SIGHEALTHUNKNOWN;
		sigInfo.isNeedUpdateStatus = false;
		sigInfo.isNeedUpdateMode = false;

		FUNCTION_EXIT;
	}

}