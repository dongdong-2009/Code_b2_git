/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/ats/AtsAgent/src/WatchDogData.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last mofified by:  $Author: lim.cy $
  *
  * This represents the ATS Train Table
  */
#ifndef WATCHDOGDATA_H
#define WATCHDOGDATA_H

#include <sstream>

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

#include "core/timers/src/ITimeoutCallback.h"
#include "core/timers/src/SingletonTimerUtil.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Bus
{
	class DataPoint;
}

namespace TA_IRS_App
{
    class IAtsStatusObserver;
}

namespace TA_IRS_App
{
	class WatchDogData : TA_Base_Core::ITimeoutCallback
    {
    public:

		/**
		  * Constructor
		  *
		  */
		WatchDogData(TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType,
					 unsigned long maxWatchdogInterval, unsigned long connFailedTimeout);

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

		void startConnectionTimer();
		void stopConnectionTimer();
		void setDatapoint();

		//Interface for ITimeoutCallback
		void timerExpired(long timerId, void* userData);

		bool m_timerStarted;
		TA_Base_Core::ReEntrantThreadLockable m_connStateLock;
		
		TA_Base_Core::SingletonTimerUtil&		m_timerUtility;
		unsigned long                           m_connectionFailedTimeout;

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
