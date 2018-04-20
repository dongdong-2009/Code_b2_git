#ifndef STATUSMONITOR_H_INCLUDED
#define STATUSMONITOR_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File$
  * @author:   Robin Ashcroft
  * @version:  $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/TISAgentEntityData.h"
#include "core/data_access_interface/tis_agent_4669/src/IPredefinedMessageLibrary.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "app/signs/tis_agent/src/STISManager.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"

#include "core/timers/src/ITimeoutCallback.h"     //TD15277
#include "core/timers/src/SingletonTimerUtil.h"


#include "bus/ats/ats_agent/IDL/src/IAtsTisCorbaDef.h"
#include "bus/ats/ats_agent/IDL/src/AtsCommonCorbaDef.h"

#define CONNECTION_LINK_SUMMARY_ALARM_DP_NAME "diiTISC-ConnectionLinkSummaryAlarm"
#define CLOCK_LINK_SUMMARY_ALARM_DP_NAME "diiTISC-ClockLinkSummaryAlarm"
#define RXH_LINK_SUMMARY_ALARM_DP_NAME "diiTISC-RXHLinkAlarm"             // OCC only
#define LED_DISPLAY_SUMMARY_ALARM_DP_NAME "diiTISC-LEDDisplaySummaryAlarm"       // Stations only
#define PLASMA_DISPLAY_SUMMARY_ALARM_DP_NAME "diiTISC-PlasmaDisplaySummaryAlarm" // Stations only
#define TIS_OCC_SERVER_STATUS_DP_NAME "diiTISC-TISOCCServerStatus"               // OCC only

#define CURRENT_STATION_LIBRARY_VERSION_DP_NAME "aiiTISC-CurrentSTISLibraryVersion"
#define CURRENT_TRAIN_LIBRARY_VERSION_DP_NAME "aiiTISC-CurrentTTISLibraryVersion"
#define NEXT_STATION_LIBRARY_VERSION_DP_NAME "aiiTISC-NextSTISLibraryVersion"
#define NEXT_TRAIN_LIBRARY_VERSION_DP_NAME "aiiTISC-NextTTISLibraryVersion"

//hongran++ TD17500
#define CENTRAL_CURRENT_STATION_LIBRARY_VERSION_DP_NAME "aiiTISC-CurrentSTISLibraryVersion-CDB"
#define CENTRAL_CURRENT_TRAIN_LIBRARY_VERSION_DP_NAME "aiiTISC-CurrentTTISLibraryVersion-CDB"
#define CENTRAL_NEXT_STATION_LIBRARY_VERSION_DP_NAME "aiiTISC-NextSTISLibraryVersion-CDB"
#define CENTRAL_NEXT_TRAIN_LIBRARY_VERSION_DP_NAME "aiiTISC-NextTTISLibraryVersion-CDB"
//++hongran TD17500

#define CURRENT_TRAIN_TIME_SCHEDULE_VERSION_DP_NAME "aiiTISC-CurrentTimeScheduleVersion"
#define CURRENT_TRAIN_TIME_SCHEDULE_KEY_DP_NAME "aiiTISC-CurrentTimeScheduleKey"

#define NEXT_STIS_LIBRARY_FILENAME_DP_NAME "diiTISC-NextSTISLibraryFilename"
#define NEXT_TTIS_LIBRARY_FILENAME_DP_NAME "diiTISC-NextTTISLibraryFilename"

#define STATION_LIBRARIES_SYNCHRONISED_DP_NAME "diiTISC-StationLibrariesSynchronised"
#define TRAIN_LIBRARIES_SYNCHRONISED_DP_NAME "diiTISC-TrainLibrariesSynchronised"

#define STIS_SERVER_EQUIPMENT_NAME "SEV"
#define ISCS_SERVER_EQUIPMENT_NAME "ISCS"

typedef TA_Base_Core::NamedObject<TA_IRS_Bus::IAtsTisCorbaDef,
		TA_IRS_Bus::IAtsTisCorbaDef_ptr,
		TA_IRS_Bus::IAtsTisCorbaDef_var> AtsNamedObject;

namespace TA_IRS_App
{

	class StatusMonitor : public TA_Base_Core::Thread, 
						  public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>,
						  public TA_Base_Core::ITimeoutCallback
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

        /**
         * Receive notifications from the Train Agent.
         *
         * @param message  the message, containing a download complete notification
         *                 the download may have succeeded or failed
         * 
         */
        virtual void receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& message );

		// Set up with current values
		void initialise( TA_Base_Core::TISAgentEntityDataPtr agentData,
                         TA_Base_Core::ILocation::ELocationType locationType );

        int getNextSTISStationLibraryVersion();

        int getNextSTISTrainLibraryVersion();

		int getCurrentSTISStationLibraryVersion();

		int getCurrentSTISTrainLibraryVersion();

        int getNextISCSStationLibraryVersion();

        int getNextISCSTrainLibraryVersion();

		int getCurrentISCSStationLibraryVersion();

		int getCurrentISCSTrainLibraryVersion();

		std::string getAgentAssetName() { return m_agentAssetName; };


		bool getStationLibrariesSynchronised();

		bool getTrainLibrariesSynchronised();

		int getCurrentTrainTimeScheduleKey();

		int getCurrentTrainTimeScheduleVersion();

		std::string getNextSTISLibraryFilename();

		std::string getNextTTISLibraryFilename();

		void setNextSTISStationLibraryVersion( int nextVersionNumber );

		void setNextSTISTrainLibraryVersion( int nextVersionNumber );

		void setCurrentSTISStationLibraryVersion( int libraryVersion );

		void setCurrentSTISTrainLibraryVersion( int libraryVersion );

		void setNextISCSStationLibraryVersion( int nextVersionNumber );

		void setNextISCSTrainLibraryVersion( int nextVersionNumber );

		void setCurrentISCSStationLibraryVersion( int libraryVersion );

		void setCurrentISCSTrainLibraryVersion( int libraryVersion );


		void setStationLibrariesSynchronised( bool synchronised );

		void setTrainLibrariesSynchronised( bool synchronised );

		void setCurrentTrainTimeScheduleVersion( int version );

		void setCurrentTrainTimeScheduleKey( int key );

		void setNextSTISLibraryFilename( const std::string& filename );

		void setNextTTISLibraryFilename( const std::string& filename );

		// Add a new datapoint to this singleton when the agent
		// starts up, and the DP entities are read from the database
		void addDataPoint( DataPointDetails* theDataPoint );

		void setDataPointsToControlMode();

		void setDataPointsToMonitorMode();

        void updateDataPointState( const std::string& dpName, TA_Base_Bus::DataPointState& dataPoint, const std::string& equipmentName, bool manuallyDressed );

        TA_Base_Bus::DpValue getCurrentDataPointValue( const std::string& dpName, const std::string& equipmentName );

		void setBadDataPointQuality();

        std::string getStationPredefinedMessageText(TA_Base_Core::ELibrarySection librarySection, unsigned short messageTag);
                
        std::string getTrainPredefinedMessageText(TA_Base_Core::ELibrarySection librarySection, unsigned short messageTag);

        TA_Base_Core::TTISLedAttributes getDefaultTTISLedAttributes();

		void processAtsMessage( const TA_IRS_Bus::IAtsTisCorbaDef::AtsTisMessageCorbaDef& atsTisMessage ); //TD15277

		/**
		  *
          * timerExpired
          *
          * This operation will be called when the registered timeout period has expired.
          *
          */
		virtual void timerExpired(long timerId, void* userData);

		//hongran++ TD17500
		void setNextISCSStationLibraryVersion_central( int nextVersionNumber_central );

		void setNextISCSTrainLibraryVersion_central( int nextVersionNumber_central );

		void setCurrentISCSStationLibraryVersion_central( int libraryVersion_central );

		void setCurrentISCSTrainLibraryVersion_central( int libraryVersion_central );

        int getNextISCSStationLibraryVersion_central();

        int getNextISCSTrainLibraryVersion_central();

		int getCurrentISCSStationLibraryVersion_central();

		int getCurrentISCSTrainLibraryVersion_central();

		//++hongran TD17500

	private:

		/**
		  * constructor
		  *
		  * @return     nothing.
		  *
		  */
		StatusMonitor();

		void subscribeToNotifications();

		void getInitialATSInformation();


        
        void loadMessageLibrary( std::string type );

        std::string getPredefinedMessageText( TA_Base_Core::IPredefinedMessageLibrary* messageLibrary,
                                              TA_Base_Core::ELibrarySection librarySection,
                                              unsigned short messageTag );

		//
		// The one and only instance of this class.
		//
		static StatusMonitor* m_theClassInstance;
		
		//
        // Protect singleton creation
        //
		static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;
		static TA_Base_Core::ReEntrantThreadLockable m_atsMessageLock;

		bool m_stationLibrariesSynchronised;
		bool m_trainLibrariesSynchronised;

		int m_currentSTISStationLibraryVersion;
        int m_currentSTISTrainLibraryVersion;
        int m_nextSTISStationLibraryVersion;
        int m_nextSTISTrainLibraryVersion;

		int m_currentISCSStationLibraryVersion;
        int m_currentISCSTrainLibraryVersion;
        int m_nextISCSStationLibraryVersion;
        int m_nextISCSTrainLibraryVersion;

		//hongran++ TD17500
		int m_currentISCSStationLibraryVersion_central;
        int m_currentISCSTrainLibraryVersion_central;
        int m_nextISCSStationLibraryVersion_central;
        int m_nextISCSTrainLibraryVersion_central;
		//++hongran TD17500


		// Current ISCS Train Time Schedule version
		int m_currentTrainTimeScheduleVersion;

		// Name of the current ISCS Train Time Schedule
		std::string m_currentTrainTimeScheduleName;

		// Name of the STIS and TTIS message library files
		// on the remote server - OCC only
		std::string m_nextSTISLibraryFilename;
		std::string m_nextTTISLibraryFilename;

		std::string m_location;

        // is this agent at the depot, occ, or a station
        TA_Base_Core::ILocation::ELocationType m_locationType;

		// Time in milliseconds between status poll messages (OCC and Stations)
        int m_statusPollInterval;

		bool m_shouldBeRunning;

		std::vector< TA_IRS_App::DataPointDetails* > m_virtualDataPointList;

		// ATS info
		//TD15277
		struct AtsTisData
		{
            TA_IRS_Bus::StationIdType			stationId;
			TA_IRS_Bus::PlatformIdType			platformId;
			TA_IRS_Bus::TisPlatformIdType		tisPlatformId;
			
            TA_IRS_Bus::IAtsTisCorbaDef::PlatformInfo	platformInformation;

			TA_IRS_Bus::IAtsTisCorbaDef::TrainInfoPair	trainInformation;
		};
        std::vector<AtsTisData> m_atsInfoList;

		AtsNamedObject m_atsAgent;

		// Has the ATS Agent been contacted to establish the state of the
		// platform and train tables?
		bool m_atsDataInitialised;

        // load the current message libraries so messages can be retreived
        TA_Base_Core::ReEntrantThreadLockable m_messageLibraryLock;
        TA_Base_Core::IPredefinedMessageLibrary* m_currentStationMessageLibrary;
        TA_Base_Core::IPredefinedMessageLibrary* m_currentTrainMessageLibrary;
		
		// agent's asset name
		std::string m_agentAssetName;
		TA_Base_Core::SingletonTimerUtil&		 m_timer;

		TA_Base_Core::ReEntrantThreadLockable m_atsListAccessLock;
	};

}

#endif // STATUSMONITOR_H_INCLUDED
