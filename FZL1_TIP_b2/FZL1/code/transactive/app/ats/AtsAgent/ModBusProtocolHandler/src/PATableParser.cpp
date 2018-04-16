/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: g:\depot\FZL1_TIP\FZL1\transactive\app\ats\AtsAgent\ModBusProtocolHandler\src\PATableParser.cpp $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for parsing PA data from a raw data.
  */

#include "app/ats/AtsAgent/ModBusProtocolHandler/src/PATableParser.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/ats/ats_agent/CommonATS/src/DataCache.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsHelper.h"
#include "bus/ats/ats_agent/CommonATS/src/PADataNotification.h"
#include "app/ats/AtsAgent/ModBusProtocolHandler/src/AtsConstants.h"

namespace TA_IRS_App
{
	PATableParser::PATableParser(TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: BaseTableParser(EntityData),
		  m_numberOfPlatforms(0)
	{
		FUNCTION_ENTRY("PATableParser");
		FUNCTION_EXIT;
	}

	PATableParser::~PATableParser()
	{
		FUNCTION_ENTRY("~PATableParser");
		FUNCTION_EXIT;
	}

	unsigned long PATableParser::getAtsModbusStartAddress()
	{
		FUNCTION_ENTRY("getAtsModbusStartAddress");
		FUNCTION_EXIT;
		return PLATFORM_PA_START_ADDRESS_POS;
	}
	unsigned long PATableParser::getAtsModbusEndAddress()
	{
		FUNCTION_ENTRY("getAtsModbusEndAddress");
		FUNCTION_EXIT;
		return (getAtsModbusStartAddress() + getTotalNumberOfPlatform() - 1);
	}

	unsigned long PATableParser::getTotalNumberOfPlatform()
	{
		FUNCTION_ENTRY("getNumberOfPlatform");
		FUNCTION_EXIT;
		return (PLATFORM_PA_RECORD_SIZE * m_numberOfPlatforms);
	}


	void PATableParser::parseAtsTable(TA_IRS_Bus::RawTable& rawTable)
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
		//Read Broadcast PA Table
		TA_IRS_Bus::RawTable paRawTable(rawTable,getAtsModbusStartAddress(),getAtsModbusEndAddress());
		readPATable(paRawTable);
		
		FUNCTION_EXIT;
	}

	void PATableParser::readPATable(TA_IRS_Bus::RawTable& rawTable)
	{
		FUNCTION_ENTRY("readPATable");

		//Displays log for raw data information
		TA_IRS_Bus::AtsHelper::printRawData(rawTable.getDataBlock(),"PATableParser");

		int recordOffset = 0;
		m_paStationInfoVec.clear();

		TA_IRS_Bus::PlatformIdType platformId = 0;
		for (int recordOffset=0; recordOffset < rawTable.length();
			 recordOffset += PLATFORM_PA_RECORD_SIZE)
		{
			platformId++;
			try
			{	//Parse PA Information
				importRecord(rawTable,recordOffset*2,platformId);
			}
			catch (...)
			{
				std::ostringstream text;
				text << "readPATable() - Platform Record with Platform ID ";
				text << (unsigned long)platformId;
				text << " not found";

				LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", text.str());
			}
		}

		//Notify for PA Data Event
		TA_IRS_Bus::PADataNotificationPtr paDataPtr(new TA_IRS_Bus::PADataNotification(&m_paStationInfoVec));

		//Set the Data Block Quality
		paDataPtr->setDataQuality(rawTable.getDatablockQuality());

		//Notify AtsData Event
		m_atsNotification->notifyAtsDataEvent(paDataPtr);

		FUNCTION_EXIT;
	}

	void PATableParser::importRecord(TA_IRS_Bus::RawTable& rawTable, unsigned int recordByteOffset, TA_IRS_Bus::PlatformIdType platformId)
	{
		FUNCTION_ENTRY("importRecord");

		//Parse PA Information
		TA_IRS_Bus::PAStationInfo paInfo;
		paInfo.PlatformId           = platformId;
		paInfo.DestinationStationId = rawTable.getByte(recordByteOffset + TRAIN_DEST_POS);

		//Get Arrival Status
		unsigned char arrivalStatus  = rawTable.getByte(recordByteOffset);
		unsigned char statusLowBits  = arrivalStatus & 0x0f;
		unsigned char statusHighBits = (arrivalStatus >> 4) & 0x0f;

		paInfo.Skip                 = (statusLowBits == 1);				             //Skip Value
		paInfo.InDepot              = ((statusLowBits == 2)||(statusLowBits == 3));  //InDepot Value				  
	    paInfo.PreArrival           = (statusHighBits == 1);						 //PreArrival Value
		paInfo.Arrival              = ((statusHighBits == 2)||(statusHighBits==3));	 //Arrival Value
		m_paStationInfoVec.push_back(paInfo);

		FUNCTION_EXIT;
	}

}