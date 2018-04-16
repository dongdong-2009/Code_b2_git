/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_Review_Branch/4669/transactive/bus/ats/ats_agent/CommonATS/src/WatchDogData.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2010/03/30 11:17:43 $
  * Last modified by:  $Author: grace.koh $
  *
  * This represents the ATS Train Table
  */
#ifndef WATCHDOGDATA_H
#define WATCHDOGDATA_H

#include <sstream>

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

namespace TA_Base_Bus
{
	class DataPoint;
}

namespace TA_IRS_Bus
{
    class IAtsStatusObserver;
}

namespace TA_IRS_Bus
{
    class WatchDogData
    {
    public:

		/**
		  * Constructor
		  *
		  */
		WatchDogData(TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType,
					 unsigned long maxWatchdogInterval);

		virtual ~WatchDogData();

		void setNumberOfConnections(int numberOfConnections);
		bool attachDataPoint(TA_Base_Bus::DataPoint* dataPoint);

		enum EConnectionState
		{
			ConnectionUnknown,
			ConnectionWrite,
			ConnectionWriteRead,
			ConnectionRead,
			ConnectionFailed
		};

		void updateConnectionState(int connection, EConnectionState connectionState);

		enum EWatchDogState 
		{
			WatchDogNotRead,
			WatchDogReadOnce,
			WatchDogValid,
			WatchDogNotValid
		};

		EWatchDogState getWatchDogState(int connection);
		void updateWatchDog(int connection, unsigned long chronologicalCounter);
		bool isAtsOk();

		void sendUpdates();

		void updateIsInControlMode( bool isControlMode ); //wenching++ (TD14526)

        void setObserver(IAtsStatusObserver* statusObserver);

	protected:

		bool checkAttribute(std::string address);
		void updateDataPoint();
			
    private:
        // Disable default constructor, copy constructor and assignment operator
		WatchDogData();
        WatchDogData( const WatchDogData& watchDogData);
        WatchDogData& operator=(const WatchDogData &);

	protected:


		struct AtsWatchDog
		{
			unsigned long					chronologicalCounter;
			EWatchDogState					watchDogState;
            EConnectionState				connectionState;
		};

		std::vector<AtsWatchDog>			m_watchDogs;
		int									m_numberOfConnections;
		unsigned long						m_maxWatchdogInterval;

		/** which location this table is catering for */
		TA_Base_Core::AtsAgentEntityData::EAtsLocationType	
											m_locationType;

		TA_Base_Bus::DataPoint*				m_watchDogDp;
		bool								m_atsOk;
		bool								m_isInControlMode; //wenching++ (TD14526)

		static const std::string			ATS_WATCHDOG;

        IAtsStatusObserver*                 m_statusObserver;

		bool								m_modeChangeFlag;	
		time_t								m_modeChangeTime; 

    };
}

#endif
