/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/AtsAgent/src/TrainTable.cpp $
  * @author:   Darren Sampson
  * @version:  $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/07/16 16:58:57 $
  * Last mofified by:  $Author: peter.wong $
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
			return 150; //based on the ICD B.3.1.2
		
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation1:
		case TA_Base_Core::AtsAgentEntityData::SecondaryStation2:
		case TA_Base_Core::AtsAgentEntityData::MainStation:
			//return 20;//DTL 203
			return 16; //Lucky
		
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
		
		// Log Train Table data
		std::string   hexText = "";
		std::string   consoleText = "";
		rawTable.getLogText(hexText,consoleText,recordByteOffset, getAtsModbusRecordSize() );
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"{ImportRecord}  Train Table Data : %s ", hexText.c_str());

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
		unsigned short trainID = rawTable.getAsciiNumber ( recordByteOffset+0 , 2 ) ;
		unsigned long carNum1 = rawTable.getAsciiNumber ( recordByteOffset+2 , 3 ) ;
		unsigned long carNum2 = rawTable.getAsciiNumber ( recordByteOffset+5 , 3 ) ;
		unsigned long carNum3 = rawTable.getAsciiNumber ( recordByteOffset+8, 3 ) ;
		trainData.updateCarNum1 ( carNum1 ) ;
        trainData.updateCarNum2 ( carNum2 ) ;
		trainData.updateCarNum3 ( carNum3 ) ;

		unsigned short carSpare = rawTable.getAsciiNumber ( recordByteOffset+11, 1 ) ;

		
		
		// <ATC train number>
		ecsData.updateAtcTrainNumber(TA_IRS_Bus::AtcTrainNumberType(trainID));
		
		// Localisation Validity
	//	bool localisationValid = (rawTable.getByte(recordByteOffset + 5) == 1);

        bool localisationValid = true ; //hardcode as true , waiting for confirmation
		unsigned short serviceNumber;
		unsigned short scheduleNumber;
		unsigned short spareTrainData;
		unsigned short trackCircuitArm;
		unsigned short trackCircuitNumber;
		
		unsigned short trackCircuitOffSet;
		unsigned short headTrainDirection;
		unsigned short headTrainStopStatus;

		if (localisationValid)
		{
			// <Service number>
			//unsigned short serviceNumber = rawTable.getWord ( recordByteOffset+14 ) ;
			//lucky
		   serviceNumber = rawTable.getAsciiNumber ( recordByteOffset+12, 2 ) ;
           trainData.updateServiceNumber ( serviceNumber ) ;
		   ecsData.updateServiceNumber( serviceNumber );
		   //unsigned short scheduleNumber  = rawTable.getWord ( recordByteOffset + 16 ) ;
		   //added by lucky
			scheduleNumber = rawTable.getAsciiNumber ( recordByteOffset+14, 2 ) ;
           ecsData.updateScheduleNumber ( scheduleNumber ) ;
		//	ecsData.updateServiceNumber(TA_IRS_Bus::ServiceNumberType(rawTable.getWord(recordByteOffset + 6)));
		//	trainData.updateServiceNumber(TA_IRS_Bus::ServiceNumberType(rawTable.getWord(recordByteOffset + 6)));
		//	cctvData.updateServiceNumber(TA_IRS_Bus::ServiceNumberType(rawTable.getWord(recordByteOffset + 6)));
			
			// <Schedule number>
			//ecsData.updateScheduleNumber(TA_IRS_Bus::ScheduleNumberType(rawTable.getWord(recordByteOffset + 8)));
			
			// 
			// Train localisation used for CCTV, Radio and ECS
			//
			// <Fixed Block's Branch ID>
		    //unsigned short armId = rawTable.getByte ( recordByteOffset + 19 ) ;//bihui TBD
			
		    spareTrainData = rawTable.getAsciiNumber( recordByteOffset+16, 1);//lucky
		    trackCircuitArm = rawTable.getAsciiNumber( recordByteOffset+17, 1); //lucky
			//unsigned short branchId = rawTable.getWord ( recordByteOffset+20 ) ;
			trackCircuitNumber = rawTable.getAsciiNumber( recordByteOffset+18, 2); //lucky
			//unsigned short offSet = rawTable.getWord ( recordByteOffset +22 ) ;
			trackCircuitOffSet = rawTable.getAsciiNumber( recordByteOffset+20, 2);
			
			headTrainDirection =  rawTable.getAsciiNumber( recordByteOffset+22, 1);
			headTrainStopStatus = rawTable.getAsciiNumber( recordByteOffset+23, 1);
			

            //unsigned long headCarAbscissaOnFb = 0 ; 
	//		unsigned long fixedBlocksBranchId = (unsigned long)rawTable.getWord(recordByteOffset + 10);
			
			// <FB abscissa in the branch>
	//		unsigned long fbAbscissaInTheBranch = (unsigned long)rawTable.getWord(recordByteOffset + 12);
			
			// <Head CAR abscissa on FB>
		//	unsigned long headCarAbscissaOnFb = (unsigned long)rawTable.getWord(recordByteOffset + 14);

			ecsData.updateZoneId(trackCircuitNumber, trackCircuitOffSet, trackCircuitArm );
			trainData.updateZoneId(trackCircuitNumber, trackCircuitOffSet, trackCircuitArm);
		//	cctvData.updateZoneId(fixedBlocksBranchId, fbAbscissaInTheBranch, headCarAbscissaOnFb);
														
			// <Talkative status>
		//	trainData.updateMute(rawTable.getByte(recordByteOffset + 18) == 0);

			// <Stalled in inter-station>
			ecsData.updateStalled(headTrainStopStatus);

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
		//unsigned char pecStatus1 = rawTable.getByte ( recordByteOffset + 26 ) ;
		unsigned short pecStatus1 = rawTable.getAsciiNumber( recordByteOffset+24, 1) ; //Lucky
        //unsigned char pecStatus2 = rawTable.getByte ( recordByteOffset + 27 ) ;
		unsigned short pecStatus2 = rawTable.getAsciiNumber( recordByteOffset+25, 1) ; //Lucky
		//unsigned char pecStatus3 = rawTable.getByte ( recordByteOffset + 28 ) ;
		unsigned short pecStatus3 = rawTable.getAsciiNumber( recordByteOffset+26, 1) ; //Lucky

		unsigned short sparePECStatus = rawTable.getAsciiNumber( recordByteOffset+27, 1) ; //Lucky
		unsigned short fireExteriorState = rawTable.getAsciiNumber( recordByteOffset+28, 1) ; //Lucky
		unsigned short fireInteriorState = rawTable.getAsciiNumber( recordByteOffset+29, 1) ; //Lucky
		unsigned short notUsedTemp = rawTable.getAsciiNumber( recordByteOffset+30, 1) ; //Lucky
		unsigned short externalTemp = rawTable.getAsciiNumber( recordByteOffset+31, 1) ; //Lucky
		unsigned short yearTimeStamp = rawTable.getAsciiNumber( recordByteOffset+32, 2) ; //Lucky
		unsigned short monthTimeStamp = rawTable.getAsciiNumber( recordByteOffset+34, 2) ; //Lucky
		unsigned short dayTimeStamp = rawTable.getAsciiNumber( recordByteOffset+36, 2) ; //Lucky
		unsigned short hourTimeStamp = rawTable.getAsciiNumber( recordByteOffset+38, 2) ; //Lucky
		unsigned short minutesTimeStamp = rawTable.getAsciiNumber( recordByteOffset+40, 2) ; //Lucky
		unsigned short secondsTimeStamp = rawTable.getAsciiNumber( recordByteOffset+42, 2) ; //Lucky



		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"\n\t\t\tTrain Table Data : %s\n\n"
				"\t\t\tTrain Identification Number : %X\n"
				"\t\t\tPhysical Car Number#1 : %X\n"
				"\t\t\tPhysical Car Number#2 : %X\n"
				"\t\t\tPhysical Car Number#3 : %X\n"
				"\t\t\tService Number        : %X\n"
				"\t\t\tSchedule Number       : %X\n"
				"\t\t\tSpare                 : %X\n"
				"\t\t\tTrack circuit Arm Identifier  : %X\n"
				"\t\t\tTrack circuit Number          : %X\n"
				"\t\t\tTrack circuit Offset          : %X\n"
				"\t\t\tHead of the train Direction of travel  : %X\n"
				"\t\t\tStopped Status                         : %X\n"
				"\t\t\tPEC Status Car#1      : %X\n"
				"\t\t\tPEC Status Car#2      : %X\n"
				"\t\t\tPEC Status Car#3      : %X\n"
				"\t\t\tSpare (byte)          : %X\n"
				"\t\t\tFire Exterior State   : %X\n"
				"\t\t\tFire Interior State   : %X\n"
				"\t\t\tNot Used              : %X\n"
				"\t\t\tExternal Temperature  : %X\n"
				"\t\t\tYear Time Stamp       : %X\n"
				"\t\t\tMonth Time Stamp      : %X\n"
				"\t\t\tDay Time Stamp        : %X\n"
				"\t\t\tHour Time Stamp       : %X\n"
				"\t\t\tMinutes Time Stamp    : %X\n"
				"\t\t\tSeconds Time Stamp    : %X\n"
				,hexText.c_str(),trainID,carNum1,carNum2,carNum3,serviceNumber,scheduleNumber,spareTrainData
				,trackCircuitArm,trackCircuitNumber,trackCircuitOffSet,headTrainDirection,headTrainStopStatus
				,pecStatus1,pecStatus2,pecStatus3,sparePECStatus,fireExteriorState,fireInteriorState,notUsedTemp
				,externalTemp,yearTimeStamp,monthTimeStamp,dayTimeStamp,hourTimeStamp,minutesTimeStamp,secondsTimeStamp);

	
		
		
		//bool oa1SubsetValid = (rawTable.getByte(recordByteOffset + 27) == 1);

		bool oa1SubsetValid = true ; //bihui TBD

		if (oa1SubsetValid)
		{
			unsigned char fireStatus = ( fireExteriorState | fireInteriorState ); /*rawTable.getByte ( recordByteOffset+ 30 );*/

			ecsData.updateEmergency ( fireStatus ==1 ) ;
			double dbExtendTemp = (externalTemp&0xff)*0.1 + 25.;
           ecsData.updateExternalTemperatureInCelsius(dbExtendTemp);

		   ecsData.updateOa1Validity(true);
		}
		else
		{
            ecsData.updateOa1Validity(false);
		}


		

		ecsData.setTrainValidity();
		trainData.setTrainValidity();
	//	cctvData.setTrainValidity();
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
			return 44/2;

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
			//readAllowed = (operationModeManager.getAgentState() != OperationModeManager::AgentNormal);
			readAllowed = true;
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
