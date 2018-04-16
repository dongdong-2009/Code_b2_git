/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/ats/AtsAgent/src/TrainTable.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last mofified by:  $Author: lim.cy $
  *
  * This represents the ATS Train Table
  */
#ifndef TRAINTABLE_H
#define TRAINTABLE_H

#include <map>

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/data_access_interface/ats/src/IAtsFixedBlockMap.h"

#include "bus/ats/ats_agent/IDL/src/AtsCommonCorbaDef.h"
#include "bus/ats/ats_agent/IDL/src/IAtsTrainCorbaDef.h"

#include "app/ats/AtsAgent/src/IAtsTable.h"
#include "app/ats/AtsAgent/src/IIscsTable.h"

#include "app/ats/AtsAgent/src/DataManager.h"

namespace TA_IRS_App
{
    class TrainTable : public IAtsTable, public IIscsTable
    {
    public:
		/**
		  * Constructor
		  *
		  */
		TrainTable(unsigned long entityKey,
				   unsigned long locationKey, TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType,
				   int atsTablesBaseAddress, int iscsTablesBaseAddress,
				   DataManager& dataManager);

		static std::string getStaticType()
		{
			return "TrainTable";
		};

		//
		// ITable Interface implementation
		//		
		void			importAtsTable(RawTable& rawTable);
		unsigned int	getNumberOfAtsRecords();
		unsigned short	getAtsModbusStartAddress();
		unsigned short	getAtsModbusEndAddress();
		bool			tableReadAllowed();
		
		void			exportIscsTable(RawTable& rawTable);
		unsigned int	getNumberOfIscsRecords();
		unsigned short	getIscsModbusStartAddress();
		unsigned short	getIscsModbusEndAddress();
		bool			tableWriteAllowed();

	protected:

		void importRecord(RawTable& rawTable, unsigned int recordByteOffset, 
						  TA_IRS_Bus::PhysicalTrainNumberType trainId);
		void importFireDetectionData(RawTable& rawTable, unsigned int fireDataOffset, 
									 TA_IRS_Bus::PhysicalTrainNumberType trainId);

		unsigned short	getAtsModbusRecordSize();
		unsigned short	getAtsModbusSize();

		void exportRecord (RawTable& rawTable, unsigned int recordByteOffset,
						   TA_IRS_Bus::PhysicalTrainNumberType physicalTrainNumber);

		unsigned short	getIscsModbusRecordSize();
		unsigned short	getIscsModbusSize();

    private:
        // Disable defult constructor, copy constructor and assignment operator
		TrainTable();
        TrainTable( const TrainTable& atsAgent);
        TrainTable& operator=(const TrainTable &);

	protected:

		//
		// Data from OA1
		//
		enum EFireSmokeDetectionStatus
		{
			FireSmokeDetectionUnknown			= 0,
			FireSmokeNotDetected				= 1,
			FireSmokeDetectionFailure			= 2,
			FireSmokeDetected					= 3
		};

		enum EInteriorSmokeDetectionStatus
		{
			InteriorSmokeUnknown				= 0,
			InteriorSmokeNoPreAlarm				= 1,
			InteriorSmokeMinorFault				= 2,
			InteriorSmokePreAlarm				= 3,
			InteriorSmokeAlarm					= 4
		};

		enum EFireExtinguisherStatus
		{
			ExtinguisherStatusUnknown			= 0,
			ExtinguisherStatusAllNormal			= 1,
			ExtinguisherStatusAtLeastOneRemoved	= 2
		};

		enum EEhsStatus
		{
			EhsStatusUnknown					= 0,
			EhsStatusNotActivated				= 1,
			EhsStatusActivated					= 2
		};

		enum EDdStatus
		{
			DdUnknown							= 0,
			DdClosedAndLocked					= 1,
			DdCoverOpen							= 2,
			DdEvacuationRequired				= 3,
			DdNotLockedOrNotClosed				= 4,
		};

		//
		// Data from OA2
		//
		enum EDriverConsoleCoverStatus
		{
			ConsoleCoverStatusUnknown			= 0,
			ConsoleCoverStatusClosed			= 1,
			ConsoleCoverStatusOpen				= 2
		};

		enum ETrainDoorStatus
		{
			TrainDoorStatusUnknown				= 0,
			TrainDoorStatusClosedAndLocked		= 1,
			TrainDoorStatusOpened				= 2,
			TrainDoorStatusPushBack				= 3,
			TrainDoorStatusObstacleDetection	= 4,
			TrainDoorStatusDoorFailureClosed	= 5,
			TrainDoorStatusDoorFailureOpen		= 6,
			TrainDoorStatusIsolated				= 7,
		};

		/** Base address for the tables */
		int									m_atsTablesBaseAddress;
		int									m_iscsTablesBaseAddress;


		/** which location this table is catering for */
		unsigned long						m_locationKey;
		TA_Base_Core::AtsAgentEntityData::EAtsLocationType	
											m_locationType;

		DataManager&						m_dataManager;
    };
}

#endif // !defined(TRAINTABLE_H)
