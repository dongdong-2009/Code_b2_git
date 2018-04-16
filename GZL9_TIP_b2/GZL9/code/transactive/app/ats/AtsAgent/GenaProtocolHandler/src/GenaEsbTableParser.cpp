
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaEsbTableParser.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaAtsDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/EsbDataNotification.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/types/public_types/Constants.h"
#include "boost/algorithm/string.hpp"
#include <string>


namespace TA_IRS_App
{
	const char * EsbStatus [] =
	{
		"ESB ACTIVE",		
		"ESB INACTIVE",		
		"UNKNOWN",			
	};
	enum EsbStatusEnum {ACTIVE = 0, INACTIVE, ESBUNKNOWN};
	#define ESBSTATLENGTH	3

	GenaEsbTableParser::GenaEsbTableParser()
	{
		FUNCTION_ENTRY("GenaEsbTableParser");
		FUNCTION_EXIT;
	}

	GenaEsbTableParser::~GenaEsbTableParser()
	{
		FUNCTION_ENTRY("~GenaEsbTableParser");
		FUNCTION_EXIT;
	}

	void GenaEsbTableParser::parseGenaAtsTable(TA_IRS_App::GenaRawTable& rawTable)
	{
		FUNCTION_ENTRY("parseGenaAtsTable"); 

		m_esbInfoVec.clear();
		if(rawTable.getTopicData() != NULL)
		{
			if(boost::to_upper_copy(rawTable.getTopicData()->getTopicName())=="EMERGENCYSTOPBUTTON")
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Start parse Ats Table %s message", rawTable.getTopicData()->getUpdateType().c_str());
				importRecord(rawTable);
			}
		}
		
		//NOTE: If the Server Status Quality is BAD, it means there is no Data to be processed 
		// instead only DataPoints are need to be updated into Bad Quality State.

		//Check if received Emergency Stop Button Info data or data point bad quality message, 
		//and then send notification to related processor
		if((!m_esbInfoVec.empty())|| (rawTable.getDataPointQuality() == TA_Base_Core::ITA_QUALITY_BAD))
		{
			//Notify for Emergency Stop Button Data Event
			TA_IRS_Bus::EsbDataNotificationPtr esbDataPtr(new TA_IRS_Bus::EsbDataNotification(&m_esbInfoVec));
			//Set Emergency Stop Button Data Quality
			esbDataPtr->setDataQuality(rawTable.getDataPointQuality());
			//Notify AtsData Event
			m_atsNotification->notifyAtsDataEvent(esbDataPtr);
		}

		FUNCTION_EXIT;
	}

	void GenaEsbTableParser::importRecord(TA_IRS_App::GenaRawTable& rawTable)
	{
		FUNCTION_ENTRY("importRecord");

		TA_Base_Bus::GENATopicDataPtr EsbData = rawTable.getTopicData();

		//Parse Emergency Stop Button Information
		TA_IRS_Bus::EsbInfo esbInfo;
		//Initialize Emergency Stop Button Value
		esbInfo.Status = ESBUNKNOWN;
		esbInfo.isNeedUpdateEsbStatus = false;
		esbInfo.EsbId = EsbData->getObjectID();
		
		std::string tmpData;
		tmpData.clear();
		//Update value get from Gena client
		tmpData = EsbData->getAttributeValue(ESBSTATUS);
		if(tmpData.compare(""))
		{			
			signed short esbStatus = convertStrToEmun(EsbStatus,tmpData, ESBSTATLENGTH);

			esbInfo.Status = esbStatus;
			esbInfo.isNeedUpdateEsbStatus = true;
		}

		m_esbInfoVec.push_back(esbInfo);

		FUNCTION_EXIT;
	} 

}