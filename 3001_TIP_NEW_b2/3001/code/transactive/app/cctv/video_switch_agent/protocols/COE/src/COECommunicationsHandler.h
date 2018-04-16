#if !defined(COECommunicationsHandler_6D856C3B_635C_4e91_8C91_072E38E337FD__INCLUDED_)
#define COECommunicationsHandler_6D856C3B_635C_4e91_8C91_072E38E337FD__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/COECommunicationsHandler.h $
  * @author:   Rob Young
  * @version:  $Revision: #15 $
  *
  * Last modification: $DateTime: 2017/03/23 10:26:54 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Handles the protocol dependent sections of the operations for a CommunicationsHandler
  * using the COE protocol.
  * This interface is used by the CommunicationsHandler class.
  *
  */

#include "app/cctv/video_switch_agent/protocols/src/IPDCommunicationsHandler.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/SynchronisedQueue.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"
#include "app/cctv/video_switch_agent/protocols/src/IPDAlarmInterface.h"
#include "app/cctv/video_switch_agent/protocols/src/IPDStageModeInterface.h"
#include "bus/trains/TrainCommonLibrary/src/AtsZoneUtils.h"
#include "core/data_access_interface/video_switch_agent/src/VideoStationIdAccessFactory.h"
#include "core/data_access_interface/video_switch_agent/src/VideoStationIdAccessFactory.h"
#include "core/data_access_interface/video_switch_agent/src/IVideoTriggeringEvent.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/entity_access/src/VideoSwitchAgent.h"

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/utilities/src/DebugUtil.h"

#include <string>
#include <memory>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>


namespace TA_Base_Core
{
    class AlarmHelper;
    class IEntityData;
}

namespace TA_IRS_App
{
    class COECommunicationsChannel;
    class IAgentRequestCommand;
    class IHardwareNotificationCommand;
    class ReceivedDataProcessor;
    class StateValidationCycle;
}

namespace TA_IRS_App
{
	template <class QueueItemType> class SynchronisedQueueWithSemaphore : public SynchronisedQueue<QueueItemType>
	{
	public:
		SynchronisedQueueWithSemaphore():m_semaphore(0)
		{

		}
		void post()
		{
			m_semaphore.post();
		}
        void push(const QueueItemType& item)
		{
			SynchronisedQueue<QueueItemType>::push(item);
			post();
		}
		void wait()
		{
			m_semaphore.wait();
		}
	private:
		TA_Base_Core::Semaphore m_semaphore;
	};

	template <class QueueItemType> class CommandsProcessor : public TA_Base_Core::Thread
	{
	public:
		typedef SynchronisedQueueWithSemaphore<QueueItemType> ItemQueue;
		typedef boost::function<void(QueueItemType)> ThreadCallBack;
		CommandsProcessor(ItemQueue& que, ThreadCallBack cb)
			:m_itemsQueue(que), m_itemProcessor(cb), m_shouldRun(true)
		{
		}

		virtual ~CommandsProcessor()
		{
			if(TA_Base_Core::Thread::THREAD_STATE_UNKNOWN != getCurrentState())
			{
				terminateAndWait();
			}
		}

		virtual void run()
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Command process thread started");
			m_shouldRun = true;
			while(m_shouldRun)
			{
				m_itemsQueue.wait();
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Queue size is:%lu", m_itemsQueue.size());
				if(m_shouldRun && !(m_itemsQueue.empty()))
				{
					try
					{
	                    m_itemProcessor(m_itemsQueue.front());
					}
			        catch( const TA_Base_Core::TransactiveException& e )
					{
						LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
					}
					catch( ... )
					{
						 LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Reason unknown" );           
					}
					m_itemsQueue.pop();
				}
			}
			// Ensure que is cleared before call run() next time
			while(!m_itemsQueue.empty())
			{
				m_itemsQueue.pop();
			}
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Command process thread is about to stop");
		}

		virtual void terminate()
		{
			m_shouldRun = false;
			m_itemsQueue.post();
		}
	private:
		ItemQueue& m_itemsQueue;
		ThreadCallBack m_itemProcessor;
		bool m_shouldRun;
	};
}

namespace TA_IRS_App
{
    class COECommunicationsHandler : public IPDCommunicationsHandler,
                                     public IPDAlarmInterface
    {
    public:

        //
        // Have to use reference counting pointers to queue commands so can guarantee they only
        // get cleaned up when all references to it are out of scope.
        //
        typedef boost::shared_ptr< IAgentRequestCommand > RequestCommandSharedPtr; 
        typedef boost::shared_ptr< IHardwareNotificationCommand > NotificationCommandSharedPtr;
        
		//
        // Possible states of the Communications Channels this object type manages.
        //
        enum ECommunicationChannelState
        {
            NOT_CURRENTLY_ESTABLISHED,
            COMMUNICATIONS_FAILURE,
            IDLE_SWITCH_CHANNEL,
            ACTIVE_SWITCH_CHANNEL
        };

        /**
          * COECommunicationsHandler
          *
          * Standard constructor.
          *
          * @param      TA_Base_Core::VideoSwitchAgent& videoSwitchEntityData
          *             The configuration data for this CommunicationsHandler.
          *
          * @exception   VideoSwitchAgentInternalException
          *              Thrown if mandatory VideoSwitchAgent parameters or configured VideoSwitch 
          *              entity addresses cannot be retrieved from the database.
          */
        COECommunicationsHandler( TA_Base_Core::VideoSwitchAgentPtr videoSwitchAgentEntityData );


        /**
          * ~COECommunicationsHandler
          *
          * Standard destructor.
          */
        virtual ~COECommunicationsHandler();


        /** 
          * submitRequestCommand
          *
          * Adds the command to the vector of commands ready to be sent to 
          * the Video Switch.
          *
          * @param command
          * @param 
          *
          */
        void submitRequestCommand( RequestCommandSharedPtr command, bool isClientRequest );


        /** 
          * submitNotificationCommand
          *
          * Adds the command to the vector of notifications received from 
          * the Video Switch.
          *
          * @param command
          *
          */
        void submitNotificationCommand( NotificationCommandSharedPtr command );
        

        /** 
          * processCommunicationsChannelState
          *
          * description
          *
          * @param channelIndex
          * @param 
          *
          */
        void processCommunicationsChannelState( unsigned int channelIndex, ECommunicationChannelState state );


        /** 
          * processSummaryAlarmsState
          *
          * description
          *
          * @param systemSummaryAlarm
          * @param transmissionSummaryAlarm
          * @param cameraSummaryAlarm
          * @param 
          *
          */
        void processSummaryAlarmsState( bool systemSummaryAlarm, /*bool transmissionSummaryAlarm,*/ bool cameraSummaryAlarm, bool ancillarySummaryAlarm, bool dvrSummaryAlarm );


        /** 
          * processAlarmStack
          *
          * description
          *
          * @param 
          *
          */
        void processAlarmStack( TA_COE::AlarmStack alarmStack );
		
        ///////////////////////////////////////////////////////////////////////
        //
        // IPDCommunicationsHandler Methods
        //
        ///////////////////////////////////////////////////////////////////////
      
        
        /**
          * connect
          *
          * Connects to the switching hardware. Once connected this will
          * request the state of all devices from the hardware.
          */
        virtual void connect();


        /**
          * disconnect
          *
          * Disconnects from the switching hardware.
          */
        virtual void disconnect();


        ///////////////////////////////////////////////////////////////////////
        //
        // IPDAlarmInterface Methods
        //
        ///////////////////////////////////////////////////////////////////////
   
		/**
          * acknowledgeAlarm
          *
	      * acknowledges an alarm on the videoswitch alarm stack
          *
          * @param      string AlarmId
          *             the transactive alarm Id associated with the alarm to be Acked
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The alarm with the given iD is not found.
	      */
       virtual void acknowledgeAlarm( unsigned long stationId, unsigned long eventId, unsigned long monitorId );

	   virtual void acknowledgeAlarm( unsigned long entityKey, unsigned long consoleId);

		/**
          * getAlarmStack
          *
	      * gets all of the alarms curently open by this alarm interface
          *
          * @return     TA_Base_Bus::SwitchManagerCorbaDef::AlarmStack&
          *             The alarm stack object
		  *
		  */
		virtual TA_Base_Bus::SwitchManagerCorbaDef::AlarmStack getAlarmStack();

		/**
          * updateAlarmStack
          *
	      * returns all of the alarms curently open by this alarm interface
          *
          * @param     TA_Base_Bus::SwitchManagerCorbaDef::AlarmStack& stack
          *             The alarm stack object
		  *
		  */
		virtual void updateAlarmStack(TA_Base_Bus::SwitchManagerCorbaDef::AlarmStack& stack);

		virtual unsigned long getEntityKey();


		TA_COE::AlarmStack getOpenAlarmsByAlarmEntityKey(const unsigned long alarmEntityKey);

		virtual void setToControl();
        

    private:
		void startProcessors();

		void terminateProcessors();

        //
        // Disable copy constructor and assignment operator.
        //
        COECommunicationsHandler( const COECommunicationsHandler& theCOECommunicationsHandler );
        COECommunicationsHandler& operator=( const COECommunicationsHandler& );

        //
        // Private Helper Functions
        //

        /**
         * loadCommunicationsChannels
         *
         * Creates a COECommunicationChannel for each VideoSwitch address and port number from the database.
         *
         * @exception   DatabaseException
         *              Thrown if the address data  cannot be retrieved from the database.
         * @exception   DataException
         *              Thrown if the type for the address data retrieved from the database is invalid.
         */
        void loadCommunicationsChannels();


        /** 
          * activateCommunicationsChannels
          *
          * description
          *
          */
        void activateCommunicationsChannels();


        /** 
          * deactivateCommuncationsChannels
          *
          * description
          *
          */
        void deactivateCommunicationsChannels();


        /** 
          * cleanUp
          *
          * description
          *
          */
        void cleanUp();


        /** 
          * processRequestCommand
          *
          * description
          *
          * @param 
          */
        void processRequestCommand( RequestCommandSharedPtr command );


		void processNotification(NotificationCommandSharedPtr command);

        /** 
          * receivedChannelStateCommsFailure
          *
          * description
          *
          */
        void receivedChannelStateCommsFailure( unsigned int channelIndex );


        /** 
          * receivedChannelStateNotCommsFailure
          *
          * description
          *
          */
        void receivedChannelStateNotCommsFailure( unsigned int channelIndex ); 
        

        /** 
          * receivedChannelStateActive
          *
          * description
          *
          * @param 
          */
        void receivedChannelStateActive( unsigned int channelIndex );


        /** 
          * receivedChannelStateNotActive
          *
          * description
          *
          * @param 
          */
        void receivedChannelStateNotActive( unsigned int channelIndex );


        /** 
          * setDataPointsAsBadValue
          *
          * description
          *
          */
        void setDataPointsAsBadValue();

        void setDataPointsAsGoodValue();


        /** 
          * isConnected
          *
          * description
          *
          *
          * @return 
          */
        bool isConnected();


        /** 
          * isThereActiveSwitchChannel
          *
          * description
          *
          * @param 
          *
          * @return 
          */
        bool isThereActiveSwitchChannel( unsigned int& activeChannelIndex );


        /** 
          * commandNotToBeProcessed
          *
          * description
          *
          * @param 
          */
        void commandNotToBeProcessed( RequestCommandSharedPtr command );


        /** 
          * commandNotToBeProcessed
          *
          * description
          *
          * @param 
          */
        void commandNotToBeProcessed( NotificationCommandSharedPtr command );



        /** 
          * clearCommandQueues
          *
          * description
          */
        void clearCommandQueues();

		void openTriggeringEventAlarms(const TA_COE::AlarmStack& alarmsToBeOpened);

		void closeTriggeringEventAlarms(const TA_COE::AlarmStack& alarmsToBeClosed);

        /** 
          * canbeManuallyClosed
          *
          * TD16201
          */
		bool canbeManuallyClosed(const TA_COE::AlarmState& alarmState);
		void infoAlarmChanged(const TA_COE::AlarmStack& alarmStack);
		bool locationIsStation(std::string locationName);

		bool verifyIsOpenAlarm(const TA_COE::AlarmState& alarm);

        //
        // Private Types
        //

        struct CommunicationsChannelInfo
        {
            ECommunicationChannelState state;
            unsigned short numberCyclesWithoutActiveChannel;
        };

        typedef SynchronisedQueueWithSemaphore< RequestCommandSharedPtr > RequestCommandQueue;
        typedef SynchronisedQueueWithSemaphore< NotificationCommandSharedPtr > NotificationCommandQueue;
        typedef std::vector< TA_Base_Core::IEntityData* > VideoSwitchData;
        typedef std::vector< COECommunicationsChannel* > CommunicationsChannels;
        typedef std::vector< CommunicationsChannelInfo > CommunicationsChannelsInfo;
        
        //
        // Private member variables
        //

        static const std::string VIDEO_SWITCH_TYPE; // The type to use when retrieving the VideoSwitch data.
        static const unsigned short NO_ACTIVE_SWITCH_CYCLE_ALLOWANCE; // Num cycles before alarm that no switch 
                                                                      // has nominated itself as the Active one. 

		//19444

        TA_Base_Core::VideoSwitchAgentPtr m_videoSwitchAgentData;
		TA_Base_Core::ILocation::ELocationType m_locationType;
        unsigned long m_switchConnectTimeout;
        unsigned long m_switchPollInterval;
        unsigned long m_maxSwitchResponseTime;
		unsigned long m_summaryPollingInterval;
		unsigned long m_maxSummaryPollingResponseTime;
        unsigned long m_lowPriorityStateValidationCycle;
        unsigned long m_highPriorityStateValidationCycle;
        VideoSwitchData m_videoSwitchData; // The VideoSwitch database objects providing address information for each comms channel to be
                                           // created.
        CommunicationsChannels m_communicationsChannels;
        CommunicationsChannelsInfo m_communicationsChannelsInfo;
        unsigned int m_activeSwitchChannelIndex;
        bool m_exceededNoActiveSwitchCycleAllowance;
        volatile bool m_doTerminate; // Flag indicating if when thread method (run) should terminate.
        TA_Base_Core::ReEntrantThreadLockable m_commsChannelsStateLock; // Synchronise access to methods effecting or requiring knowledge of
        TA_Base_Core::ReEntrantThreadLockable m_threadStateLock; // Synchronise access to methods effecting or requiring knowledge of
        RequestCommandQueue m_clientRequestCommandQueue; // Client Request Commands to be processed
        RequestCommandQueue m_internalRequestCommandQueue; // Internal Request Commands to be processed
        NotificationCommandQueue m_notificationCommandQueue; // Notification Commands to be processed
        std::auto_ptr< ReceivedDataProcessor > m_receivedDataProcessor;
		TA_COE::AlarmStack m_alarmStack;
		// Alarm entity
		TA_IRS_Core::IVideoTriggeringEvents m_videoTriggeringEvents;
		TA_Base_Core::IEntityDataList m_cctvAlarmEntities;
		std::map<unsigned long, TA_Base_Core::IEntityData*> m_entityKeyToCCTVAlarm;
		typedef std::pair<TA_Base_Core::IEntityData*, TA_IRS_Core::IVideoTriggeringEvent*> AlarmEntityVideoTriggeringEventPair; 
        std::map<TA_COE::AlarmState, AlarmEntityVideoTriggeringEventPair>             m_alarmToEntityAndVideoTriggeringEvent;
        typedef std::pair<unsigned long, unsigned long> MonitorKeyIdPair;
        std::map<unsigned long, MonitorKeyIdPair>                           m_consoleKeyToMonitorKeyId;
		std::map<TA_COE::AlarmState, unsigned long>							m_alarmToWallMonitorKeyMap;	
		std::set<unsigned long>											    m_stationAlarmMonitorSet, m_occAlarmMonitorSet;
		// Station id alarm monitor key map, only used by OCC
		std::map<unsigned long, unsigned long>                              m_stationIdAlarmMonitorKeyMap;
		struct AlarmID
		{
			std::string alarmType;
			unsigned long entityKey;
			std::string alarmID; // TD16164
			unsigned long alarmLocationKey;
		};
		std::map<TA_COE::AlarmState, AlarmID>				m_openAlarms; // TD15853 modify second value to AlarmType
		TA_Base_Core::ReEntrantThreadLockable m_openAlarmsLock; // Synchronise access to methods effecting or requiring knowledge of
        TA_Base_Core::IEntityDataList m_videoMonitorEntities;
		//std::auto_ptr< TA_Base_Core::AlarmHelper > m_alarmHelper; // The AlarmHelper is used to submit alarms regarding connection status.
                                                               // Use an auto_ptr, because we are responsible for cleaning up the AlarmHelper.
		CommandsProcessor<RequestCommandSharedPtr>* m_clientRequestProcessor;
		CommandsProcessor<RequestCommandSharedPtr>* m_internalRequestProcessor;
		CommandsProcessor<NotificationCommandSharedPtr>* m_notificationProcessor;

		int m_alarmCounter;
        
    }; // COECommunicationsHandler

} // TA_IRS_App
    
#endif // !defined(COECommunicationsHandler_6D856C3B_635C_4e91_8C91_072E38E337FD__INCLUDED_)
