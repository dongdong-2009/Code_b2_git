/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/ats/AtsAgent/src/TrainTable.cpp $
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

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/AtsAgentException.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "app/ats/AtsAgent/src/AtsAgentRunParams.h"
#include "app/ats/AtsAgent/src/AddressUtil.h"
#include "app/ats/AtsAgent/src/TrainTable.h"

namespace TA_IRS_App
{
	TrainTable::TrainTable(	unsigned long entityKey, 
							unsigned long locationKey, 
							TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType,
							int atsTablesBaseAddress, int iscsTablesBaseAddress,
							DataManager& dataManager)
	: m_atsTablesBaseAddress (atsTablesBaseAddress),
	  m_iscsTablesBaseAddress (iscsTablesBaseAddress),
	  m_locationKey (locationKey), m_locationType (locationType),
	  m_dataManager(dataManager)
	{
	};

	unsigned int TrainTable::getNumberOfAtsRecords()
	{
		switch(m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Occ:
		case TA_Base_Core::AtsAgentEntityData::Depot:
			return 99;
		
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
		case TA_Base_Core::AtsAgentEntityData::MainStation:
			return 20;
		
		default:
			TA_ASSERT(false, "Unknown location type - ATS Train table is valid for OCC, DPT and STATION");
			return 0;
		}
	}

	void TrainTable::importAtsTable(RawTable& rawTable)
	{
		OperationModeManager& operationModeManager = m_dataManager.getOperationModeManager();

		if (operationModeManager.inControlMode() == false)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "importAtsTable() - ignored as agent in Monitor Mode");
			return;
		}

		TA_IRS_Bus::PhysicalTrainNumberType trainId;
		unsigned int numberOfRecords = 0;

		// Clear Update Flags
		m_dataManager.clearTrainImportFlags();
		
		int recordWordOffset;

		for (recordWordOffset = 0; 
		     recordWordOffset < rawTable.length();
			 recordWordOffset += getAtsModbusRecordSize())
		{
			trainId = TA_IRS_Bus::PhysicalTrainNumberType(rawTable.getWord(recordWordOffset*2));

			if (trainId == 0)
			{
				// No more valid records
				break;
			}

			try
			{
				importRecord(rawTable, recordWordOffset*2, trainId);
				
				numberOfRecords++;
			}
			catch (...)
			{
				std::ostringstream message;
				message.clear();
				message << "importAtsTable() - Error: Physical Train ID ";
				message << (unsigned long)trainId;
				message << " out of range";

				LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", message.str());
			}
		}

		// Check to find records that were not updated
		m_dataManager.checkIfTrainNotImported();

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
			"importAtsTable() - TrainTable imported - received %d records", numberOfRecords);
	}

	void TrainTable::importRecord(RawTable& rawTable, unsigned int recordByteOffset, TA_IRS_Bus::PhysicalTrainNumberType trainId)
	{
		unsigned int car;

		AtsEcsData& ecsData = m_dataManager.getEcsData(trainId);
		AtsTrainData& trainData = m_dataManager.getTrainData(trainId);
		AtsCctvData& cctvData = m_dataManager.getCctvData(trainId);

		TA_Base_Core::ThreadGuard ecsGuard(ecsData);
		TA_Base_Core::ThreadGuard trainGuard(trainData);
		TA_Base_Core::ThreadGuard cctvGuard(cctvData);

		//
		// Train identification
		//
		// Physical Train Number and Schedule Number are used in all the MFT GUI 
		// to identify the train clearly. 
		//
		// Notes (valid for all tables): 
		// - Physical Train Number is equivalent to PV Number
		// - Train Number is equivalent to Service number
		//
		
		// <ATC train number>
		ecsData.updateAtcTrainNumber(TA_IRS_Bus::AtcTrainNumberType(rawTable.getWord(recordByteOffset + 2)));
		
		// Localisation Validity
		bool localisationValid = (rawTable.getByte(recordByteOffset + 5) == 1);

		if (localisationValid)
		{
			// <Service number>
			ecsData.updateServiceNumber(TA_IRS_Bus::ServiceNumberType(rawTable.getWord(recordByteOffset + 6)));
			trainData.updateServiceNumber(TA_IRS_Bus::ServiceNumberType(rawTable.getWord(recordByteOffset + 6)));
			cctvData.updateServiceNumber(TA_IRS_Bus::ServiceNumberType(rawTable.getWord(recordByteOffset + 6)));
			
			// <Schedule number>
			ecsData.updateScheduleNumber(TA_IRS_Bus::ScheduleNumberType(rawTable.getWord(recordByteOffset + 8)));
			
			// 
			// Train localisation used for CCTV, Radio and ECS
			//
			// <Fixed Block's Branch ID>
			unsigned long fixedBlocksBranchId = (unsigned long)rawTable.getWord(recordByteOffset + 10);
			
			// <FB abscissa in the branch>
			unsigned long fbAbscissaInTheBranch = (unsigned long)rawTable.getWord(recordByteOffset + 12);
			
			// <Head CAR abscissa on FB>
			unsigned long headCarAbscissaOnFb = (unsigned long)rawTable.getWord(recordByteOffset + 14);
			
			ecsData.updateZoneId(fixedBlocksBranchId, fbAbscissaInTheBranch, headCarAbscissaOnFb);
			trainData.updateZoneId(fixedBlocksBranchId, fbAbscissaInTheBranch, headCarAbscissaOnFb);
			cctvData.updateZoneId(fixedBlocksBranchId, fbAbscissaInTheBranch, headCarAbscissaOnFb);
														
			// <Talkative status>
			trainData.updateMute(rawTable.getByte(recordByteOffset + 18) == 0);

			// <Stalled in inter-station>
			ecsData.updateStalled((rawTable.getByte(recordByteOffset + 23) == 1));

			ecsData.updateLocalisationValidity(true);
			trainData.updateLocalisationValidity(true);
			cctvData.updateLocalisationValidity(true);
		}
		else
		{
			ecsData.updateLocalisationValidity(false);
			trainData.updateLocalisationValidity(false);
			cctvData.updateLocalisationValidity(false);
		}
	
		bool oa1SubsetValid = (rawTable.getByte(recordByteOffset + 27) == 1);

		if (oa1SubsetValid)
		{
			importFireDetectionData(rawTable, recordByteOffset + 36, trainId);
			
			ecsData.updateExternalTemperatureInCelsius((unsigned long)rawTable.getByte(recordByteOffset + 49));

			for (car = 0; car < 3; car++)
			{
				TA_IRS_Bus::IAtsCctvCorbaDef::EFireExtinguisherStatus feStatus
					= TA_IRS_Bus::IAtsCctvCorbaDef::ExtinguisherStatusUnknown;

				// Convert to CORBA type
				switch(rawTable.getByte(recordByteOffset + 50 + car))
				{
				case ExtinguisherStatusUnknown:
					feStatus = TA_IRS_Bus::IAtsCctvCorbaDef::ExtinguisherStatusUnknown;
					break;

				case ExtinguisherStatusAllNormal:
					feStatus = TA_IRS_Bus::IAtsCctvCorbaDef::ExtinguisherStatusAllNormal;
					break;
				
				case ExtinguisherStatusAtLeastOneRemoved:
					feStatus = TA_IRS_Bus::IAtsCctvCorbaDef::ExtinguisherStatusAtLeastOneRemoved;
					break;
				}

				cctvData.updateFireExtinguisherStatus(car, feStatus);
			}
			
			for (car = 0; car < 3; car++)
			{
				unsigned int door, bit;

				for (door = 0, bit = 15; door < 8; door++, bit-=2)
				{
					TA_IRS_Bus::IAtsCctvCorbaDef::EEhsStatus ehsStatus
						= TA_IRS_Bus::IAtsCctvCorbaDef::EhsStatusUnknown;

					// Convert to CORBA type
					switch(rawTable.getBitsFromWord(recordByteOffset + 54 + (car * 2), bit-1, bit))
					{
					case EhsStatusUnknown:
						ehsStatus = TA_IRS_Bus::IAtsCctvCorbaDef::EhsStatusUnknown;
						break;

					case EhsStatusNotActivated:
						ehsStatus = TA_IRS_Bus::IAtsCctvCorbaDef::EhsStatusNotActivated;
						break;
					
					case EhsStatusActivated:
						ehsStatus = TA_IRS_Bus::IAtsCctvCorbaDef::EhsStatusActivated;
						break;
					}

					cctvData.updateEhsStatus(car, door, ehsStatus);
				}
			}

			for (car = 0; car < 2; car++)
			{
				TA_IRS_Bus::IAtsCctvCorbaDef::EDdStatus ddStatus
					= TA_IRS_Bus::IAtsCctvCorbaDef::DdUnknown;

				// Convert to CORBA type
				switch(rawTable.getByte(recordByteOffset + 60 + car))
				{
				case DdUnknown:
					ddStatus = TA_IRS_Bus::IAtsCctvCorbaDef::DdUnknown;
					break;

				case DdClosedAndLocked:
					ddStatus = TA_IRS_Bus::IAtsCctvCorbaDef::DdClosedAndLocked;
					break;
				
				case DdCoverOpen:
					ddStatus = TA_IRS_Bus::IAtsCctvCorbaDef::DdCoverOpen;
					break;

				case DdEvacuationRequired:
					ddStatus = TA_IRS_Bus::IAtsCctvCorbaDef::DdEvacuationRequired;
					break;

				case DdNotLockedOrNotClosed:
					ddStatus = TA_IRS_Bus::IAtsCctvCorbaDef::DdNotLockedOrNotClosed;
					break;
				}

				// Car 1/Car 3 indexes 0/1 respectively
				cctvData.updateDetrainmentDoorStatus(car, ddStatus);
			}

			ecsData.updateOa1Validity(true);
			cctvData.updateOa1Validity(true);
		}
		else
		{
			ecsData.updateOa1Validity(false);
			cctvData.updateOa1Validity(false);
		}

		bool oa2SubsetValid = (rawTable.getByte(recordByteOffset + 63) == 1);

		if (oa2SubsetValid)
		{
			//
			// Data from OA2
			//
			for (car = 0; car < 2; car++)
			{
				TA_IRS_Bus::IAtsCctvCorbaDef::EDriverConsoleCoverStatus dccStatus
					= TA_IRS_Bus::IAtsCctvCorbaDef::ConsoleCoverStatusUnknown;

				// Convert to CORBA type
				switch(rawTable.getByte(recordByteOffset + 64 + car))
				{
				case ConsoleCoverStatusUnknown:
					dccStatus = TA_IRS_Bus::IAtsCctvCorbaDef::ConsoleCoverStatusUnknown;
					break;

				case ConsoleCoverStatusClosed:
					dccStatus = TA_IRS_Bus::IAtsCctvCorbaDef::ConsoleCoverStatusClosed;
					break;
				
				case ConsoleCoverStatusOpen:
					dccStatus = TA_IRS_Bus::IAtsCctvCorbaDef::ConsoleCoverStatusOpen;
					break;
				}

				cctvData.updateDriverConsoleStatus(car, dccStatus);
			}
			

			for (car = 0; car < 3; car++)
			{
				unsigned int index, bit;

				for (unsigned int abDoors = 0; abDoors < 2; abDoors++)
				{
					for (index = 0, bit = 15; index < 4; index++, bit-=4)
					{
						TA_IRS_Bus::IAtsCctvCorbaDef::ETrainDoorStatus tdStatus
							= TA_IRS_Bus::IAtsCctvCorbaDef::TrainDoorStatusUnknown;

						// Convert to CORBA type
						switch(rawTable.getBitsFromWord(recordByteOffset + 66 + (car * 4) + (abDoors*2), bit-3, bit))
						{
						case TrainDoorStatusUnknown:
							tdStatus = TA_IRS_Bus::IAtsCctvCorbaDef::TrainDoorStatusUnknown;
							break;

						case TrainDoorStatusClosedAndLocked:
							tdStatus = TA_IRS_Bus::IAtsCctvCorbaDef::TrainDoorStatusClosedAndLocked;
							break;
						
						case TrainDoorStatusOpened:
							tdStatus = TA_IRS_Bus::IAtsCctvCorbaDef::TrainDoorStatusOpened;
							break;

						case TrainDoorStatusIsolated:
							tdStatus = TA_IRS_Bus::IAtsCctvCorbaDef::TrainDoorStatusIsolated;
							break;

						case TrainDoorStatusPushBack:
							tdStatus = TA_IRS_Bus::IAtsCctvCorbaDef::TrainDoorStatusPushBack;
							break;

						case TrainDoorStatusObstacleDetection:
							tdStatus = TA_IRS_Bus::IAtsCctvCorbaDef::TrainDoorStatusObstacleDetection;
							break;

						case TrainDoorStatusDoorFailureClosed:
							tdStatus = TA_IRS_Bus::IAtsCctvCorbaDef::TrainDoorStatusDoorFailureClosed;
							break;

						case TrainDoorStatusDoorFailureOpen:
							tdStatus = TA_IRS_Bus::IAtsCctvCorbaDef::TrainDoorStatusDoorFailureOpen;
							break;
						}

						cctvData.updateTrainDoorStatus(car, index+(abDoors*4), tdStatus);
					}
				}
			}

			cctvData.updateOa2Validity(true);
		}
		else
		{
			cctvData.updateOa2Validity(false);
		}

		ecsData.setTrainValidity();
		trainData.setTrainValidity();
		cctvData.setTrainValidity();
	}

	void TrainTable::importFireDetectionData(RawTable& rawTable, unsigned int fireDataOffset, TA_IRS_Bus::PhysicalTrainNumberType trainId)
	{	
		bool emergency = false;

		AtsCctvData& cctvData = m_dataManager.getCctvData(trainId);
		AtsEcsData& ecsData = m_dataManager.getEcsData(trainId);

		for (unsigned int car = 0; car < 3; car++)
		{
			{
				TA_IRS_Bus::IAtsCctvCorbaDef::EFireSmokeDetectionStatus usfStatus
					= TA_IRS_Bus::IAtsCctvCorbaDef::FireSmokeDetectionUnknown;

				// Convert to CORBA type
				switch(rawTable.getByte(fireDataOffset + (car*4) + 0))
				{
				case FireSmokeDetectionUnknown:
					usfStatus = TA_IRS_Bus::IAtsCctvCorbaDef::FireSmokeDetectionUnknown;
					break;

				case FireSmokeNotDetected:
					usfStatus = TA_IRS_Bus::IAtsCctvCorbaDef::FireSmokeNotDetected;
					break;
				
				case FireSmokeDetectionFailure:
					usfStatus = TA_IRS_Bus::IAtsCctvCorbaDef::FireSmokeDetectionFailure;
					break;

				case FireSmokeDetected:
					usfStatus = TA_IRS_Bus::IAtsCctvCorbaDef::FireSmokeDetected;
					break;
				}

				cctvData.updateUnderSeatFireStatus(car, usfStatus);
				emergency |= (usfStatus == TA_IRS_Bus::IAtsCctvCorbaDef::FireSmokeDetected);
			}

			{
				TA_IRS_Bus::IAtsCctvCorbaDef::EFireSmokeDetectionStatus uffStatus
					= TA_IRS_Bus::IAtsCctvCorbaDef::FireSmokeDetectionUnknown;

				// Convert to CORBA type
				switch(rawTable.getByte(fireDataOffset + (car*4) + 1))
				{
				case FireSmokeDetectionUnknown:
					uffStatus = TA_IRS_Bus::IAtsCctvCorbaDef::FireSmokeDetectionUnknown;
					break;

				case FireSmokeNotDetected:
					uffStatus = TA_IRS_Bus::IAtsCctvCorbaDef::FireSmokeNotDetected;
					break;
				
				case FireSmokeDetectionFailure:
					uffStatus = TA_IRS_Bus::IAtsCctvCorbaDef::FireSmokeDetectionFailure;
					break;

				case FireSmokeDetected:
					uffStatus = TA_IRS_Bus::IAtsCctvCorbaDef::FireSmokeDetected;
					break;
				}

				cctvData.updateUnderFrameFireStatus(car, uffStatus);
				emergency |= (uffStatus == TA_IRS_Bus::IAtsCctvCorbaDef::FireSmokeDetected);
			}

			{
				TA_IRS_Bus::IAtsCctvCorbaDef::EInteriorSmokeDetectionStatus isdStatus
					= TA_IRS_Bus::IAtsCctvCorbaDef::InteriorSmokeUnknown;

				// Convert to CORBA type
				switch(rawTable.getByte(fireDataOffset + (car*4) + 2))
				{
				case InteriorSmokeUnknown:
					isdStatus = TA_IRS_Bus::IAtsCctvCorbaDef::InteriorSmokeUnknown;
					break;

				case InteriorSmokeNoPreAlarm:
					isdStatus = TA_IRS_Bus::IAtsCctvCorbaDef::InteriorSmokeNoPreAlarm;
					break;

				case InteriorSmokeMinorFault:
					isdStatus = TA_IRS_Bus::IAtsCctvCorbaDef::InteriorSmokeMinorFault;
					break;
				
				case InteriorSmokePreAlarm:
					isdStatus = TA_IRS_Bus::IAtsCctvCorbaDef::InteriorSmokePreAlarm;
					break;

				case InteriorSmokeAlarm:
					isdStatus = TA_IRS_Bus::IAtsCctvCorbaDef::InteriorSmokeAlarm;
					break;
				}

				cctvData.updateInteriorSmokeDetectionStatus(car, isdStatus);
				emergency |= (isdStatus == TA_IRS_Bus::IAtsCctvCorbaDef::InteriorSmokeAlarm);
			}

			{
				TA_IRS_Bus::IAtsCctvCorbaDef::EFireSmokeDetectionStatus esdStatus
					= TA_IRS_Bus::IAtsCctvCorbaDef::FireSmokeDetectionUnknown;

				// Convert to CORBA type
				switch(rawTable.getByte(fireDataOffset + (car*4) + 3))
				{
				case FireSmokeDetectionUnknown:
					esdStatus = TA_IRS_Bus::IAtsCctvCorbaDef::FireSmokeDetectionUnknown;
					break;

				case FireSmokeNotDetected:
					esdStatus = TA_IRS_Bus::IAtsCctvCorbaDef::FireSmokeNotDetected;
					break;
				
				case FireSmokeDetectionFailure:
					esdStatus = TA_IRS_Bus::IAtsCctvCorbaDef::FireSmokeDetectionFailure;
					break;

				case FireSmokeDetected:
					esdStatus = TA_IRS_Bus::IAtsCctvCorbaDef::FireSmokeDetected;
					break;
				}

				cctvData.updateExteriorSmokeDetectionStatus(car, esdStatus);
				emergency |= (esdStatus == TA_IRS_Bus::IAtsCctvCorbaDef::FireSmokeDetected);
			}
		}

		// The isInEmergency flag will be set to true if any of the following
		// fields are set:
		//
		//      <Under Frame Fire Detected in Car 1>
		//      <Under Frame Fire Detected in Car 2>
		//      <Under Frame Fire Detected in Car 3>
		//      <Exterior smoke detected in Car 1>    
		//      <Exterior smoke detected in Car 2>
		//      <Exterior smoke detected in Car 3>
		//      <Interior smoke detected in Car 1>
		//      <Interior smoke detected in Car 2>
		//      <Interior smoke detection in Car 3>
		//      <Under Seat Fire  detected in Car 1>
		//      <Under Seat Fire  detected in Car 2>
		//      <Under Seat Fire  detected in Car 3>

		ecsData.updateEmergency(emergency);
	}


	unsigned short TrainTable::getAtsModbusStartAddress()
	{
		TA_ASSERT(m_locationType == TA_Base_Core::AtsAgentEntityData::Occ || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::MainStation ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::Depot,
				  "Unknown location type - ATS Train table is valid for OCC, DPT and STATION");

		// Taken from SIG-ATS/ISCS PMF
		switch (m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Occ:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
		case TA_Base_Core::AtsAgentEntityData::MainStation:
		case TA_Base_Core::AtsAgentEntityData::Depot:
			return m_atsTablesBaseAddress + 2;
		
		default:
			return 0;
		}
	}

	unsigned short TrainTable::getAtsModbusRecordSize()
	{
		TA_ASSERT(m_locationType == TA_Base_Core::AtsAgentEntityData::Occ || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::MainStation ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::Depot,
				  "Unknown location type - ATS Train table is valid for OCC, DPT and STATION");

		// Taken from SIG-ATS/ISCS SICD and divided by 2 to give words
		switch (m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Occ:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
		case TA_Base_Core::AtsAgentEntityData::MainStation:
		case TA_Base_Core::AtsAgentEntityData::Depot:
			return 96/2;

		default:
			return 0;
		}
	}

	unsigned short TrainTable::getAtsModbusEndAddress()
	{
		return getAtsModbusStartAddress() + getAtsModbusSize() - 1;
	}

	unsigned short TrainTable::getAtsModbusSize()
	{
		// In words
		return (getNumberOfAtsRecords() * getAtsModbusRecordSize());
	}

	bool TrainTable::tableReadAllowed()
	{
		bool readAllowed = false;
		OperationModeManager& operationModeManager = m_dataManager.getOperationModeManager();

		switch (m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Occ:
			readAllowed = true;
			break;

		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
		case TA_Base_Core::AtsAgentEntityData::MainStation:
			readAllowed = (operationModeManager.getAgentState() != OperationModeManager::AgentNormal);
			break;
		
		case TA_Base_Core::AtsAgentEntityData::Depot:
		default:
			readAllowed = true;
			break;
		}

		return readAllowed;
	}

	void TrainTable::exportIscsTable (RawTable& rawTable)
	{
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "exportIscsTable() - TrainTable");

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
			if ((*it).second != NULL &&
				(*it).second->isValidForExport(m_locationKey, m_locationType))
			{
				try
				{
					exportRecord(rawTable,
								 numRecords*getIscsModbusRecordSize()*2,
								 (*it).first);
					numRecords++;
				}
				catch (...)
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", 
								"exportIscsTable() - error while exporting record");
				}
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
			"exportIscsTable() - IscsTrainRecord exported - %d records", numRecords);
	}

	void TrainTable::exportRecord (RawTable& rawTable, unsigned int recordByteOffset,
								   TA_IRS_Bus::PhysicalTrainNumberType physicalTrainNumber)
	{
		AtsTrainData& trainData = m_dataManager.getTrainData(physicalTrainNumber);
		TA_Base_Core::ThreadGuard guard(trainData);		
		
		unsigned int i;

		// <Physical Train number>
		rawTable.putWord(recordByteOffset + 0, (unsigned short)physicalTrainNumber);

		// Spare Bytes
		rawTable.putByte(recordByteOffset + 2, 0);

		// <Validity field>
		if (trainData.isOa1Valid())
		{
			rawTable.putByte(recordByteOffset + 3, 1);
		}
		else
		{
			rawTable.putByte(recordByteOffset + 3, 0);
		}


		// OA1 table
		for (i = 0; i < 24; i++)
		{
			rawTable.putByte(recordByteOffset + 4 + i, (unsigned char)trainData.getOa1Data(i));
		}
	}

	unsigned short TrainTable::getIscsModbusStartAddress()
	{
		TA_ASSERT(m_locationType == TA_Base_Core::AtsAgentEntityData::Occ || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::MainStation ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::Depot,
				  "Unknown location type - ISCS Train table is valid for OCC, DPT and STATION");

		// Taken from SIG-ATS/ISCS PMF
		switch (m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Occ:
			return m_iscsTablesBaseAddress + 2;

		case TA_Base_Core::AtsAgentEntityData::MainStation:
			return m_iscsTablesBaseAddress + 2;

		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
			return m_iscsTablesBaseAddress + 1002;

		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
			return m_iscsTablesBaseAddress + 2002;
		
		case TA_Base_Core::AtsAgentEntityData::Depot:
			return m_iscsTablesBaseAddress + 2;

		default:
			return 0;
		}
	}

	unsigned short TrainTable::getIscsModbusRecordSize()
	{
		TA_ASSERT(m_locationType == TA_Base_Core::AtsAgentEntityData::Occ || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::MainStation ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::Depot,
				  "Unknown location type - ISCS Train table is valid for OCC, DPT and STATION");

		// Taken from SIG-ATS/ISCS SICD and divided by 2 to give words
		switch (m_locationType)
		{
		case TA_Base_Core::AtsAgentEntityData::Occ:
			return 28/2;

		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
		case TA_Base_Core::AtsAgentEntityData::MainStation:
			return 28/2;
		
		case TA_Base_Core::AtsAgentEntityData::Depot:
			return 28/2;

		default:
			return 0;
		}
	}

	unsigned short TrainTable::getIscsModbusEndAddress()
	{
		return getIscsModbusStartAddress() + getIscsModbusSize() - 1;
	}

	unsigned short TrainTable::getIscsModbusSize()
	{
		// In words
		return (getNumberOfIscsRecords() * getIscsModbusRecordSize());
	}

	unsigned int TrainTable::getNumberOfIscsRecords()
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
			TA_ASSERT(false, "Unknown location type - ATS Train table is valid for OCC, DPT and STATION");
			return 0;
		}
	}
		
	bool TrainTable::tableWriteAllowed()
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
			break;
		}

		return writeAllowed;
	}
}
