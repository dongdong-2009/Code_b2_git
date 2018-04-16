/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/AtsAgent/src/DssTable.cpp $
  * @author:   Darren Sampson
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last mofified by:  $Author: haijun.li $
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
#include "app/ats/AtsAgent/src/DssTable.h"

namespace TA_IRS_App
{
	DssTable::DssTable(TA_Base_Core::AtsAgentEntityDataPtr entityData, DataManager& dataManager)
	: m_locationType(entityData->getAgentAtsLocationType()), 
	  m_atsTablesBaseAddress (entityData->getAtsTablesBaseAddress()), 
	  m_dataManager(dataManager)
	{
		TA_ASSERT(m_locationType == TA_Base_Core::AtsAgentEntityData::Occ || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::MainStation ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::Depot,
				  "Unknown location type - DSS table is valid for OCC, DPT and STATION");

		if(m_locationType == TA_Base_Core::AtsAgentEntityData::Occ)
		{
			m_atsRecordNum = 32;
		}
		else
		{
			m_atsRecordNum = 16;
		}
	};

	unsigned int DssTable::getNumberOfAtsRecords()
	{
		return m_atsRecordNum;
	}

	void DssTable::importAtsTable(RawTable& rawTable)
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
		std::vector<AtsDssData> incomingData;
		std::vector<AtsDssData> dataNeedToTriggerPlan;

		for (recordWordOffset = 0, i = 0; 
		     recordWordOffset < rawTable.length() && i < m_atsRecordNum;
			 recordWordOffset += getAtsModbusRecordSize(), i++)
		{
			importRecord(rawTable, recordWordOffset*2, incomingData);
		}

		for(std::vector<AtsDssData>::iterator itIncoming = incomingData.begin(); itIncoming != incomingData.end(); ++itIncoming)
		{
			bool isfound = false;
			for(std::vector<AtsDssData>::iterator itPrevious = m_lastKnownData.begin(); itPrevious != m_lastKnownData.end(); ++itPrevious)
			{
				if (*itIncoming == *itPrevious)
				{
					//we already trigger this dss event.
					//detele it from m_lastKnownData. so make next search more efficient.
					m_lastKnownData.erase(itPrevious);
					isfound = true;
					break;
				}
			}

			if (!isfound)
			{
				dataNeedToTriggerPlan.push_back(*itIncoming);
			}
		}

		m_dataManager.addNewDssEvent(dataNeedToTriggerPlan);

		m_lastKnownData = incomingData;

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
			"importAtsTable() - DssTable imported - received %d new records", 
			dataNeedToTriggerPlan.size());
	}

	void DssTable::importRecord(RawTable& rawTable, unsigned int recordByteOffset, 
								std::vector< AtsDssData >& incomingData)
	{
		unsigned long year = rawTable.getWord(recordByteOffset + 0);
		if (year != 0)
		{
			struct tm when;			
			when.tm_year = year - 1900;
			when.tm_mon	 = rawTable.getByte(recordByteOffset + 3) - 1;
			when.tm_mday = rawTable.getByte(recordByteOffset + 5);
			when.tm_hour = rawTable.getByte(recordByteOffset + 7);
			when.tm_min	 = rawTable.getByte(recordByteOffset + 9);
			when.tm_sec	 = rawTable.getByte(recordByteOffset + 11);
			
			AtsDssData newData;
			newData.timestamp = mktime(&when);
			newData.dssEvent  = rawTable.getWord(recordByteOffset + 12);

			std::stringstream temp;
			temp << rawTable.getByte(recordByteOffset + 14)
				 << rawTable.getByte(recordByteOffset + 15)
				 << rawTable.getByte(recordByteOffset + 16)
				 << rawTable.getByte(recordByteOffset + 17);
			newData.dssInput  = temp.str();

			incomingData.push_back(newData);
		}		
	}

	unsigned short DssTable::getAtsModbusStartAddress()
	{
		TA_ASSERT(m_locationType == TA_Base_Core::AtsAgentEntityData::Occ || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::MainStation ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::Depot,
				  "Unknown location type - DSS table is valid for OCC, DPT and STATION");

		// Taken from SIG-ATS/ISCS PMF
		switch (m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Occ:
			return m_atsTablesBaseAddress + 14996;

		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
		case TA_Base_Core::AtsAgentEntityData::MainStation:
			return m_atsTablesBaseAddress + 3082;

		case TA_Base_Core::AtsAgentEntityData::Depot:
			return m_atsTablesBaseAddress + 9506;
			
		default:
			return 0;
		}
	}

	unsigned short DssTable::getAtsModbusRecordSize()
	{
		TA_ASSERT(m_locationType == TA_Base_Core::AtsAgentEntityData::Occ || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::MainStation ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::Depot,
				  "Unknown location type - DSS table is valid for OCC, DPT and STATION");

		// Taken from SIG-ATS/ISCS SICD and divided by 2 to give words
		switch (m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Occ:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
		case TA_Base_Core::AtsAgentEntityData::MainStation:
		case TA_Base_Core::AtsAgentEntityData::Depot:
			return 18/2;
		
		default:
			return 0;
		}
	}

	unsigned short DssTable::getAtsModbusEndAddress()
	{
		return getAtsModbusStartAddress() + getAtsModbusSize() - 1;
	}

	unsigned short DssTable::getAtsModbusSize()
	{
		// In words
		return (m_atsRecordNum * getAtsModbusRecordSize());
	}

	bool DssTable::tableReadAllowed()
	{
		return false;
	}
}
