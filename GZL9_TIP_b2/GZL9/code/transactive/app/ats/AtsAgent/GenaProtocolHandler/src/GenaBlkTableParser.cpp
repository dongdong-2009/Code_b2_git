
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaBlkTableParser.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaAtsDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/BlkDataNotification.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/types/public_types/Constants.h"
#include "boost/algorithm/string.hpp"
#include <string>


namespace TA_IRS_App
{
	const char* BlockStatus [] = 
	{
		"OCCUPIED",		
		"UNOCCUPIED",	
		"DISTURBED",	
		"UNKNOWN"		
	};

	enum BlockStatusEnum {OCCUPIED = 0, UNOCCUPIED, BLKDISTURBED, BLKSTATUNKNOWN};
	#define BLOCKSTATLENGTH	4

	GenaBlkTableParser::GenaBlkTableParser()
	{
		FUNCTION_ENTRY("GenaBlkTableParser");
		FUNCTION_EXIT;
	}

	GenaBlkTableParser::~GenaBlkTableParser()
	{
		FUNCTION_ENTRY("~GenaBlkTableParser");
		FUNCTION_EXIT;
	}

	void GenaBlkTableParser::parseGenaAtsTable(TA_IRS_App::GenaRawTable& rawTable)
	{
		FUNCTION_ENTRY("parseGenaAtsTable");

		m_blkInfoVec.clear();
		if(rawTable.getTopicData() != NULL)
		{
			if(boost::to_upper_copy(rawTable.getTopicData()->getTopicName())=="BLOCK")
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Start parse Ats Table %s message", rawTable.getTopicData()->getUpdateType().c_str());
				importRecord(rawTable);
			}
		}
		
		//NOTE: If the Server Status Quality is BAD, it means there is no Data to be processed 
		// instead only DataPoints are need to be updated into Bad Quality State.

		//Check if received block Info data or data point bad quality message, 
		//and then send notification to related processor
		if((!m_blkInfoVec.empty())|| (rawTable.getDataPointQuality() == TA_Base_Core::ITA_QUALITY_BAD))
		{
			//Notify for Block Data Event
			TA_IRS_Bus::BlkDataNotificationPtr blkDataPtr(new TA_IRS_Bus::BlkDataNotification(&m_blkInfoVec));
			//Set the Data Quality
			blkDataPtr->setDataQuality(rawTable.getDataPointQuality());
			//Notify AtsData Event
			m_atsNotification->notifyAtsDataEvent(blkDataPtr);
		}

		FUNCTION_EXIT;
	}

	void GenaBlkTableParser::importRecord(TA_IRS_App::GenaRawTable& rawTable)
	{
		FUNCTION_ENTRY("importRecord");

		TA_Base_Bus::GENATopicDataPtr BlkData = rawTable.getTopicData();

		//Parse Block Information
		TA_IRS_Bus::BlkInfo blkInfo;
		//Initialize BlkInfoValue
		blkInfo.BlkStatus = BLKSTATUNKNOWN;
		blkInfo.isNeedUpdateBlkStatus = false;

		blkInfo.BlockId = BlkData->getObjectID();

		std::string tmpData;
		//Update value get from Gena client
		tmpData = BlkData->getAttributeValue(BLKSTATUS);
		if(tmpData.compare(""))
		{
			
			signed short blkStatus = convertStrToEmun(BlockStatus,tmpData, BLOCKSTATLENGTH);
	
			blkInfo.BlkStatus = blkStatus;
			blkInfo.isNeedUpdateBlkStatus = true;
		}
		
		m_blkInfoVec.push_back(blkInfo);

		FUNCTION_EXIT;
	} 

}