/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/ats/AtsAgent/src/IscsWatchDogTable.cpp $
  * @author:   Darren Sampson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last mofified by:  $Author: lim.cy $
  *
  * This represents the ATS Train Record
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <time.h>
#include <sstream>

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/AtsAgentException.h"

#include "app/ats/AtsAgent/src/AddressUtil.h"
#include "app/ats/AtsAgent/src/IscsWatchDogTable.h"

namespace TA_IRS_App
{
	IscsWatchDogTable::IscsWatchDogTable(TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType,
										 int iscsTablesBaseAddress, DataManager& dataManager)
	:
	m_chronologicalCounter (0xFF00), m_locationType (locationType),
	m_iscsTablesBaseAddress (iscsTablesBaseAddress), m_dataManager (dataManager)
	{
		TA_ASSERT(locationType == TA_Base_Core::AtsAgentEntityData::Occ || 
				  locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
				  locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
				  locationType == TA_Base_Core::AtsAgentEntityData::MainStation ||
				  locationType == TA_Base_Core::AtsAgentEntityData::Depot,
				  "Unknown location type - ISCS Watch Dog table is valid for OCC, DPT and STATION");
	};

	IscsWatchDogTable::~IscsWatchDogTable()
	{
	}

	void IscsWatchDogTable::updateWatchdogCounter(int differenceInSecs)
	{
		// Need to incrememt the counter for each second
		m_chronologicalCounter = (m_chronologicalCounter+differenceInSecs) % 0xFFFF;
	}

	void IscsWatchDogTable::exportIscsTable(RawTable& rawTable)
	{
		/*
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

		// <Watch dog>
		rawTable.putWord(0, m_chronologicalCounter);

        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
			"exportIscsTable() - IscsWatchDogTable inserted");*/

	}

	unsigned short IscsWatchDogTable::getIscsModbusStartAddress()
	{
		TA_ASSERT(m_locationType == TA_Base_Core::AtsAgentEntityData::Occ || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::MainStation ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::Depot,
				  "Unknown location type - ISCS Watchdog table is valid for OCC, DPT and STATION");

		// Taken from SIG-ATS/ISCS PMF
		switch (m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Occ:
			return m_iscsTablesBaseAddress;

		case TA_Base_Core::AtsAgentEntityData::MainStation:
			return m_iscsTablesBaseAddress;

		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
			return m_iscsTablesBaseAddress + 1000;

		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
			return m_iscsTablesBaseAddress + 2000;

		case TA_Base_Core::AtsAgentEntityData::Depot:
			return m_iscsTablesBaseAddress;

		default:
			return 0;
		}
	}

	unsigned short IscsWatchDogTable::getIscsModbusEndAddress()
	{
		return getIscsModbusStartAddress() + getIscsModbusSize() - 1;
	}

	unsigned short IscsWatchDogTable::getIscsModbusSize()
	{
		// In words
		return (IscsWatchDogTable::getStaticModbusRecordSize(m_locationType));
	}

	unsigned short IscsWatchDogTable::getStaticModbusRecordSize(TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType)
	{
		TA_ASSERT(locationType == TA_Base_Core::AtsAgentEntityData::Occ || 
				  locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
				  locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
				  locationType == TA_Base_Core::AtsAgentEntityData::MainStation ||
				  locationType == TA_Base_Core::AtsAgentEntityData::Depot,
				  "Unknown location type - ISCS Watchdog table is valid for OCC, DPT and STATION");

		// Taken from SIG-ATS/ISCS SICD and divided by 2 to give words
		switch (locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Occ:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
		case TA_Base_Core::AtsAgentEntityData::MainStation:
		case TA_Base_Core::AtsAgentEntityData::Depot:
			return 2/2;

		default:
			return 0;
		}
	}

	unsigned int IscsWatchDogTable::getNumberOfIscsRecords()
	{
		return 1;
	}
}
