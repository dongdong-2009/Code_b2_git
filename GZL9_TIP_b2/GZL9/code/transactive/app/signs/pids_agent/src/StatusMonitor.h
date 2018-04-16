#ifndef STATUSMONITOR_H_INCLUDED
#define STATUSMONITOR_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL13_TIP/GZL13/transactive/app/signs/pids_agent/src/StatusMonitor.h $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/10/01 16:36:38 $
  * Last modified by:  $Author: grace.koh $
  *
  */
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
//#include "core/data_access_interface/entity_access/src/PIDSEntityData.h"

#include "app/signs/pids_agent/src/PIDSManager.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"

#include "core/timers/src/ITimeoutCallback.h"     //TD15277
#include "core/timers/src/SingletonTimerUtil.h"


//#include "bus/ats/ats_agent/IDL/src/IAtsPidsCorbaDef.h"
//#include "bus/ats/ats_agent/IDL/src/AtsCommonCorbaDef.h"

#define CONNECTION_LINK_SUMMARY_ALARM_DP_NAME "diiPIDSC-ConnectionLinkSummaryAlarm"
#define CLOCK_LINK_SUMMARY_ALARM_DP_NAME "diiPIDSC-ClockLinkSummaryAlarm"
#define RXH_LINK_SUMMARY_ALARM_DP_NAME "diiPIDSC-RXHLinkAlarm"             // OCC only
#define LED_DISPLAY_SUMMARY_ALARM_DP_NAME "diiPIDSC-LEDDisplaySummaryAlarm"       // Stations only
#define PLASMA_DISPLAY_SUMMARY_ALARM_DP_NAME "diiPIDSC-PlasmaDisplaySummaryAlarm" // Stations only
#define PIDS_OCC_SERVER_STATUS_DP_NAME "diiPIDSC-PIDSOCCServerStatus"               // OCC only

#define PIDS_SERVER_EQUIPMENT_NAME "SEV"
#define ISCS_SERVER_EQUIPMENT_NAME "ISCS"
/*
typedef TA_Base_Core::NamedObject<TA_IRS_Bus::IAtsPidsCorbaDef,
		TA_IRS_Bus::IAtsPidsCorbaDef_ptr,
		TA_IRS_Bus::IAtsPidsCorbaDef_var> AtsNamedObject;
*/
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
//		void initialise( TA_Base_Core::PIDSAgentEntityDataPtr agentData,
//                       TA_Base_Core::ILocation::ELocationType locationType );

		std::string getAgentAssetName() { return m_agentAssetName; };

		// Add a new datapoint to this singleton when the agent
		// starts up, and the DP entities are read from the database
/*		void addDataPoint( DataPointDetails* theDataPoint );

		void setDataPointsToControlMode();

		void setDataPointsToMonitorMode();

        void updateDataPointState( const std::string& dpName, TA_Base_Bus::DataPointState& dataPoint, const std::string& equipmentName, bool manuallyDressed );

        TA_Base_Bus::DpValue getCurrentDataPointValue( const std::string& dpName, const std::string& equipmentName );

		void setBadDataPointQuality();
*/

//		void processAtsMessage( const TA_IRS_Bus::IAtsPidsCorbaDef::AtsPidsMessageCorbaDef& atsPidsMessage ); //TD15277

		/**
		  *
          * timerExpired
          *
          * This operation will be called when the registered timeout period has expired.
          *
          */
		virtual void timerExpired(long timerId, void* userData);

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

		//
		// The one and only instance of this class.
		//
		static StatusMonitor* m_theClassInstance;
		
		//
        // Protect singleton creation
        //
		static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;
		static TA_Base_Core::ReEntrantThreadLockable m_atsMessageLock;


		// Current ISCS Train Time Schedule version
		int m_currentTrainTimeScheduleVersion;

		// Name of the current ISCS Train Time Schedule
		std::string m_currentTrainTimeScheduleName;

		std::string m_location;

        // is this agent at the depot, occ, or a station
        TA_Base_Core::ILocation::ELocationType m_locationType;

		bool m_shouldBeRunning;

//		std::vector< TA_IRS_App::DataPointDetails* > m_virtualDataPointList;
/*
		// ATS info
		//TD15277
		struct AtsPidsData
		{
            TA_IRS_Bus::StationIdType			stationId;
			TA_IRS_Bus::PlatformIdType			platformId;
			TA_IRS_Bus::PidsPlatformIdType		PidsPlatformId;
			
            TA_IRS_Bus::IAtsPidsCorbaDef::PlatformInfo	platformInformation;

			TA_IRS_Bus::IAtsPidsCorbaDef::TrainInfoPair	trainInformation;
		};
        std::vector<AtsPidsData> m_atsInfoList;

		AtsNamedObject m_atsAgent;

		// Has the ATS Agent been contacted to establish the state of the
		// platform and train tables?
		bool m_atsDataInitialised;
*/
        // load the current message libraries so messages can be retreived
        TA_Base_Core::ReEntrantThreadLockable m_messageLibraryLock;
		
		// agent's asset name
		std::string m_agentAssetName;
		TA_Base_Core::SingletonTimerUtil&		 m_timer;
	};

}

#endif // STATUSMONITOR_H_INCLUDED
