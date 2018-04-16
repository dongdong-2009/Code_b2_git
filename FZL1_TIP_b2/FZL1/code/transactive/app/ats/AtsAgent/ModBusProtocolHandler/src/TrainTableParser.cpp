/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\app\ats\AtsAgent\ModBusProtocolHandler\src\TrainTableParser.cpp $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for parsing Train data from a raw data.
  */

#include "app/ats/AtsAgent/ModBusProtocolHandler/src/TrainTableParser.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsHelper.h"
#include "bus/ats/ats_agent/CommonATS/src/TrainDataNotification.h"
#include "app/ats/AtsAgent/ModBusProtocolHandler/src/AtsConstants.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsHelper.h"

namespace TA_IRS_App
{
	TrainTableParser::TrainTableParser(TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: BaseTableParser(EntityData),
		  m_numberOfTrains(0)
	{
		FUNCTION_ENTRY("TrainTableParser");
		FUNCTION_EXIT;
	}

	TrainTableParser::~TrainTableParser()
	{
		FUNCTION_ENTRY("~TrainTableParser");
		FUNCTION_EXIT;
	}

	void TrainTableParser::parseAtsTable(TA_IRS_Bus::RawTable& rawTable)
	{
		FUNCTION_ENTRY("parseAtsTable");

		//Get the Number of Trains
		m_numberOfTrains = rawTable.getWord(NUMBER_OF_TRAIN_POS * MODBUS_BYTE_SIZE);

		bool _dpQuality = TA_IRS_Bus::AtsHelper::isGoodDatapointQuality(rawTable.getDatablockQuality());

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Data Block Quality: %s, Number of Trains: %d",
				_dpQuality?"GOOD":"BAD", m_numberOfTrains);

		//Read Train Table
		TA_IRS_Bus::RawTable trainRawTable(rawTable,getAtsModbusStartAddress(),getAtsModbusEndAddress());
		readTrainTable(trainRawTable);
		
		FUNCTION_EXIT;
	}

	unsigned long TrainTableParser::getAtsModbusStartAddress()
	{
		FUNCTION_ENTRY("getAtsModbusStartAddress");
		FUNCTION_EXIT;
		return TRAIN_START_ADDRESS_POS;
	}
	unsigned long TrainTableParser::getAtsModbusEndAddress()
	{
		FUNCTION_ENTRY("getAtsModbusEndAddress");
		FUNCTION_EXIT;
		//Computation to get the Total End Address based on the number of Trains
		// Condition:
		// 1. If the Train number != 0
		//    Start Address + No. of trains - 1
		// 2. If the Train number == 0
        //    Start Address + No. of trains or simply Start Address
		int _sub = ( getTotalNumberOfTrains() != 0)?1:0;
		return (getAtsModbusStartAddress() + getTotalNumberOfTrains() - _sub);
	}

	unsigned long TrainTableParser::getTotalNumberOfTrains()
	{
		FUNCTION_ENTRY("getTotalNumberOfTrains");
		FUNCTION_EXIT;
		return (TRAIN_RECORD_SIZE * m_numberOfTrains);
	}

	void TrainTableParser::readTrainTable(TA_IRS_Bus::RawTable& rawTable)
	{
		FUNCTION_ENTRY("readTrainTable");

		//Displays log for raw data information
		TA_IRS_Bus::AtsHelper::printRawData(rawTable.getDataBlock(),"TrainTableParser");

		int recordOffset = 0;
		bool lastTrainRecord = false;
		m_trainInfoVec.clear();

		//Check if the Data Block quality is Good
		//If so, import the Data Block and parse the the Train Data
		if (TA_IRS_Bus::AtsHelper::isGoodDatapointQuality(rawTable.getDatablockQuality()) &&  m_numberOfTrains>0)
		{
			unsigned short trainCounts = 0;
			for (int recordOffset=0; recordOffset < rawTable.length();
				recordOffset += TRAIN_RECORD_SIZE)
			{
				try
				{
					trainCounts++;
					//Check for the last train information.
					if (m_numberOfTrains == trainCounts)
						lastTrainRecord = true;
					//Parse Train Information
					importRecord(rawTable,recordOffset*2,lastTrainRecord);
				}
				catch (...)
				{
					std::ostringstream text;
					text << "readTrainTable() - Train table encountered exception.";
					LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", text.str());
				}
			}
		}

		//NOTE: If the Data Block Quality is BAD, it means there is no Data to be processed 
		// instead only DataPoints are need to be updated into Bad Quality State.
		TA_IRS_Bus::TrainDataNotificationPtr trainDataPtr(new TA_IRS_Bus::TrainDataNotification(&m_trainInfoVec));
		
		//Set the Data Block Quality
		trainDataPtr->setDataQuality(rawTable.getDatablockQuality());

		//Notify AtsData Event
		m_atsNotification->notifyAtsDataEvent(trainDataPtr);

		FUNCTION_EXIT;
	}

	void TrainTableParser::importRecord(TA_IRS_Bus::RawTable& rawTable, unsigned int recordByteOffset, bool lastTrainRecord)
	{
		FUNCTION_ENTRY("importRecord");

		//Parse Train Information
		TA_IRS_Bus::TrainInfo trainInfo;
		trainInfo.TrainCongestion    = (rawTable.getByte(recordByteOffset) == 1);
		trainInfo.TrainServiceNumber = TA_IRS_Bus::AtsHelper::IntToStr(rawTable.getWord(recordByteOffset + TRAIN_SERVICE_NUM_POS));
		trainInfo.TrainDirection     = TA_IRS_Bus::AtsHelper::IntToStr(rawTable.getByte(recordByteOffset + TRAIN_DIRECTION_POS));
		trainInfo.TrainTrackLocation = TA_IRS_Bus::AtsHelper::IntToHexStr(rawTable.getWord(recordByteOffset + TRAIN_LOCATION_POS));
		trainInfo.lastTrainRecord    = lastTrainRecord;
		trainInfo.shownTrainOnTrack  = false;
		m_trainInfoVec.push_back(trainInfo);

		FUNCTION_EXIT;
	}
	
}