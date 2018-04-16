
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaPlatformDoorTableParser.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaAtsDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/PDDataNotification.h"
#include "bus/protocol/Gena/src/GENATopicData.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/types/public_types/Constants.h"
#include "boost/algorithm/string.hpp"
#include <string>

namespace TA_IRS_App
{

	const char* OverrideStatus [] = 
	{
		"ACTIVE",		
		"INACTIVE",	
		"FAILED",	
		"UNKNOWN"		
	};

	enum OverrideStatusEnum {ACTIVE = 0, INACTIVE, FAILED, UNKNOWN};
	#define OVERRIDESTATLENGTH	4

	GenaPlatformDoorTableParser::GenaPlatformDoorTableParser()
	{
		FUNCTION_ENTRY("GenaPlatformDoorTableParser");
		FUNCTION_EXIT;
	}

	GenaPlatformDoorTableParser::~GenaPlatformDoorTableParser()
	{
		FUNCTION_ENTRY("~GenaPlatformDoorTableParser");

		m_platformDoorInfoVec.clear();

		FUNCTION_EXIT;
	}

	void GenaPlatformDoorTableParser::parseGenaAtsTable(TA_IRS_App::GenaRawTable& rawTable)
	{
		FUNCTION_ENTRY("parseGenaAtsTable");

		m_platformDoorInfoVec.clear();
		if(rawTable.getTopicData() != NULL)
		{
			if(boost::to_upper_copy(rawTable.getTopicData()->getTopicName())=="PLATFORMDOOR")
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Start parse Ats Table %s message", rawTable.getTopicData()->getUpdateType().c_str());
				importRecord(rawTable);
			}
		}
		
		//NOTE: If the Server Status Quality is BAD, it means there is no Data to be processed 
		// instead only DataPoints are need to be updated into Bad Quality State.

		//Check if received Public Addressing Info data or data point bad quality message, 
		//and then send notification to related processor
		if((!m_platformDoorInfoVec.empty())|| (rawTable.getDataPointQuality() == TA_Base_Core::ITA_QUALITY_BAD))
		{
			//Notify for Pis Data Event
			TA_IRS_Bus::PDDataNotificationPtr platformDataPtr(new TA_IRS_Bus::PDDataNotification(&m_platformDoorInfoVec));
			//Set the Data Pis Quality
			platformDataPtr->setDataQuality(rawTable.getDataPointQuality());
			//Notify AtsData Event
			m_atsNotification->notifyAtsDataEvent(platformDataPtr);
		}
		FUNCTION_EXIT;
	}

	void GenaPlatformDoorTableParser::importRecord(TA_IRS_App::GenaRawTable& rawTable)
	{
		FUNCTION_ENTRY("importRecord");

		TA_Base_Bus::GENATopicDataPtr PlatformDoorData = rawTable.getTopicData();

		//Parse PlatformDoor Information
		TA_IRS_Bus::PlatformDoorInfo platformDoorInfo;
		//Initialize PlatformDoorInfo value
		constructEmptyInfo(platformDoorInfo);

		platformDoorInfo.PlatformDoorId = PlatformDoorData->getObjectID();

		std::string tmpData;
		//Update value get from Gena client
		tmpData = PlatformDoorData->getAttributeValue(OVERRIDE_STATUS);
		if(tmpData.compare(""))
		{
			
			signed short overrideStatus = convertStrToEmun(OverrideStatus,tmpData, OVERRIDESTATLENGTH);

			platformDoorInfo.OverrideStatus = overrideStatus;
			platformDoorInfo.isNeedUpdateOverride = true;
		}

		m_platformDoorInfoVec.push_back(platformDoorInfo);


		FUNCTION_EXIT;
	}

	void GenaPlatformDoorTableParser::constructEmptyInfo(TA_IRS_Bus::PlatformDoorInfo& platformDoorInfo)
	{
		FUNCTION_ENTRY("constructEmptyInfo");

		platformDoorInfo.PlatformDoorId = TA_IRS_Bus::EMPTY_VAL;
		platformDoorInfo.OverrideStatus = UNKNOWN;
		platformDoorInfo.isNeedUpdateOverride = false;

		FUNCTION_EXIT;
	}

}