
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaSwitchTableParser.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaAtsDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/SwitchDataNotification.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/types/public_types/Constants.h"
#include "boost/algorithm/string.hpp"

namespace TA_IRS_App
{
	const char * SwtichPosition[] = 
	{
		"LEFT",		
		"RIGHT",	
		"OUT OF CORRESPONDENCE"	
	};
	enum SwitchPosEnum {LEFT = 0, RIGHT, OUTOFCORRESPONDENCE};
	#define POSITIONLENGTH	3

	const char * SwtichFailedStatus[] = 
	{
		"OK",			
		"DISTURBED"		
	};
	enum SwtichFailedStatusEnum {OK = 0, SWITCHDISTURBED, FAILEDUNKNOWN};
	#define FAILEDSTATLENGTH	2

	const char * SwtichResStatus[] = 
	{
		"RESERVED",		
		"NOTRESERVED"	
	};
	enum SwtichResStatusEnum {RESERVED = 0, NOTRESERVED, RESUNKNOWN};
	#define RESSTATLENGTH	2

	GenaSwitchTableParser::GenaSwitchTableParser()
	{
		FUNCTION_ENTRY("GenaSwitchTableParser");
		FUNCTION_EXIT;
	}

	GenaSwitchTableParser::~GenaSwitchTableParser()
	{
		FUNCTION_ENTRY("~GenaSwitchTableParser");
		FUNCTION_EXIT;
	}

	void GenaSwitchTableParser::parseGenaAtsTable(TA_IRS_App::GenaRawTable& rawTable)
	{
		FUNCTION_ENTRY("parseAtsTable");

		m_switchInfoVec.clear();

		if(rawTable.getTopicData()!= NULL)
		{
			if(boost::to_upper_copy(rawTable.getTopicData()->getTopicName())=="SWITCH")
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Start parse Ats Table %s message", rawTable.getTopicData()->getUpdateType().c_str());
				importRecord(rawTable);
			}
		}
		//NOTE: If the Server Status Quality is BAD, it means there is no Data to be processed 
		// instead only DataPoints are need to be updated into Bad Quality State.

		//Check if received Switch Info data or data point bad quality message, 
		//and then send notification to related processor
		if((!m_switchInfoVec.empty())|| (rawTable.getDataPointQuality() == TA_Base_Core::ITA_QUALITY_BAD))
		{
			//Notify for Switch Data Event
			TA_IRS_Bus::SwitchDataNotificationPtr switchDataPtr(new TA_IRS_Bus::SwitchDataNotification(&m_switchInfoVec));
			//Set Switch Info Data Quality
			switchDataPtr->setDataQuality(rawTable.getDataPointQuality());
			//Notify AtsData Event
			m_atsNotification->notifyAtsDataEvent(switchDataPtr);
		}

		FUNCTION_EXIT;
	}

	void GenaSwitchTableParser::importRecord(TA_IRS_App::GenaRawTable& rawTable)
	{
		FUNCTION_ENTRY("importRecord");

		TA_Base_Bus::GENATopicDataPtr switchData = rawTable.getTopicData();

		//Parse Switch Information
		TA_IRS_Bus::SwtichInfo switchInfo;
		//Initialize Switch Info
		switchInfo.SwitchId = switchData->getObjectID();
		switchInfo.isNeedUpdateFailed = false;
		switchInfo.isNeedUpdatePos = false;
		switchInfo.isNeedUpdateRes = false;

		//Update value get from Gena client
		std::string tmpData;
		//Position
		tmpData = switchData->getAttributeValue(SWPOSITION);
		if(tmpData.compare(""))
		{
			
			signed short switchPosition = convertStrToEmun(SwtichPosition,tmpData, POSITIONLENGTH);

			switchInfo.Position = switchPosition;
			switchInfo.isNeedUpdatePos = true;
		}
		//Failed Status
		tmpData.clear();
		tmpData = switchData->getAttributeValue(FAILEDSTATUS);
		if(tmpData.compare(""))
		{
			
			signed short failedStatus = convertStrToEmun(SwtichFailedStatus,tmpData, FAILEDSTATLENGTH);
			if(failedStatus != -1)
			{
				switchInfo.FailedStatus = failedStatus;
			}
			else
			{
				switchInfo.FailedStatus = FAILEDUNKNOWN;
			}
			switchInfo.isNeedUpdateFailed = true;
		}
		//Reserved Status
		tmpData.clear();
		tmpData = switchData->getAttributeValue(RESSTATUS);
		if(tmpData.compare(""))
		{
			
			signed short resStatus = convertStrToEmun(SwtichResStatus,tmpData, RESSTATLENGTH);
			if(resStatus != -1)
			{
				switchInfo.ResStatus = resStatus;
			}
			else
			{
				switchInfo.ResStatus = RESUNKNOWN;
			}
			switchInfo.isNeedUpdateRes = true;
		}

		m_switchInfoVec.push_back(switchInfo);

		FUNCTION_EXIT;
	}

}