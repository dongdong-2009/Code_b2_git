
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaPlatformTableParser.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaAtsDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/PlatformDataNotification.h"
#include "bus/protocol/Gena/src/GENATopicData.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/types/public_types/Constants.h"
#include "boost/algorithm/string.hpp"
#include <string>

namespace TA_IRS_App
{
	const char * HoldStatus[] = 
	{
		"NOT HOLD",		
		"PERSISTENT HOLD"
	};
	enum HoldStatEnum {NOTHOLD = 0, HOLD, HOLDUNKNOWN};
	#define HOLDSTATLENGTH	2

	const char * OpenStatus[] = 
	{
		"OPEN",		
		"CLOSED"
	};
	enum OpenStatEnum {OPEN = 0, CLOSE, OPENSTATUNKOWN};
	#define OPENSTATLENGTH	2

	GenaPlatformTableParser::GenaPlatformTableParser()
	{
		FUNCTION_ENTRY("GenaPlatformTableParser");
		FUNCTION_EXIT;
	}

	GenaPlatformTableParser::~GenaPlatformTableParser()
	{
		FUNCTION_ENTRY("~GenaPlatformTableParser");

		m_platformInfoVec.clear();

		FUNCTION_EXIT;
	}

	void GenaPlatformTableParser::parseGenaAtsTable(TA_IRS_App::GenaRawTable& rawTable)
	{
		FUNCTION_ENTRY("parseGenaAtsTable");

		m_platformInfoVec.clear();

		if(rawTable.getTopicData() != NULL)
		{
			if(boost::to_upper_copy(rawTable.getTopicData()->getTopicName())=="PLATFORM")
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Start parse Ats Table %s message", rawTable.getTopicData()->getUpdateType().c_str());
				importRecord(rawTable);
			}
		}

		//NOTE: If the Server Status Quality is BAD, it means there is no Data to be processed 
		// instead only DataPoints are need to be updated into Bad Quality State.

		//Check if received Public Addressing Info data or data point bad quality message, 
		//and then send notification to related processor
		if((!m_platformInfoVec.empty())|| (rawTable.getDataPointQuality() == TA_Base_Core::ITA_QUALITY_BAD))
		{
			//Notify for Pis Data Event
			TA_IRS_Bus::PlatformDataNotificationPtr platformDataPtr(new TA_IRS_Bus::PlatformDataNotification(&m_platformInfoVec));
			//Set the Data Pis Quality
			platformDataPtr->setDataQuality(rawTable.getDataPointQuality());
			//Notify AtsData Event
			m_atsNotification->notifyAtsDataEvent(platformDataPtr);
		}
		FUNCTION_EXIT;
	}

	void GenaPlatformTableParser::importRecord(TA_IRS_App::GenaRawTable& rawTable)
	{
		FUNCTION_ENTRY("importRecord");

		TA_Base_Bus::GENATopicDataPtr PlatformData = rawTable.getTopicData();

		//Parse Platform Information
		TA_IRS_Bus::PlatformInfo platformInfo;
		constructEmptyPisInfo(platformInfo);

		platformInfo.PlatformId = PlatformData->getObjectID();
		
		std::string tmpData;
		//Update value get from Gena client
		//Hold Status
		tmpData.clear();
		tmpData = PlatformData->getAttributeValue(HOLDSTATUS);
		if(tmpData.compare(""))
		{
			
			signed short holdStatus = convertStrToEmun(HoldStatus,tmpData, HOLDSTATLENGTH);
			if(holdStatus != -1)
			{
				platformInfo.hold = holdStatus;
			}
			else
			{
				platformInfo.hold = HOLDUNKNOWN;
			}
		
			platformInfo.isNeedUpdateHold = true;
		}

		//Skip Status
		tmpData.clear();
		tmpData = PlatformData->getAttributeValue(OPENSTATUS);
		if(tmpData.compare(""))
		{
			signed short openStatus = convertStrToEmun(OpenStatus,tmpData, OPENSTATLENGTH);
			if(openStatus != -1)
			{
				platformInfo.Skip = openStatus;
			}
			else
			{
				platformInfo.Skip = OPENSTATUNKOWN;
			}
			platformInfo.isNeedUpdateSkip = true;
		}

		m_platformInfoVec.push_back(platformInfo);

		FUNCTION_EXIT;
	}

	void GenaPlatformTableParser::constructEmptyPisInfo(TA_IRS_Bus::PlatformInfo& platformInfo)
	{
		platformInfo.PlatformId = TA_IRS_Bus::EMPTY_VAL;;
		platformInfo.Skip = OPENSTATUNKOWN;
		platformInfo.hold = HOLDUNKNOWN;
		platformInfo.isNeedUpdateHold = false;
		platformInfo.isNeedUpdateSkip = false;
	}  

}