#ifndef STATUSMONITOR_H_INCLUDED
#define STATUSMONITOR_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/src/StatusMonitor.h $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #7 $
  *
  * Last modification: $DateTime: 2014/04/02 16:28:20 $
  * Last modified by:  $Author: huang.wenbo $
  *
  */
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/TISAgentEntityData.h"
#include "core/data_access_interface/tis_agent_4669/src/IPredefinedMessageLibrary.h"
#include "core/data_access_interface/src/ILocation.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "app/signs/tis_agent/src/STISManager.h"
#include "DataPointCache.h"

namespace TA_IRS_App
{

	class StatusMonitor : public TA_Base_Core::Thread
	{

	public:

		/**
		  * StatusMonitor
		  *
		  * Destructor
		  *
		  * @return     none.
		  *
		  */
		virtual ~StatusMonitor();

		/**
		  * getInstance
		  *
		  * Gets the one and only StatusMonitor.
		  *
		  * @return     StatusMonitor&
		  *             The only StatusMonitor.
		  */
		static StatusMonitor* getInstance();

		// Other methods

		virtual void run();

		virtual void terminate();

		// Set up with current values
		void initialise( TA_Base_Core::TISAgentEntityDataPtr agentData,
                         TA_Base_Core::ILocation::ELocationType locationType );

        unsigned long getNextSTISStationLibraryVersion();

        unsigned long getNextSTISTrainLibraryVersion();

		unsigned long getCurrentSTISStationLibraryVersion();

		//unsigned long getCurrentSTISTrainLibraryVersion();

        unsigned long getNextISCSStationLibraryVersion();

        unsigned long getNextISCSTrainLibraryVersion();

		unsigned long getCurrentISCSStationLibraryVersion();

		unsigned long getCurrentISCSTrainLibraryVersion();

		std::string getAgentAssetName();

		bool getStationLibrariesSynchronised();

		bool getTrainLibrariesSynchronised();

        void setNextSTISStationLibraryVersion( unsigned long nextVersionNumber, const std::string& dataPointFirstLevelName = "" );

		void setNextSTISTrainLibraryVersion( unsigned long nextVersionNumber );

		void setCurrentSTISStationLibraryVersion( unsigned long libraryVersion, const std::string& dataPointFirstLevelName = "" );

		//void setCurrentSTISTrainLibraryVersion( unsigned long libraryVersion );

		void setNextISCSStationLibraryVersion( unsigned long nextVersionNumber );

		void setNextISCSTrainLibraryVersion( unsigned long nextVersionNumber );

		void setCurrentISCSStationLibraryVersion( unsigned long libraryVersion );

		void setCurrentISCSTrainLibraryVersion( unsigned long libraryVersion );


		void setStationLibrariesSynchronised( bool synchronised );

		void setTrainLibrariesSynchronised( bool synchronised );

		// Add a new datapoint to this singleton when the agent
		// starts up, and the DP entities are read from the database
		void addDataPoint( TA_Base_Bus::DataPoint* theDataPoint );

		void setDataPointsToControlMode();

		void setDataPointsToMonitorMode();

		void updatePidStatus(const int address, const int status, const std::string& dataPointFirstLevelName = "");

		void updateConnectionLinkAlarmDataPointState(bool status, const std::string& dataPointFirstLevelName = "");

		void updateOccServerStatusAlarmDataPointState(bool status);
		void setBadDataPointQuality();

		void setNextISCSStationLibraryVersion_central( unsigned long nextVersionNumber_central );

		void setNextISCSTrainLibraryVersion_central( unsigned long nextVersionNumber_central );

		void setCurrentISCSStationLibraryVersion_central( unsigned long libraryVersion_central );

		void setCurrentISCSTrainLibraryVersion_central( unsigned long libraryVersion_central );

        unsigned long getNextISCSStationLibraryVersion_central();

        unsigned long getNextISCSTrainLibraryVersion_central();

		unsigned long getCurrentISCSStationLibraryVersion_central();

		unsigned long getCurrentISCSTrainLibraryVersion_central();

		void setTimeScheduleDownloadStatus(bool status);
		
		bool isTimeScheduleInDownload();

	private:

		static void setDataPointIntValue(TA_Base_Bus::DataPoint* dataPoint, unsigned long value, bool forceState = true);
		static void setDataPointBooleanValue(TA_Base_Bus::DataPoint* dataPoint, bool value, bool forceState = true);
		static void setDataPointEnumValue(TA_Base_Bus::DataPoint* dataPoint, int value, bool forceState = true);
		/**
		  * constructor
		  *
		  * @return     nothing.
		  *
		  */
		StatusMonitor();

		//
		// The one and only instance of this class.
		//
		static StatusMonitor* m_theClassInstance;
		
		//
        // Protect singleton creation
        //
		static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;

		std::string m_location;

        // is this agent at the depot, occ, or a station
        TA_Base_Core::ILocation::ELocationType m_locationType;

		// Time in milliseconds between status poll messages (OCC and Stations)
        int m_statusPollInterval;

		bool m_shouldBeRunning;

		
		// agent's asset name
		std::string m_agentAssetName;
		// time schedule download status, true mean download in progress 
		bool m_isTimeScheduleDownloadInProgress;
		// Cache all datapoints
		DataPointCache m_dataPointCahe;
	};

}

#endif // STATUSMONITOR_H_INCLUDED
