/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/ats/AtsAgent/src/WashTable.cpp $
  * @author:   Darren Sampson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last mofified by:  $Author: lim.cy $
  *
  * This represents the Wash Table
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <sstream>

#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/ats/src/AtsTWPDataFactory.h"
#include "core/data_access_interface/ats/src/AtsTWPData.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/AtsAgentException.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "app/ats/AtsAgent/src/AddressUtil.h"
#include "app/ats/AtsAgent/src/WashTable.h"

namespace TA_IRS_App
{
	WashTable::WashTable(	unsigned long entityKey, 
								unsigned long locationKey, 
								TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType,
								int atsTablesBaseAddress, int iscsTablesBaseAddress,
								DataManager& dataManager)
	: m_locationType (locationType),
	  m_atsTablesBaseAddress (atsTablesBaseAddress),
	  m_iscsTablesBaseAddress (iscsTablesBaseAddress),
	  m_dataManager(dataManager)
	{
		TA_ASSERT(locationType == TA_Base_Core::AtsAgentEntityData::Depot,
				  "ATS Wash table is only valid for DPT");
	};

	WashTable::~WashTable()
	{
	}

	unsigned int WashTable::getNumberOfAtsRecords()
	{
		return 1;
	}
		
	unsigned short WashTable::getAtsModbusStartAddress()
	{
		TA_ASSERT(m_locationType == TA_Base_Core::AtsAgentEntityData::Depot,
				  "Wash table is only valid for DPT");

		// Taken from SIG-ATS/ISCS PMF
		switch (m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Depot:
			return m_atsTablesBaseAddress + 9586;
		
		case TA_Base_Core::AtsAgentEntityData::Occ:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
		case TA_Base_Core::AtsAgentEntityData::MainStation:
		default:
			return 0;
		}
	}

	unsigned short WashTable::getAtsModbusRecordSize()
	{
		TA_ASSERT(m_locationType == TA_Base_Core::AtsAgentEntityData::Depot,
				  "ATS Wash table is only valid for DPT");

		// Taken from SIG-ATS/ISCS SICD and divided by 2 to give words
		switch (m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Depot:
			return 8/2;

		case TA_Base_Core::AtsAgentEntityData::Occ:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
		case TA_Base_Core::AtsAgentEntityData::MainStation:
		default:
			return 0;
		}
	}

	unsigned short WashTable::getAtsModbusEndAddress()
	{
		return getAtsModbusStartAddress() + getAtsModbusSize() - 1;
	}

	unsigned short WashTable::getAtsModbusSize()
	{
		// In words
		return getAtsModbusRecordSize();
	}

	void WashTable::importAtsTable(RawTable& rawTable)
	{	
		OperationModeManager& operationModeManager = m_dataManager.getOperationModeManager();

		if (operationModeManager.inControlMode() == false)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "importAtsTable() - ignored as agent in Monitor Mode");
			return;
		}

		importRecord(rawTable, 0);
			
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
			"importAtsTable() - WashTable imported");
	}

	void WashTable::importRecord(RawTable& rawTable, unsigned int recordByteOffset)
	{	
		AtsTwpData& twpData = m_dataManager.getTwpData();
		TA_Base_Core::ThreadGuard guard(twpData);

		// <Validity field>
		if (rawTable.getByte(recordByteOffset + 0) == 1)
		{
			twpData.updateValidity(true);

			// <Selection of No Wash mode>
			twpData.updateNoWashControl(rawTable.getByte(recordByteOffset + 1) == 1);
			
			// <Selection of Daily rinse wash>
			twpData.updateDailyRinseWashControl(rawTable.getByte(recordByteOffset + 2) == 1);
				
			// <Selection of Daily detergent wash>
			twpData.updateDailyDetergentWashControl(rawTable.getByte(recordByteOffset + 3) == 1);
			
			// <Selection of Weekly wash>
			twpData.updateWeeklyWashControl(rawTable.getByte(recordByteOffset + 4) == 1);
			
			// <Train stopped for faces wash>
			twpData.updateTrainStoppedForCabEndWash(rawTable.getByte(recordByteOffset + 5) == 1);
			
			// <Stop WP request>
			twpData.updateStopWpControl(rawTable.getByte(recordByteOffset + 6) == 1);
			
			// <Restart WP request>
			twpData.updateRestartWpControl(rawTable.getByte(recordByteOffset + 7) == 1);
		}
		else
		{
			twpData.updateValidity(false);
		}
	}

	unsigned short WashTable::getIscsModbusStartAddress()
	{
		TA_ASSERT(m_locationType == TA_Base_Core::AtsAgentEntityData::Depot,
				  "Wash table is only valid for DPT");

		// Taken from SIG-ATS/ISCS PMF
		switch (m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Depot:
			return m_iscsTablesBaseAddress + 2774;

		case TA_Base_Core::AtsAgentEntityData::Occ:
		case TA_Base_Core::AtsAgentEntityData::MainStation:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
		default:
			return 0;
		}
	}

	unsigned short WashTable::getIscsModbusRecordSize()
	{
		TA_ASSERT(m_locationType == TA_Base_Core::AtsAgentEntityData::Depot,
				  "ISCS Wash table is only valid for DPT");

		// Taken from SIG-ATS/ISCS SICD and divided by 2 to give words
		switch (m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Depot:
			return 34/2;

		case TA_Base_Core::AtsAgentEntityData::Occ:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
		case TA_Base_Core::AtsAgentEntityData::MainStation:
		default:
			return 0;
		}
	}

	unsigned short WashTable::getIscsModbusEndAddress()
	{
		return getIscsModbusStartAddress() + getIscsModbusSize() - 1;
	}

	unsigned short WashTable::getIscsModbusSize()
	{
		// In words
		return (getNumberOfIscsRecords() * getIscsModbusRecordSize());
	}

	unsigned int WashTable::getNumberOfIscsRecords()
	{
		return 1;
	}

	bool WashTable::tableReadAllowed()
	{
		bool readAllowed = false;
	
		switch (m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Occ:
			readAllowed = false;
			break;

		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
		case TA_Base_Core::AtsAgentEntityData::MainStation:
			readAllowed = false;
			break;
		
		case TA_Base_Core::AtsAgentEntityData::Depot:
		default:
			readAllowed = false;
			break;
		}

		return readAllowed;
	}

	void WashTable::exportIscsTable(RawTable& rawTable)
	{
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "exportIscsTable() - WashTable");

		OperationModeManager& operationModeManager = m_dataManager.getOperationModeManager();

		if (operationModeManager.inControlMode() == false)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "exportIscsTable() - ignored as agent in Monitor Mode");
			return;
		}

		if (rawTable.length() != getIscsModbusSize())
		{
			std::ostringstream message;
			message.clear();
			message << "exportIscsTable() - Error: Size of raw table does not match size of table - raw table ";
			message << rawTable.length();
			message << ", table ";
			message << getIscsModbusSize();
			TA_THROW(TA_Base_Core::AtsAgentException(message.str()));
		}

		exportRecord(rawTable, 0);

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
			"exportIscsTable() - WashTable inserted");
	}

	void WashTable::exportRecord(RawTable& rawTable, unsigned int recordByteOffset)
	{
		bool valid = true;
		AtsTwpData& twpData = m_dataManager.getTwpData();
		TA_Base_Core::ThreadGuard guard(twpData);

		// <WP Status>,TD19827, SITE - Mapping Of TWP Status From TWP to ATS, modified by hongzhi
		rawTable.putByte(recordByteOffset + 1, 
						 ((unsigned char)((int)(twpData.getTwpValue(TA_IRS_Core::AtsTWPData::WP_STATUS, valid))-1)));
		// TD19827 completed
		// <No wash selected>
		rawTable.putByte(recordByteOffset + 2, 
						 twpData.getTwpValue(TA_IRS_Core::AtsTWPData::NO_WASH_SELECTED, valid));

		// <Daily rinse selected>
		rawTable.putByte(recordByteOffset + 3, 
						 twpData.getTwpValue(TA_IRS_Core::AtsTWPData::DAILY_RINSE_SELECTED, valid));

		// <Daily detergent selected>
		rawTable.putByte(recordByteOffset + 4, 
						 twpData.getTwpValue(TA_IRS_Core::AtsTWPData::DAILY_DETERGENT_SELECTED, valid));
		
		// <Weekly wash selected>
		rawTable.putByte(recordByteOffset + 5, 
						 twpData.getTwpValue(TA_IRS_Core::AtsTWPData::WEEKLY_WASH_SELECTED, valid));
		
		// <Cab end wash completed>
		rawTable.putByte(recordByteOffset + 6, 
						 twpData.getTwpValue(TA_IRS_Core::AtsTWPData::CAB_END_WASH_COMPLETED, valid));
		
		// <Train Wash cycle fully completed>
		rawTable.putByte(recordByteOffset + 7, 
						 twpData.getTwpValue(TA_IRS_Core::AtsTWPData::TRAIN_WASH_CYCLE_COMPLETED, valid));

		// Spare bits
		rawTable.putByte(recordByteOffset + 8, 0);

		// <Train Overspeed>
		rawTable.putByte(recordByteOffset + 9, 
						 twpData.getTwpValue(TA_IRS_Core::AtsTWPData::TRAIN_SPEED_OUT_OF_RANGE, valid));
		
		// <Train stalled in WP (detected by TWP)>
		rawTable.putByte(recordByteOffset + 10, 
						 twpData.getTwpValue(TA_IRS_Core::AtsTWPData::TRAIN_STALLED_IN_TWP, valid));

		// <Train incorrectly Positioned>
		rawTable.putByte(recordByteOffset + 11, 
						 twpData.getTwpValue(TA_IRS_Core::AtsTWPData::TRAIN_INCORRECTLY_POSITIONED, valid));
	
		// Spare bits
		rawTable.putByte(recordByteOffset + 12, 0);

		// <Wash Cyle Start Failure>
		rawTable.putByte(recordByteOffset + 13, 
						 twpData.getTwpValue(TA_IRS_Core::AtsTWPData::WASH_CYCLE_START_FAILURE, valid));

		// <pH check (<6 or >9)>
		rawTable.putByte(recordByteOffset + 14, 
						 twpData.getTwpValue(TA_IRS_Core::AtsTWPData::PH_CHECK, valid));

		// <Mains Water Low>
		rawTable.putByte(recordByteOffset + 15, 
						 twpData.getTwpValue(TA_IRS_Core::AtsTWPData::MAINS_WATER_LOW, valid));

		// <Second Brush Rinse and Final Rinse Spray>
		rawTable.putByte(recordByteOffset + 16, 
						 twpData.getTwpValue(TA_IRS_Core::AtsTWPData::SECOND_BRUSH_RINSE, valid));
		
		// <End Wash Gantry Movement Failure (hor.)>
		rawTable.putByte(recordByteOffset + 17, 
						 twpData.getTwpValue(TA_IRS_Core::AtsTWPData::CAB_END_WASH_GANTRY_FAILURE_HOR, valid));

		// <End Wash Gantry Movement Failure (ver.)>
		rawTable.putByte(recordByteOffset + 18, 
						 twpData.getTwpValue(TA_IRS_Core::AtsTWPData::CAB_END_WASH_GANTRY_FAILURE_VER, valid));
		
		// <End Wash Brush Movement Failure>
		rawTable.putByte(recordByteOffset + 19, 
						 twpData.getTwpValue(TA_IRS_Core::AtsTWPData::CAB_END_WASH_BRUSH_FAILURE, valid));
		
		// <EMERGENCY STOP ACTIVATED>
		rawTable.putByte(recordByteOffset + 20, 
						 twpData.getTwpValue(TA_IRS_Core::AtsTWPData::EMERGENCY_STOP_ACTIVATED, valid));
	
		// <Pneumatic Failure>
		rawTable.putByte(recordByteOffset + 21, 
						 twpData.getTwpValue(TA_IRS_Core::AtsTWPData::PNEUMATIC_FAILURE, valid));
		
		// <Brush Actuation (individual) Failure>
		rawTable.putByte(recordByteOffset + 22, 
						 twpData.getTwpValue(TA_IRS_Core::AtsTWPData::BRUSH_ACTUATION_FAILURE, valid));
		
		// <Side/roof Brushing Failure>
		rawTable.putByte(recordByteOffset + 23, 
						 twpData.getTwpValue(TA_IRS_Core::AtsTWPData::SIDE_ROOF_BRUSHING_FAILURE, valid));
	
		// <End Brushing Failure>
		rawTable.putByte(recordByteOffset + 24, 
						 twpData.getTwpValue(TA_IRS_Core::AtsTWPData::END_BRUSHING_FAILURE, valid));
		
		// <Air Blow Failure>
		rawTable.putByte(recordByteOffset + 25, 
						 twpData.getTwpValue(TA_IRS_Core::AtsTWPData::AIR_BLOW_FAILURE, valid));

		// <Pre-wet Failure>
		rawTable.putByte(recordByteOffset + 26, 
						 twpData.getTwpValue(TA_IRS_Core::AtsTWPData::PRE_WET_FAILURE, valid));
		
		// <Recycled Rinse failure>
		rawTable.putByte(recordByteOffset + 27, 
						 twpData.getTwpValue(TA_IRS_Core::AtsTWPData::RECYCLED_RINSE_FAILURE, valid));

		// <Water Recycling Failure>
		rawTable.putByte(recordByteOffset + 28, 
						 twpData.getTwpValue(TA_IRS_Core::AtsTWPData::WATER_RECYCLING_FAILURE, valid));
		
		// <Detergent failure>
		rawTable.putByte(recordByteOffset + 29, 
						 twpData.getTwpValue(TA_IRS_Core::AtsTWPData::DETERGENT_FAILURE, valid));

		// <Detergent Low Alarms>
		rawTable.putByte(recordByteOffset + 30, 
						 twpData.getTwpValue(TA_IRS_Core::AtsTWPData::DETERGENT_LOW_ALARMS, valid));

		// <Air Pressure Low>
		rawTable.putByte(recordByteOffset + 31, 
						 twpData.getTwpValue(TA_IRS_Core::AtsTWPData::AIR_PRESSURE_LOW, valid));
		
		// <Non Critical Summary Alarm>
		rawTable.putByte(recordByteOffset + 32, 
						 twpData.getTwpValue(TA_IRS_Core::AtsTWPData::NON_CRITICAL_SUMMARY_ALARM, valid));

		// Spare bits
		rawTable.putByte(recordByteOffset + 33, 0);

		// <Validity Field>
		rawTable.putByte(recordByteOffset + 0, valid?1:0);
	}

	bool WashTable::tableWriteAllowed()
	{
		/*
		bool writeAllowed = false;

		switch (m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Occ:
			writeAllowed = false;
			break;

		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
		case TA_Base_Core::AtsAgentEntityData::MainStation:
			writeAllowed = false;
			break;
		
		case TA_Base_Core::AtsAgentEntityData::Depot:
		default:
			writeAllowed = true;
			break;
		}

		return writeAllowed;*/
		return false ;
	}
}
