/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/AtsAgent/src/WatchDogData.cpp $
  * @author:   Darren Sampson
  * @version:  $Revision: #4 $
  *
  * Last modification: $DateTime: 2015/01/21 17:42:44 $
  * Last mofified by:  $Author: peter.wong $
  *
  * This represents the ATS Train Record
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <sstream>
#include "time.h"

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/AtsAgentException.h"
#include "core/exceptions/src/FatalConfigurationException.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "app/ats/AtsAgent/src/AddressUtil.h"
#include "app/ats/AtsAgent/src/WatchDogData.h"
#include "app/ats/AtsAgent/src/IAtsStatusObserver.h"

namespace TA_IRS_App
{
	const std::string WatchDogData::ATS_WATCHDOG = "AtsWatchDog";

	WatchDogData::WatchDogData(	TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType,
								unsigned long maxWatchdogInterval, unsigned long connFailedTimeout)
	:
	m_numberOfConnections (0),
	m_locationType(locationType),
	m_maxWatchdogInterval (maxWatchdogInterval),
	m_watchDogDp (NULL),
	m_isInControlMode (false),
	m_atsOk (true),
    m_statusObserver(NULL),
	m_modeChangeFlag(false),
	m_connectionFailedTimeout(connFailedTimeout),
	m_timerUtility(TA_Base_Core::SingletonTimerUtil::getInstance()),
	m_timerStarted(false)
	{
		TA_ASSERT(locationType == TA_Base_Core::AtsAgentEntityData::Occ || 
				  locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
				  locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
				  locationType == TA_Base_Core::AtsAgentEntityData::MainStation ||
				  locationType == TA_Base_Core::AtsAgentEntityData::Depot,
				  "Unknown location type - ATS Watchdog table is valid for OCC, DPT and STATION");

		m_watchDogs.clear();

		if (m_maxWatchdogInterval == 0)
		{
			m_maxWatchdogInterval = 8;
		}

		if (m_connectionFailedTimeout == 0)
		{
			m_connectionFailedTimeout = 10000; //10s default value as suggested by Bihui
		}

	}

	WatchDogData::~WatchDogData()
	{
	}

	void WatchDogData::setNumberOfConnections(int numberOfConnections)
	{
		m_watchDogs.clear();

		m_numberOfConnections = numberOfConnections;

		for (int i = 0; i < m_numberOfConnections; i++)
		{
			AtsWatchDog watchDog;
			watchDog.chronologicalCounter = 0;
			watchDog.watchDogState = WatchDogNotRead;
			watchDog.connectionState = ConnectionUnknown;

			m_watchDogs.push_back(watchDog);
		}
	}

	bool WatchDogData::attachDataPoint(TA_Base_Bus::DataPoint* dataPoint)
	{
		std::string address = dataPoint->getDataPointAddress();

		bool found = checkAttribute(address);
		
		if (found)
		{
			if (m_watchDogDp != NULL)
			{
				TA_THROW(TA_Base_Core::FatalConfigurationException("Configuration Error - More than one WatchDog datapoint"));
			}

			m_watchDogDp = dataPoint;
			
			updateDataPoint();
		}

		return found;
	}

	bool WatchDogData::checkAttribute(std::string address)
	{
		// WatchDog Datapoint
		if (AddressUtil::caseInsensitiveCompare(address, ATS_WATCHDOG))
		{
			return true;
		}

		return false;
	}

	void WatchDogData::sendUpdates()
	{
		updateDataPoint();
	}


	void WatchDogData::updateDataPoint()
	{	
		
		//TD15303++
		if ( NULL == m_watchDogDp ) 
		{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                "updateDataPoint() - m_watchDogDp is NULL!Please check ConfigureEditor-KCD.BMF.ATS.DATS...."
				);

				return;
		}
		//++TD15303

		TA_Base_Bus::DpValue dpValue(m_watchDogDp->getDataPointDataType(),
									 m_watchDogDp->getEnumLabelsMap(),
									 m_watchDogDp->getBooleanLabels());
		TA_Base_Bus::EQualityStatus dpQuality = TA_Base_Bus::QUALITY_BAD_NO_SPECIFIC_REASON;


		if (m_atsOk)
		{
			// Set atsOk to false only if all connections are failed
			// or connected with a failed watchdog

			bool changeState = true;

			for (int i = 0; i < m_numberOfConnections; i++)
			{
				changeState = changeState && 
							  (m_watchDogs[i].connectionState == ConnectionFailed ||
							   ((m_watchDogs[i].connectionState == ConnectionRead ||
								 m_watchDogs[i].connectionState == ConnectionWriteRead) &&
								m_watchDogs[i].watchDogState == WatchDogNotValid));
			}

			if (changeState)
			{
				// Bad ATS State
				m_atsOk = false;
				
                if (m_statusObserver != NULL)
                {
                    m_statusObserver->atsServerStatusChanged(m_atsOk);
                }
			}
		}
		else
		{
			// Set atsOk to true if any connection is WriteRead
			// and WatchDog is valid

			bool changeState = false;

			for (int i = 0; i < m_numberOfConnections; i++)
			{
				changeState = changeState ||
							  ( ( m_watchDogs[i].connectionState == ConnectionRead || 
							      m_watchDogs[i].connectionState == ConnectionWriteRead
								 ) &&
							   m_watchDogs[i].watchDogState == WatchDogValid);
			}

			if (changeState)
			{
				// Good ATS State
				m_atsOk = true;

                if (m_statusObserver != NULL)
                {
                    m_statusObserver->atsServerStatusChanged(m_atsOk);
                }
			}
		}


		if (m_modeChangeFlag)
		{
			time_t currentTime;
			time(&currentTime);
			// 30 seconds are customer's suggestion for server failover, which is much longer than agent failover.
			// the idea here is to prevent the false value setting during the failover 30 seconds.
			// the false value will cause the datapoint alarm raised.
			// when doing testing, at the failover, there're some unreliability problem with the watch dog
			// this should fix it. otherwise, the last step we want to do is to set m_atsOk as true in this period.
			if ( currentTime - m_modeChangeTime >= 30)
			{
				m_modeChangeFlag = false;
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					"[Debug Purpose]updateDataPoint() during modechange, just return for the first 30 seconds, not really reliable.");

				if ( !m_atsOk || !m_isInControlMode)
				{
					return;
				}
				
			}			
		}
	
		//If ATS Server Connection is not OK,
		//then wait for a certain time to let the ATS Server to recover
		//before updating the Datapoint to raise an Alarm
		if(false == m_atsOk)
		{
			//Timer mechanism to wait for a certain time before updating the Datapoint to raise alarm.
			startConnectionTimer();
		}
		else
		{
			//Stop the timer if the ATS Server connection has been established. 
			//And update the Datapoint immediately, telling that the Connection is OK.
			stopConnectionTimer();
			setDatapoint();
		}
	}

	WatchDogData::EWatchDogState WatchDogData::getWatchDogState(int connection)
	{
		TA_ASSERT(connection < m_numberOfConnections && connection >= 0, "Invalid connection");

		std::string watchDogState = "";

		switch(m_watchDogs[connection].watchDogState)
		{
		case WatchDogNotRead:
			watchDogState = "WatchDogNotRead";
			break;

		case WatchDogReadOnce:
			watchDogState = "WatchDogReadOnce";
			break;

		case WatchDogValid:
			watchDogState = "WatchDogValid";
			break;

		case WatchDogNotValid:
			watchDogState = "WatchDogNotValid";
			break;
		}

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "isWatchDogValid(%d) - watchDogState=%s", connection, watchDogState.c_str());

		return 	m_watchDogs[connection].watchDogState;
	}

	void WatchDogData::updateConnectionState(int connection, WatchDogData::EConnectionState connectionState)
	{
		TA_ASSERT(connection < m_numberOfConnections && connection >= 0, "Invalid connection");

		//bihui modified because of the watch dog problem
		//there is no write operation for ats modbus now.
		
        m_watchDogs[connection].connectionState = connectionState ;
		if (m_watchDogs[connection].connectionState == ConnectionFailed)
		{
			m_watchDogs[connection].watchDogState = WatchDogNotRead;

			//Reset the internal state of chronologicalCounter if Connection failure occur
			m_watchDogs[connection].chronologicalCounter = 0;

			updateDataPoint();
		}

	

      /*
		switch (m_watchDogs[connection].connectionState)
		{
		case ConnectionUnknown:
		case ConnectionWrite:
			switch (connectionState)
			{
			case ConnectionWrite:
			case ConnectionWriteRead:
			case ConnectionRead:
			case ConnectionFailed:
				m_watchDogs[connection].connectionState = connectionState;

			case ConnectionUnknown:
			default:
				break;
			}
			break;

		case ConnectionWriteRead:
			switch (connectionState)
			{
			case ConnectionRead:
			case ConnectionFailed:
				m_watchDogs[connection].connectionState = connectionState;

			case ConnectionUnknown:
			case ConnectionWrite:
			case ConnectionWriteRead:
			default:
				break;
			}
			break;


		case ConnectionRead:
		case ConnectionFailed:
			switch (connectionState)
			{
			case ConnectionWrite:
			case ConnectionRead:
			case ConnectionFailed:
				m_watchDogs[connection].connectionState = connectionState;

			case ConnectionUnknown:
			case ConnectionWriteRead:
			default:
				break;
			}
			break;

		default:
			break;
		}

		if (m_watchDogs[connection].connectionState == ConnectionFailed)
		{
			m_watchDogs[connection].watchDogState = WatchDogNotRead;
			updateDataPoint();
		}*/

	}

	void WatchDogData::updateWatchDog(int connection, unsigned long chronologicalCounter)
	{
		TA_ASSERT(connection < m_numberOfConnections && connection >= 0, "Invalid connection");

		bool watchDogIncremented =  ((chronologicalCounter > m_watchDogs[connection].chronologicalCounter &&
												   chronologicalCounter <= m_watchDogs[connection].chronologicalCounter + m_maxWatchdogInterval) ||
												  (chronologicalCounter < m_watchDogs[connection].chronologicalCounter &&
												   chronologicalCounter + 0xFFFF <= m_watchDogs[connection].chronologicalCounter + m_maxWatchdogInterval));

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"watchDogIncremented : %s, Internal state chronologicalCounter : %d, ATS Server chronologicalCounter :%d",
			(watchDogIncremented)?"TRUE":"FALSE", m_watchDogs[connection].chronologicalCounter, chronologicalCounter);

		switch(m_watchDogs[connection].watchDogState)
		{
		case WatchDogNotRead:
			m_watchDogs[connection].watchDogState = WatchDogReadOnce;
			break;

		case WatchDogReadOnce:
		case WatchDogValid:
		case WatchDogNotValid:
		default:
			if (watchDogIncremented)
			{
				m_watchDogs[connection].watchDogState = WatchDogValid;
			}
			else
			{
				m_watchDogs[connection].watchDogState = WatchDogNotValid;
			}
			break;
		}
		
		m_watchDogs[connection].chronologicalCounter = chronologicalCounter;
	}

	bool WatchDogData::isAtsOk()
	{
		return m_atsOk;
	}

	void WatchDogData::updateIsInControlMode( bool isControlMode )
	{
		if ( (isControlMode == true) && (m_isInControlMode == false) )
		{
			m_atsOk = true;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
				"[Debug Purpose]updateIsInControlMode(), m_atsOk is set to true." );
			
			if (m_statusObserver != NULL)
			{
				m_statusObserver->atsServerStatusChanged(m_atsOk);
            }

			m_modeChangeFlag = true;
			time(&m_modeChangeTime ); 
		}

		m_isInControlMode = isControlMode;

	}


    void WatchDogData::setObserver(IAtsStatusObserver* statusObserver)
    {
        m_statusObserver = statusObserver;
    }

	void WatchDogData::timerExpired(long timerId, void* userData)
	{
		TA_Base_Core::ThreadGuard guard(m_connStateLock);
		//reset timer
		m_timerStarted = false;
		//Alarm will be raised here
		setDatapoint();
	}

	void WatchDogData::startConnectionTimer()
	{
		TA_Base_Core::ThreadGuard guard(m_connStateLock);
		if (m_timerStarted == false)
		{
			//timer flag
			m_timerStarted = true;
			//Timer will be started here.
			m_timerUtility.startPeriodicTimeOutClock(this,m_connectionFailedTimeout);

		}
	}

	void WatchDogData::stopConnectionTimer()
	{
		TA_Base_Core::ThreadGuard guard(m_connStateLock);
		if (m_timerStarted == true)
		{
			//timer flag
			m_timerStarted = false;
			//Timer will be stopped here, if the connection has been established.
			m_timerUtility.stopPeriodicTimeOutClock(this);
		}
	}
	void WatchDogData::setDatapoint()
	{
		// Set the value of the datapoint
		TA_Base_Bus::DpValue dpValue(m_watchDogDp->getDataPointDataType(),
			m_watchDogDp->getEnumLabelsMap(),
			m_watchDogDp->getBooleanLabels());

		dpValue.setBoolean ( m_atsOk );
		TA_Base_Bus::EQualityStatus dpQuality = TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON;

		if (m_watchDogDp != NULL && m_isInControlMode) //wenching++ (TD14526)
		{
			//The log below is used for testing purposes only
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Update datapoints for Watchdog, Value : %s",
				m_atsOk?"TRUE":"False");
			TA_Base_Bus::DataPointState newState ( dpValue, time( NULL ), dpQuality );
			m_watchDogDp->updateFieldState( newState );		
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"updateDataPoint() - Watchdog Datapoint not defined - ATS Server %s",
				(m_atsOk?"OK":"Not OK"));
		}
	}

}
