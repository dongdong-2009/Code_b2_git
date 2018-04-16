/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/AtsAgent/src/AtcDataTable.cpp $
  * @author:   Darren Sampson
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last mofified by:  $Author: haijun.li $
  *
  * This represents the ATS AtcAlarm Table
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/AtsAgentException.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "app/ats/AtsAgent/src/AtsAgentRunParams.h"
#include "app/ats/AtsAgent/src/AddressUtil.h"
#include "app/ats/AtsAgent/src/AtcDataTable.h"

namespace TA_IRS_App
{
	AtcDataTable::AtcDataTable(unsigned long locationKey, TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType,
							   int iscsTablesBaseAddress, DataManager& dataManager)
	: m_locationKey (locationKey), m_locationType (locationType), 
	  m_iscsTablesBaseAddress (iscsTablesBaseAddress),
	  m_dataManager (dataManager)
	{
	}

	AtcDataTable::~AtcDataTable()
	{
	}

	void AtcDataTable::exportIscsTable (RawTable& rawTable)
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

		DataManager::TrainDataMap& trainDataMap
			= m_dataManager.getTrainDataMap();

		DataManager::TrainDataMapIt it;
		unsigned int numRecords;
		
		for (it = trainDataMap.begin(), numRecords = 0; 
			 it != trainDataMap.end() && numRecords < getNumberOfIscsRecords();
			 it++)
		{
			if ((*it).second->isAtcTrainStatusValid())
			{
				if ((*it).second != NULL &&
					(*it).second->isValidForExport(m_locationKey, m_locationType))
				{
					exportRecord(rawTable,
								 numRecords * getIscsModbusRecordSize()*2,
								 (*it).first);
					numRecords++;
				}
			}
		}

		unsigned int numBlankRecords;

		for (numBlankRecords = numRecords; 
		     numBlankRecords != (unsigned int)rawTable.length()/getIscsModbusRecordSize(); 
			 numBlankRecords++)
		{
			rawTable.putBlankRecord(numBlankRecords*getIscsModbusRecordSize()*2,
									getIscsModbusRecordSize());
		}

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
			"exportIscsTable() - AtcDataTable inserted - %d records", numRecords);
	}

	void AtcDataTable::exportRecord (RawTable& rawTable, unsigned int recordByteOffset, 
									 TA_IRS_Bus::PhysicalTrainNumberType physicalTrainNumber)
	{
		unsigned int i;
		AtsTrainData& trainData = m_dataManager.getTrainData(physicalTrainNumber);
		TA_Base_Core::ThreadGuard guard(trainData);

		// <Physical Train number>
		rawTable.putWord(recordByteOffset + 0, (unsigned short)physicalTrainNumber);

		// Code header
		rawTable.putByte(recordByteOffset + 2, trainData.getAtcTrainStatusCodeHeader());

		// Atc Status Report
		for (i = 0; i < ATC_STATUS_REPORT_SIZE && i < 23; i++)
		{
			rawTable.putByte(recordByteOffset + 3 + i, 
							 (unsigned char)trainData.getAtcStatusReport(i));
		}

		// Spare bytes
		for (i = ATC_STATUS_REPORT_SIZE; i < 23; i++)
		{
			rawTable.putByte(recordByteOffset + 3 + i, 0);
		}
	}

	unsigned short AtcDataTable::getIscsModbusStartAddress()
	{
		TA_ASSERT(m_locationType == TA_Base_Core::AtsAgentEntityData::Occ || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::MainStation ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::Depot,
				  "Unknown location type - ISCS ATC Alarm table is valid for OCC, DPT and STATION");

		// Taken from SIG-ATS/ISCS PMF
		switch (m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Occ:
			return m_iscsTablesBaseAddress + 3774;

		case TA_Base_Core::AtsAgentEntityData::MainStation:
			return m_iscsTablesBaseAddress + 402;

		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
			return m_iscsTablesBaseAddress + 1402;

		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
			return m_iscsTablesBaseAddress + 2402;
		
		case TA_Base_Core::AtsAgentEntityData::Depot:
			return m_iscsTablesBaseAddress + 2806;

		default:
			return 0;
		}
	}

	unsigned short AtcDataTable::getIscsModbusRecordSize()
	{
		TA_ASSERT(m_locationType == TA_Base_Core::AtsAgentEntityData::Occ || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::MainStation ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::Depot,
				  "Unknown location type - ISCS ATC Alarm table is valid for OCC, DPT and STATION");


		// Taken from SIG-ATS/ISCS SICD and divided by 2 to give words
		switch (m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Occ:
			return 26/2;

		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
		case TA_Base_Core::AtsAgentEntityData::MainStation:
			return 26/2;
		
		case TA_Base_Core::AtsAgentEntityData::Depot:
		default:
			return 26/2;
		}
	}

	unsigned short AtcDataTable::getIscsModbusEndAddress()
	{
		return getIscsModbusStartAddress() + getIscsModbusSize() - 1;
	}

	unsigned short AtcDataTable::getIscsModbusSize()
	{
		// In words
		return (getNumberOfIscsRecords() * getIscsModbusRecordSize());
	}
	
	unsigned int AtcDataTable::getNumberOfIscsRecords()
	{
		switch(m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Occ:
		case TA_Base_Core::AtsAgentEntityData::Depot:
			return 99;
		
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
		case TA_Base_Core::AtsAgentEntityData::MainStation:
			return 10;
		
		default:
			TA_ASSERT(false, "Unknown location type - ATS ATC Alarm table is valid for OCC, DPT and STATION");
			return 0;
		}
	}

	bool AtcDataTable::tableWriteAllowed()
	{
		bool writeAllowed = false;
		OperationModeManager& operationModeManager = m_dataManager.getOperationModeManager();

		switch (m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Occ:
			writeAllowed = (operationModeManager.getAgentState() != OperationModeManager::AgentRadioFallback);
			break;

		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
		case TA_Base_Core::AtsAgentEntityData::MainStation:
			writeAllowed = (operationModeManager.getAgentState() != OperationModeManager::AgentNormal);
			break;
		
		case TA_Base_Core::AtsAgentEntityData::Depot:
		default:
			writeAllowed = true;
		}

		return writeAllowed;
	}
}
