/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\app\ats\AtsAgent\ModBusProtocolHandler\src\PIDSTableParser.cpp $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for parsing PIDS data from a raw data.
  */

#include "app/ats/AtsAgent/ModBusProtocolHandler/src/PIDSTableParser.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/ats/AtsAgent/ModBusProtocolHandler/src/AtsConstants.h"
#include "bus/ats/ats_agent/CommonATS/src/PIDSDataNotification.h"

using namespace TA_IRS_Bus;
namespace TA_IRS_App
{
	PIDSTableParser::PIDSTableParser(TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: BaseTableParser(EntityData)
	{
		FUNCTION_ENTRY("PIDSTableParser");
		FUNCTION_EXIT;
	}

	PIDSTableParser::~PIDSTableParser()
	{
		FUNCTION_ENTRY("~PIDSTableParser");
		FUNCTION_EXIT;
	}

	void PIDSTableParser::parseAtsTable(TA_IRS_Bus::RawTable& rawTable)
	{
		FUNCTION_ENTRY("parseAtsTable");

		//Check the number of platforms.
		int numberOfPlatforms = rawTable.getWord(NUMBER_OF_PLATFORM_POS * MODBUS_BYTE_SIZE);

		//If the number of platform is zero, the raw data will not be forwarded to PIDS.
		if (numberOfPlatforms == 0)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Data will not be forwarded to PIDS due to number of Platforms is %d",numberOfPlatforms);
			return;
		}

		//Notify for PIDS Data Event
		TA_IRS_Bus::PIDSDataNotificationPtr pidsData(new TA_IRS_Bus::PIDSDataNotification(rawTable));

		//Set the Data Block Quality
		pidsData->setDataQuality(rawTable.getDatablockQuality());

		//Notify AtsData Event
		m_atsNotification->notifyAtsDataEvent(pidsData);

		FUNCTION_EXIT;
	}
}