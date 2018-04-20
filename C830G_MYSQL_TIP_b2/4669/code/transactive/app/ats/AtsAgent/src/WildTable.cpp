/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/ats/AtsAgent/src/WildTable.cpp $
  * @author:   Darren Sampson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last mofified by:  $Author: builder $
  *
  * This represents the ATS Train Table
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <sstream>

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/AtsAgentException.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "app/ats/AtsAgent/src/AddressUtil.h"
#include "app/ats/AtsAgent/src/WildTable.h"

namespace TA_IRS_App
{
	WildTable::WildTable(unsigned long entityKey,
							   unsigned long locationKey, 
							   TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType,
							   int atsTablesBaseAddress,
							   DataManager& dataManager)
	: m_locationType(locationType),
	  m_atsTablesBaseAddress (atsTablesBaseAddress),
	  m_dataManager(dataManager)
	{
		TA_ASSERT(locationType == TA_Base_Core::AtsAgentEntityData::Occ, "WILD Table only valid at OCC");
	};

	unsigned int WildTable::getNumberOfAtsRecords()
	{
		return 5;
	}

	void WildTable::importAtsTable(RawTable& rawTable)
	{
		OperationModeManager& operationModeManager = m_dataManager.getOperationModeManager();

		if (operationModeManager.inControlMode() == false)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "importAtsTable() - ignored as agent in Monitor Mode");
			return;
		}

		unsigned int i = 0;
		int recordWordOffset;

		for (recordWordOffset = 0, i = 0; 
		     recordWordOffset < rawTable.length() && i < 5;
			 recordWordOffset += getAtsModbusRecordSize(), i++)
		{
			importRecord(rawTable, recordWordOffset*2, i);
		}

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
			"importAtsTable() - WildTable imported - received %d records", i);
	}

	void WildTable::importRecord(RawTable& rawTable, unsigned int recordByteOffset,
								 unsigned int index)
	{
		AtsWildData& wildData = m_dataManager.getWildData(index);
		TA_Base_Core::ThreadGuard guard(wildData);

		//
		// Time of train crossing the WILD
		//
		TA_IRS_Bus::TimeStampType timeStamp;
				
		timeStamp.year = rawTable.getWord(recordByteOffset + 0);
		timeStamp.month = rawTable.getByte(recordByteOffset + 3);
		timeStamp.day = rawTable.getByte(recordByteOffset + 5);
		timeStamp.hour = rawTable.getByte(recordByteOffset + 7);
		timeStamp.minute = rawTable.getByte(recordByteOffset + 9);
		timeStamp.second = rawTable.getByte(recordByteOffset + 11);

		wildData.updateTimestamp(timeStamp);

		//
		// Train and Leading cab identification
		//
		
		// <Physical Train number>
		wildData.updatePhysicalTrainNumber(TA_IRS_Bus::PhysicalTrainNumberType(rawTable.getWord(recordByteOffset + 12)));
	
		// <Active Cab number>
		wildData.updateActiveCabNumber(TA_IRS_Bus::IAtsWildCorbaDef::ActiveCabNumberType(rawTable.getWord(recordByteOffset + 14)),TA_IRS_Bus::PhysicalTrainNumberType(rawTable.getWord(recordByteOffset + 12)));

		// <Active Cab number>
		wildData.updateHeadCabDirection(TA_IRS_Bus::IAtsWildCorbaDef::EHeadCabDirection(rawTable.getByte(recordByteOffset + 17)));
	}

	unsigned short WildTable::getAtsModbusStartAddress()
	{
		TA_ASSERT(m_locationType == TA_Base_Core::AtsAgentEntityData::Occ, "WILD Table only valid at OCC");

		// Taken from SIG-ATS/ISCS PMF
		switch (m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Occ:
			return m_atsTablesBaseAddress + 14906;

		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
		case TA_Base_Core::AtsAgentEntityData::MainStation:
		case TA_Base_Core::AtsAgentEntityData::Depot:
		default:
			return 0;
		}
	}

	unsigned short WildTable::getAtsModbusRecordSize()
	{
		TA_ASSERT(m_locationType == TA_Base_Core::AtsAgentEntityData::Occ, "WILD Table only valid at OCC");

		// Taken from SIG-ATS/ISCS SICD and divided by 2 to give words
		switch (m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Occ:
			return 18/2;

		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
		case TA_Base_Core::AtsAgentEntityData::MainStation:
		case TA_Base_Core::AtsAgentEntityData::Depot:
		default:
			return 0;
		}
	}

	unsigned short WildTable::getAtsModbusEndAddress()
	{
		return getAtsModbusStartAddress() + getAtsModbusSize() - 1;
	}

	unsigned short WildTable::getAtsModbusSize()
	{
		// In words
		return (5 * getAtsModbusRecordSize());
	}

	bool WildTable::tableReadAllowed()
	{
		bool readAllowed = false;
	
		switch (m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Occ:
			readAllowed = true;
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
}
