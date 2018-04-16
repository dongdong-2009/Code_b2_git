/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/ats/AtsAgent/src/AtsWatchDogTable.cpp $
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

#include <sstream>

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/AtsAgentException.h"

#include "app/ats/AtsAgent/src/AddressUtil.h"
#include "app/ats/AtsAgent/src/WatchDogData.h"
#include "app/ats/AtsAgent/src/AtsWatchDogTable.h"

namespace TA_IRS_App
{
	AtsWatchDogTable::AtsWatchDogTable(	unsigned long entityKey, 
										unsigned long locationKey, 
										TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType,
										unsigned long atsTablesBaseAddress,
										DataManager& dataManager)
	:
	m_entityKey (entityKey), 
	m_locationKey (locationKey),
	m_locationType(locationType),
	m_atsTablesBaseAddress (atsTablesBaseAddress),
	m_dataManager(dataManager)
	{
		TA_ASSERT(locationType == TA_Base_Core::AtsAgentEntityData::Occ || 
				  locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
				  locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
				  locationType == TA_Base_Core::AtsAgentEntityData::MainStation ||
				  locationType == TA_Base_Core::AtsAgentEntityData::Depot,
				  "Unknown location type - ATS Watchdog table is valid for OCC, DPT and STATION");
	};

	AtsWatchDogTable::~AtsWatchDogTable()
	{
	}

	unsigned int AtsWatchDogTable::getNumberOfAtsRecords()
	{
		return 1;
	}

	void AtsWatchDogTable::importAtsTable(RawTable& rawTable, int connection)
	{
		OperationModeManager& operationModeManager = m_dataManager.getOperationModeManager();

		if (operationModeManager.inControlMode() == false)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "importAtsTable() - ignored as agent in Monitor Mode");
			return;
		}

		WatchDogData& watchDogData = m_dataManager.getWatchDogData();

		unsigned long chronologicalCounter = rawTable.getWord(0);

		//Log for the Ats WatchDogTable
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"\n\n\t\t\t\t\t\t***** WatchDog Table *****"
			"\n\t\t\t\t\t\tChronological Counter : %ld\n\n", chronologicalCounter);

		watchDogData.updateWatchDog(connection, chronologicalCounter);

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
			"importAtsTable() - AtsWatchDogTable imported for connection %d", connection);
	}

	unsigned short AtsWatchDogTable::getAtsModbusStartAddress()
	{
		TA_ASSERT(m_locationType == TA_Base_Core::AtsAgentEntityData::Occ ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::MainStation ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::Depot,
				  "Unknown location type - ATS Watchdog table is valid for OCC, DPT and STATION");

		// Taken from SIG-ATS/ISCS PMF
		switch (m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Occ:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
		case TA_Base_Core::AtsAgentEntityData::MainStation:
		case TA_Base_Core::AtsAgentEntityData::Depot:
			return m_atsTablesBaseAddress;
		
		default:
			return 0;
		}
	}

	unsigned short AtsWatchDogTable::getAtsModbusEndAddress()
	{
		return getAtsModbusStartAddress() + getAtsModbusSize() - 1;
	}

	unsigned short AtsWatchDogTable::getAtsModbusSize()
	{
		// In words
		return (AtsWatchDogTable::getStaticModbusRecordSize(m_locationType));
	}

	unsigned short AtsWatchDogTable::getStaticModbusRecordSize(TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType)
	{
		TA_ASSERT(locationType == TA_Base_Core::AtsAgentEntityData::Occ ||
				  locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
				  locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
				  locationType == TA_Base_Core::AtsAgentEntityData::MainStation ||
				  locationType == TA_Base_Core::AtsAgentEntityData::Depot,
				  "Unknown location type - ATS Watchdog table is valid for OCC, DPT and STATION");

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
}
