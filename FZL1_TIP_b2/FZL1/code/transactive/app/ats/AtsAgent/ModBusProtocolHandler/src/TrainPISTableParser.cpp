/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: g:\depot\FZL1_TIP\FZL1\transactive\app\ats\AtsAgent\ModBusProtocolHandler\src\TrainPISTableParser.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for parsing PIS data from  raw data.
  */
#include "app/ats/AtsAgent/ModBusProtocolHandler/src/TrainPISTableParser.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/ats/AtsAgent/ModBusProtocolHandler/src/AtsConstants.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsHelper.h"
#include "bus/ats/ats_agent/CommonATS/src/TrainPISDataNotification.h"

namespace TA_IRS_App
{
	TrainPISTableParser::TrainPISTableParser(TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: BaseTableParser(EntityData),
		m_numberOfPlatforms(0)
	{
		FUNCTION_ENTRY("TrainPISTableParser");
		FUNCTION_EXIT;
	}

	TrainPISTableParser::~TrainPISTableParser()
	{
		FUNCTION_ENTRY("~TrainPISTableParser");
		FUNCTION_EXIT;
	}

	unsigned long TrainPISTableParser::getAtsModbusStartAddress()
	{
		FUNCTION_ENTRY("getAtsModbusStartAddress");
		FUNCTION_EXIT;
		return TRAIN_PIS_START_ADDRESS;
	}
	unsigned long TrainPISTableParser::getAtsModbusEndAddress()
	{
		FUNCTION_ENTRY("getAtsModbusEndAddress");
		FUNCTION_EXIT;
		return (getAtsModbusStartAddress() + getTotalNumberOfPlatform() - 1);
	}

	unsigned long TrainPISTableParser::getTotalNumberOfPlatform()
	{
		FUNCTION_ENTRY("getNumberOfPlatform");
		FUNCTION_EXIT;
		return (TRAIN_PIS_RECORD_SIZE * m_numberOfPlatforms);
	}

	void TrainPISTableParser::parseAtsTable(TA_IRS_Bus::RawTable& rawTable)
	{
		FUNCTION_ENTRY("importAtsTable");

		//Get the Number of Platforms
		m_numberOfPlatforms = rawTable.getWord(NUMBER_OF_PLATFORM_POS * MODBUS_BYTE_SIZE);

		//Data will not be processed if the number of platforms is 0.
		if (m_numberOfPlatforms == 0)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Number of Platforms: %d, Data will not be processed.",m_numberOfPlatforms);
			return;
		}

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Number of Platforms: %d",m_numberOfPlatforms);
		//Read PIS Table
		TA_IRS_Bus::RawTable paRawTable(rawTable,getAtsModbusStartAddress(),getAtsModbusEndAddress());
		readPISTable(paRawTable);

		FUNCTION_EXIT;
	}

	void TrainPISTableParser::readPISTable(TA_IRS_Bus::RawTable& rawTable)
	{
		FUNCTION_ENTRY("readPATable");

		//Displays log for raw data information
		TA_IRS_Bus::AtsHelper::printRawData(rawTable.getDataBlock(),"TrainPISTableParser");

		int recordOffset = 0;
		m_pisInfoVec.clear();

		unsigned long platformId = 0;
		for (int recordOffset=0; recordOffset < rawTable.length();
			recordOffset += TRAIN_PIS_RECORD_SIZE)
		{
			platformId++;
			try
			{	//Parse PIS Information
				importRecord(rawTable,recordOffset*2,platformId);
			}
			catch (...)
			{
				std::ostringstream text;
				text << "readPISTable() - Platform Record with Platform ID ";
				text << (unsigned long)platformId;
				text << " not found";

				LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", text.str());
			}
		}

		//Notify for Train PIS Data Event
		TA_IRS_Bus::TrainPISDataNotificationPtr pisDataPtr(new TA_IRS_Bus::TrainPISDataNotification(&m_pisInfoVec));

		//Set the Data Block Quality
		pisDataPtr->setDataQuality(rawTable.getDatablockQuality());

		//Notify AtsData Event
		m_atsNotification->notifyAtsDataEvent(pisDataPtr);

		FUNCTION_EXIT;
	}

	void TrainPISTableParser::importRecord(TA_IRS_Bus::RawTable& rawTable, unsigned int recordByteOffset, TA_IRS_Bus::PlatformIdType platformId)
	{
		FUNCTION_ENTRY("importRecord");

		//Parse PIS Information
		TA_IRS_Bus::PISInfo paInfo;

		//Get the 1st Train Arrival Time
		paInfo.Train1st_ArrivalTime = rawTable.getDWord(recordByteOffset);
		paInfo.PlatformId = platformId;
		m_pisInfoVec.push_back(paInfo);

		FUNCTION_EXIT;
	}
}