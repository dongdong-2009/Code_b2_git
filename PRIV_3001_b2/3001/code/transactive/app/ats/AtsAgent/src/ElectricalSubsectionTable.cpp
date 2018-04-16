/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/ats/AtsAgent/src/ElectricalSubsectionTable.cpp $
  * @author:   Darren Sampson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last mofified by:  $Author: lim.cy $
  *
  * This represents the ATS ElectricalSubsection Table
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/AtsAgentException.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "app/ats/AtsAgent/src/AtsAgentRunParams.h"
#include "app/ats/AtsAgent/src/AddressUtil.h"
#include "app/ats/AtsAgent/src/ElectricalSubsectionTable.h"

namespace TA_IRS_App
{
	ElectricalSubsectionTable::ElectricalSubsectionTable(unsigned long locationKey, 
														 TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType,
														 int iscsTablesBaseAddress, DataManager& dataManager)
	: m_locationType (locationType),
	  m_iscsTablesBaseAddress (iscsTablesBaseAddress), m_dataManager(dataManager)
	{
		TA_ASSERT(locationType == TA_Base_Core::AtsAgentEntityData::Occ || 
				  locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
				  locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
				  locationType == TA_Base_Core::AtsAgentEntityData::MainStation,
				  "ISCS Electrical Subsection Table only valid at OCC or STATION");
	};

	void ElectricalSubsectionTable::exportIscsTable (RawTable& rawTable)
	{
		OperationModeManager& operationModeManager = m_dataManager.getOperationModeManager();

		if (operationModeManager.inControlMode() == false)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "exportIscsTable() - ignored as agent in Monitor Mode");
			return;
		}

		if (rawTable.length() != getIscsModbusSize())
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
					"exportIscsTable() - Error: Size of raw table does not match size of table - raw table %d, table %d.",
					rawTable.length(), getIscsModbusSize());
		}

		DataManager::PowerDataMap& powerDataMap = m_dataManager.getPowerDataMap();

		if (rawTable.length() < (signed int)powerDataMap.size()*getIscsModbusRecordSize())
		{
			std::ostringstream message;
			message.clear();
			message << "exportIscsTable() - Error: Size of raw table is less than the minimum size of the table - raw table ";
			message << rawTable.length();
			message << ", table ";
			message << powerDataMap.size()*
						getIscsModbusRecordSize();
			TA_THROW(TA_Base_Core::AtsAgentException(message.str()));
		}

		if (rawTable.length()%getIscsModbusRecordSize() != 0)
		{
			std::ostringstream message;
			message.clear();
			message << "exportIscsTable() - Error: Size of raw table is not a multiple of the size of the records - raw table ";
			message << rawTable.length();
			message << ", record size ";
			message << getIscsModbusRecordSize();
			TA_THROW(TA_Base_Core::AtsAgentException(message.str()));
		}

		DataManager::PowerDataMapIt it;
		unsigned int numRecords;
		
		for (it = powerDataMap.begin(), numRecords = 0; 
			 it != powerDataMap.end() && numRecords < getNumberOfIscsRecords();
			 it++, numRecords++)
		{
			exportRecord(rawTable, 
						 numRecords*getIscsModbusRecordSize()*2,
						 (*it).first);
		}

		unsigned int numBlankRecords;

		for (numBlankRecords = numRecords; 
		     numBlankRecords != (unsigned int)(rawTable.length()/getIscsModbusRecordSize()); 
			 numBlankRecords++)
		{
			rawTable.putBlankRecord(numBlankRecords*getIscsModbusRecordSize()*2,
									getIscsModbusRecordSize());
		}

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
			"exportIscsTable() - ElectricalSubsectionTable exported - %d records", numRecords);
	}
		
	void ElectricalSubsectionTable::exportRecord (RawTable& rawTable, unsigned int recordByteOffset,
												  unsigned short subsectionId)
	{
		PowerData& powerData = m_dataManager.getPowerData(subsectionId);
		TA_Base_Core::ThreadGuard guard(powerData);

		// <Platform area ID>
		rawTable.putWord(recordByteOffset + 0, powerData.getSubsectionId());

		// <Validity>
		rawTable.putByte(recordByteOffset + 2, powerData.getValidityByte());

		// <Power Status>
		rawTable.putByte(recordByteOffset + 3, powerData.getPowerStatusByte());
	}

	unsigned short ElectricalSubsectionTable::getIscsModbusStartAddress()
	{
		TA_ASSERT(m_locationType == TA_Base_Core::AtsAgentEntityData::Occ || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::MainStation,
				  "ISCS Electrical Subsection Table only valid at OCC or STATION");

		// Taken from SIG-ATS/ISCS PMF
		switch (m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Occ:
			return m_iscsTablesBaseAddress + 2774;

		case TA_Base_Core::AtsAgentEntityData::MainStation:
			return m_iscsTablesBaseAddress + 342;

		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
			return m_iscsTablesBaseAddress + 1342;

		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
			return m_iscsTablesBaseAddress + 2342;
		
		case TA_Base_Core::AtsAgentEntityData::Depot:
		default:
			return 0;
		}
	}

	unsigned short ElectricalSubsectionTable::getIscsModbusRecordSize()
	{
		TA_ASSERT(m_locationType == TA_Base_Core::AtsAgentEntityData::Occ || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::MainStation,
				  "ISCS Electrical Subsection Table only valid at OCC or STATION");

		// Taken from SIG-ATS/ISCS SICD and divided by 2 to give words
		switch (m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Occ:
			return 4/2;

		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
		case TA_Base_Core::AtsAgentEntityData::MainStation:
			return 4/2;
		
		case TA_Base_Core::AtsAgentEntityData::Depot:
		default:
			return 0;
		}
	}

	unsigned short ElectricalSubsectionTable::getIscsModbusEndAddress()
	{
		return getIscsModbusStartAddress() + getIscsModbusSize() - 1;
	}

	unsigned short ElectricalSubsectionTable::getIscsModbusSize()
	{
		// In words
		return (getNumberOfIscsRecords() * getIscsModbusRecordSize());
	}

	unsigned int ElectricalSubsectionTable::getNumberOfIscsRecords()
	{
		switch(m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Occ:
		case TA_Base_Core::AtsAgentEntityData::Depot:
			return 250;
		
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
		case TA_Base_Core::AtsAgentEntityData::MainStation:
			return 15;
		
		default:
			TA_ASSERT(false, "Unknown location type - ISCS Electrical Subsection Table only valid at OCC or STATION");
			return 0;
		}
	}

	bool ElectricalSubsectionTable::tableWriteAllowed()
	{
		bool writeAllowed = false;
		/*

		switch (m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Occ:
			writeAllowed = true;
			break;

		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
		case TA_Base_Core::AtsAgentEntityData::MainStation:
			writeAllowed = true;
			break;
		
		case TA_Base_Core::AtsAgentEntityData::Depot:
		default:
			writeAllowed = false;
		}*/


		return writeAllowed;

	}
}
