/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File$
  * @author:   Darren Sampson
  * @version:  $Revision$
  *
  * Last modification: $DateTime$
  * Last mofified by:  $Author$
  *
  * This represents the ATS Train Table
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/AtsAgentException.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "app/ats/AtsAgent/src/AtsAgentRunParams.h"
#include "app/ats/AtsAgent/src/AddressUtil.h"
#include "app/ats/AtsAgent/src/PlatformTable.h"

namespace TA_IRS_App
{
	PlatformTable::PlatformTable(	unsigned long entityKey, 
									unsigned long locationKey, 
									TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType,
									int atsTablesBaseAddress, int iscsTablesBaseAddress,
									DataManager& dataManager)
	: m_locationType (locationType)
	, m_atsTablesBaseAddress (atsTablesBaseAddress)
	, m_iscsTablesBaseAddress (iscsTablesBaseAddress)
	, m_dataManager (dataManager)
	{
		TA_ASSERT(locationType == TA_Base_Core::AtsAgentEntityData::Occ || 
				  locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
				  locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
				  locationType == TA_Base_Core::AtsAgentEntityData::MainStation,
					"ATS Platform Table only valid at OCC or STATION");
	};

	unsigned int PlatformTable::getNumberOfAtsRecords()
	{
		switch(m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Occ:
			return 100;
		
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
		case TA_Base_Core::AtsAgentEntityData::MainStation:
			return 20;
		
		case TA_Base_Core::AtsAgentEntityData::Depot:
		default:
			TA_ASSERT(false, "Unknown location type - ISCS Platform Table only valid at OCC or STATION");
			return 0;
		}
	}

	void PlatformTable::importAtsTable(RawTable& rawTable)
	{
		OperationModeManager& operationModeManager = m_dataManager.getOperationModeManager();

		if (operationModeManager.inControlMode() == false)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "importAtsTable() - ignored as agent in Monitor Mode");
			return;
		}

		TA_IRS_Bus::PlatformIdType platformId;
		unsigned int numberOfRecords = 0;

		int recordWordOffset;

		for (recordWordOffset = 0; 
		     recordWordOffset < rawTable.length();
			 recordWordOffset += getAtsModbusRecordSize())
		{
			platformId = TA_IRS_Bus::PlatformIdType(rawTable.getWord(recordWordOffset*2));		// <Platform area ID>

			if (platformId == 0)
			{
				// No more valid data to read
				break;
			}

			try
			{
				importRecord(rawTable, recordWordOffset*2, platformId);

				numberOfRecords++;
			}
			catch (...)
			{
				std::ostringstream text;
				text << "importAtsTable() - Platform Record with Platform ID ";
				text << (unsigned long)platformId;
				text << " not found";

				LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", text.str());
			}
		}

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
			"importAtsTable() - PlatformTable imported - received %d records", numberOfRecords);
	}

	void PlatformTable::importRecord(RawTable& rawTable, unsigned int recordByteOffset,
									 TA_IRS_Bus::PlatformIdType platformId)
	{
		OperationModeManager& operationModeManager = m_dataManager.getOperationModeManager();

		AtsTisData& tisData = m_dataManager.getTisData(platformId);
		AtsPasData& pasData = m_dataManager.getPasData(platformId);

		TA_Base_Core::ThreadGuard tisGuard(tisData);
		TA_Base_Core::ThreadGuard pasGuard(pasData);

		if ((m_locationType != TA_Base_Core::AtsAgentEntityData::SecondaryStation1 && 
			 m_locationType != TA_Base_Core::AtsAgentEntityData::SecondaryStation2 &&
			 m_locationType != TA_Base_Core::AtsAgentEntityData::MainStation) ||
			operationModeManager.getAgentState() == OperationModeManager::AgentIscsDegraded ||
			operationModeManager.getAgentState() == OperationModeManager::AgentOccFailure)
		{	
            // <Skip>
            // Bohong modified for CL-20887, we just move the extraction of skip-byte to the 1st place.
            // And also we need to modify function pasData.updatePreArrival, to judge wether skip-byte is 
            // set to 1 or not.
            tisData.updateSkip(rawTable.getByte(recordByteOffset + 6) == 1);
            pasData.updateSkip(rawTable.getByte(recordByteOffset + 6) == 1);			

			// <Pre-arrival>
			tisData.updatePreArrival(rawTable.getByte(recordByteOffset + 2) == 1);
			pasData.updatePreArrival(rawTable.getByte(recordByteOffset + 2) == 1);

			// <Arrival>
			tisData.updateArrival(rawTable.getByte(recordByteOffset + 3) == 1);
			pasData.updateArrival(rawTable.getByte(recordByteOffset + 3) == 1);
		
			// <Pre-departure>
			tisData.updatePreDeparture(rawTable.getByte(recordByteOffset + 4) == 1);
			pasData.updatePreDeparture(rawTable.getByte(recordByteOffset + 4) == 1);
		
			// <Departure>
			tisData.updateDeparture(rawTable.getByte(recordByteOffset + 5) == 1);
			pasData.updateDeparture(rawTable.getByte(recordByteOffset + 5) == 1);

			// <Hold>
			tisData.updateHold(rawTable.getByte(recordByteOffset + 7) == 1);
			pasData.updateHold(rawTable.getByte(recordByteOffset + 7) == 1);
		
			// <Evacuation>
             tisData.updateEvacuation(rawTable.getByte(recordByteOffset + 8) == 1); //bihui-- CL-22084

            // pasData.updateEvacuation(rawTable.getByte(recordByteOffset + 8) == 1);
            pasData.updateLastTrainOfOperatingDay( rawTable.getByte(recordByteOffset + 8) == 1 ); //limin++, ECP-657 CL-21010
				
			// <Train overrun>
			tisData.updateTrainOverrun(rawTable.getByte(recordByteOffset + 9) == 1);
			pasData.updateTrainOverrun(rawTable.getByte(recordByteOffset + 9) == 1);

			// Train Info
			for (unsigned int i = 0; i < 2; i++)
			{
				importTrainInfo(rawTable, recordByteOffset + 10 + (i*22), i, platformId);
			}
		}

		if (m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
			m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
			m_locationType == TA_Base_Core::AtsAgentEntityData::MainStation)
		{
			AtsPsdData& psdData = m_dataManager.getPsdData(platformId);
			TA_Base_Core::ThreadGuard psdGuard(psdData);

			// <PSD close pre-warning>
			psdData.updatePsdClosePreWarning(rawTable.getByte(recordByteOffset + 54) == 0);

			// <PSD closed and locked>
			psdData.updatePsdClosedAndLocked(rawTable.testBitInWord(recordByteOffset + 54, 0) &&
												   rawTable.testBitInWord(recordByteOffset + 54, 1));

			// <PSD door inhibition status>
			unsigned int index, bit;
			for (index = 0, bit = 13; index < 12; index++, bit--)
			{
				// Skip blank bits..
				if (index == 6)
				{
					bit = 5;
				}

				psdData.updatePsdInhibitionStatus(index, !rawTable.testBitInWord(recordByteOffset + 56, bit));
			}
		}
	}

	void PlatformTable::importTrainInfo(RawTable& rawTable, unsigned int trainInfoOffset, unsigned int trainIndex,
										TA_IRS_Bus::PlatformIdType platformId)
	{	
		// Validity
		bool trainInfoValid = (rawTable.getByte(trainInfoOffset + 0) == 1);

		AtsTisData& tisData = m_dataManager.getTisData(platformId);
		AtsPasData& pasData = m_dataManager.getPasData(platformId);

		//if (trainInfoValid)
		{
			// <Platform Type>
			tisData.updatePlatformType(trainIndex, rawTable.getByte(trainInfoOffset + 1) == 0);

			// <Physical Train number>
			tisData.updatePhysicalTrainNumber(trainIndex, TA_IRS_Bus::PhysicalTrainNumberType(rawTable.getWord(trainInfoOffset + 2)));

			// <Service number>
			tisData.updateServiceNumber(trainIndex, TA_IRS_Bus::ServiceNumberType(rawTable.getWord(trainInfoOffset + 4)));
			
			//TD 16335
			//zhou yuan++
			// <Destination area ID>
			unsigned long platformId = rawTable.getWord(trainInfoOffset + 6);
			unsigned long stationId = m_dataManager.getStationIdByPlatformId(platformId);
			
			tisData.updateDestinationStationId( trainIndex, TA_IRS_Bus::StationIdType(stationId) );
			if (trainIndex == 0)
			{
				pasData.updateDestinationStationId( TA_IRS_Bus::StationIdType(stationId) );
			}
			//++zhouyuan
		
			// <Last train of the operating day>
			tisData.updateLastTrainOfOperatingDay(trainIndex, rawTable.getByte(trainInfoOffset + 9) == 1);
            // limin--, ECP-657 CL-21010
            // if (trainIndex == 0)
            // {
            //     pasData.updateLastTrainOfOperatingDay(rawTable.getByte(trainInfoOffset + 9) == 1);
            // }
            // --limin, ECP-657 CL-21010

			TA_IRS_Bus::TimeStampType scheduledArrival;
				
			scheduledArrival.year = rawTable.getWord(trainInfoOffset + 10);
			scheduledArrival.month = rawTable.getByte(trainInfoOffset + 13);
			scheduledArrival.day = rawTable.getByte(trainInfoOffset + 15);
			scheduledArrival.hour = rawTable.getByte(trainInfoOffset + 17);
			scheduledArrival.minute = rawTable.getByte(trainInfoOffset + 19);
			scheduledArrival.second = rawTable.getByte(trainInfoOffset + 21);

			tisData.updateScheduledArrival(trainIndex, scheduledArrival);

            if (trainInfoValid) // TD20559 LC ++
            {
			    tisData.updateTrainInfoValidity(trainIndex, true);
            
			    if (trainIndex == 0)
			    {
				    pasData.updateTrainInfoValidity(true);
			    }
            }
            else
		    {
			    tisData.updateTrainInfoValidity(trainIndex, false);
			    if (trainIndex == 0)
			    {
				    pasData.updateTrainInfoValidity(false);
			    }
            }  // TD20559 LC ++
		}
		
		//fwc modify for TD17754
		/*else
		{
			tisData.updateTrainInfoValidity(trainIndex, false);
			if (trainIndex == 0)
			{
				pasData.updateTrainInfoValidity(false);
			}
		}*/
	}

	unsigned short PlatformTable::getAtsModbusStartAddress()
	{
		TA_ASSERT(m_locationType == TA_Base_Core::AtsAgentEntityData::Occ || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::MainStation,
				  "ATS Platform Table only valid at OCC or STATION");

		// Taken from SIG-ATS/ISCS PMF
		switch (m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Occ:
			return m_atsTablesBaseAddress + 9506;

		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
		case TA_Base_Core::AtsAgentEntityData::MainStation:
			return m_atsTablesBaseAddress + 1922;
		
		case TA_Base_Core::AtsAgentEntityData::Depot:
		default:
			return 0;
		}
	}

	unsigned short PlatformTable::getAtsModbusRecordSize()
	{
		TA_ASSERT(m_locationType == TA_Base_Core::AtsAgentEntityData::Occ || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::MainStation,
				  "ATS Platform Table only valid at OCC or STATION");

		// Taken from SIG-ATS/ISCS SICD and divided by 2 to give words
		switch (m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Occ:
			return 54/2;

		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
		case TA_Base_Core::AtsAgentEntityData::MainStation:
			return 58/2;
		
		case TA_Base_Core::AtsAgentEntityData::Depot:
		default:
			return 0;
		}
	}

	unsigned short PlatformTable::getAtsModbusEndAddress()
	{
		return getAtsModbusStartAddress() + getAtsModbusSize() - 1;
	}

	unsigned short PlatformTable::getAtsModbusSize()
	{
		// In words
		return (getNumberOfAtsRecords() * getAtsModbusRecordSize());
	}

	bool PlatformTable::tableReadAllowed()
	{
		return true;
	}

	void PlatformTable::exportIscsTable (RawTable& rawTable)
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

		DataManager::PsdDataMap& psdDataMap
			= m_dataManager.getPsdDataMap();

		if (rawTable.length() < (signed int)(psdDataMap.size()*getIscsModbusRecordSize()))
		{
			std::ostringstream message;
			message.clear();
			message << "exportIscsTable() - Error: Size of raw table is less than the minimum size of the table - raw table ";
			message << rawTable.length();
			message << ", table ";
			message << psdDataMap.size()*
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

		DataManager::PsdDataMapIt it;
		unsigned int numRecords;
		
		for (it = psdDataMap.begin(), numRecords = 0; 
			 it != psdDataMap.end() && numRecords < getNumberOfIscsRecords();
			 it++)
		{
			if ((*it).second != NULL)
			{
				exportRecord(rawTable,
							 numRecords*getIscsModbusRecordSize()*2,
							 *((*it).second));
				numRecords++;
			}
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
				"exportIscsTable() - PlatformTable exported - %d records", numRecords);
	}

	void PlatformTable::exportRecord (RawTable& rawTable, unsigned int recordByteOffset,
									  AtsPsdData& psdData)
	{
		// <Platform area ID>
		rawTable.putWord(recordByteOffset + 0, (unsigned short)psdData.getPlatformId());

		// Check for open/close requests
	
		// <Close Request>
		rawTable.putByte(recordByteOffset + 2, psdData.getPsdCloseRequest()?1:0);
		
		// <Open Request>
		rawTable.putByte(recordByteOffset + 3, psdData.getPsdOpenRequest()?1:0);

		// <PSD door status> Doors 1-12 indexes 0-11 respectively
		unsigned int index, bit;
		for (index = 0, bit = 13; index < 6; index++, bit--)
		{
			rawTable.putBitToWord(recordByteOffset + 4, bit, psdData.getDoorStatus(index));
		}

		for (index = 6, bit = 5; index < 12; index++, bit--)
		{
			rawTable.putBitToWord(recordByteOffset + 4, bit, psdData.getDoorStatus(index));
		}
	}

	unsigned short PlatformTable::getIscsModbusStartAddress()
	{
		TA_ASSERT(m_locationType == TA_Base_Core::AtsAgentEntityData::Occ || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::MainStation,
				  "ISCS Platform Table only valid at OCC or STATION");

		// Taken from SIG-ATS/ISCS PMF
		switch (m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Occ:
			return m_iscsTablesBaseAddress + 2802;

		case TA_Base_Core::AtsAgentEntityData::MainStation:
			return m_iscsTablesBaseAddress + 282;

		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
			return m_iscsTablesBaseAddress + 1282;

		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
			return m_iscsTablesBaseAddress + 2282;
		
		case TA_Base_Core::AtsAgentEntityData::Depot:
		default:
			return 0;
		}
	}

	unsigned short PlatformTable::getIscsModbusRecordSize()
	{
		TA_ASSERT(m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::MainStation,
				  "ISCS Platform Table only valid at STATION");

		// Taken from SIG-ATS/ISCS SICD and divided by 2 to give words
		switch (m_locationType)
		{
			return 6/2;

		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
		case TA_Base_Core::AtsAgentEntityData::MainStation:
			return 6/2;
		
		case TA_Base_Core::AtsAgentEntityData::Occ:
		case TA_Base_Core::AtsAgentEntityData::Depot:
		default:
			return 0;
		}
	}

	unsigned short PlatformTable::getIscsModbusEndAddress()
	{
		return getIscsModbusStartAddress() + getIscsModbusSize() - 1;
	}

	unsigned short PlatformTable::getIscsModbusSize()
	{
		// In words
		return (getNumberOfIscsRecords() * getIscsModbusRecordSize());
	}

	unsigned int PlatformTable::getNumberOfIscsRecords()
	{
		switch(m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
		case TA_Base_Core::AtsAgentEntityData::MainStation:
			return 10;
		
		case TA_Base_Core::AtsAgentEntityData::Occ:
		case TA_Base_Core::AtsAgentEntityData::Depot:
		default:
			TA_ASSERT(false, "Unknown location type - ISCS Platform Table only valid at STATION");
			return 0;
		}
	}

	bool PlatformTable::tableWriteAllowed()
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
			writeAllowed = true;
			break;
		
		case TA_Base_Core::AtsAgentEntityData::Depot:
		default:
			writeAllowed = false;
		}

		return writeAllowed;
	}
}
