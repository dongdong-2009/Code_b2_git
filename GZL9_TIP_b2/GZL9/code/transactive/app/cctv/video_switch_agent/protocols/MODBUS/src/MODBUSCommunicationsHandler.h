
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSCommunicationsHandler.h $
  * @author:   Rob Young
  * @version:  $Revision: #5 $
  *
  * Last modification: $DateTime: 2013/08/30 15:38:55 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Handles the protocol dependent sections of the operations for a CommunicationsHandler
  * using the MODBUS protocol.
  * This interface is used by the CommunicationsHandler class.
  *
  */
#if !defined(MODBUSCommunicationsHandler_6D856C3B_635C_4e91_8C91_072E38E337FD__INCLUDED_)
#define MODBUSCommunicationsHandler_6D856C3B_635C_4e91_8C91_072E38E337FD__INCLUDED_

#include "app/cctv/video_switch_agent/protocols/src/IPDCommunicationsHandler.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSHelper.h"
#include "app/cctv/video_switch_agent/protocols/src/IPDAlarmInterface.h"
#include "app/cctv/video_switch_agent/protocols/src/IPDStageModeInterface.h"
//#include "core/data_access_interface/video_switch_agent/src/VideoStationIdAccessFactory.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/entity_access/src/VideoSwitchAgent.h"
#include <string>
#include <memory>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace TA_Base_Core
{
    class AlarmHelper;
    class IEntityData;
}

namespace TA_Base_Bus
{
	class IModbusHelper;
}

namespace TA_IRS_App
{
    class IAgentRequestCommand;
    class IHardwareNotificationCommand;
}

namespace TA_IRS_App
{

	struct ManualSequenceConfig
	{
		unsigned char cameraID;
		unsigned char stationID;
		unsigned short stayTime;
		ManualSequenceConfig(unsigned char cameraID, unsigned char stationID, unsigned short stayTime)
			:cameraID(cameraID), stationID(stationID), stayTime(stayTime)
		{
		}
	};

    class MODBUSCommunicationsHandler : public IPDCommunicationsHandler,
                                     public IPDAlarmInterface,
                                     public IPDStageModeInterface
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
          * MODBUSCommunicationsHandler
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
        MODBUSCommunicationsHandler( TA_Base_Core::VideoSwitchAgentPtr videoSwitchAgentEntityData );


        /**
          * ~MODBUSCommunicationsHandler
          *
          * Standard destructor.
          */
        virtual ~MODBUSCommunicationsHandler();


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
        void processSummaryAlarmsState( bool systemSummaryAlarm, bool transmissionSummaryAlarm, bool cameraSummaryAlarm, bool ancillarySummaryAlarm, bool dvrSummaryAlarm );


        /** 
          * processAlarmStack
          *
          * description
          *
          * @param 
          *
          */
        void processAlarmStack( TA_MODBUS::AlarmStack alarmStack );

		
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
		  *				TD15853 - changed parameters
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The alarm with the given iD is not found.
	      */
       virtual void acknowledgeAlarm( const std::string& alarmKey,/* unsigned long entityKey,*/unsigned long consoleId );
        
		/**
          * acknowledgeTrainCameraAlarm
          *
	      * acknowledges an trainborne camera alarm 
          *
          * @param      string AlarmId
          *             the transactive alarm Id associated with the alarm to be Acked
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The alarm with the given iD is not found.
	      */
       virtual void acknowledgeTrainCameraAlarm(unsigned long locationId, unsigned long trainId, const std::vector<unsigned long>& cameras, unsigned long consoleId );
		
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

	    /**
          * closeAlarm
          *
	      * closed an alarm on the videoswitch alarm stack that the switch cant close itself
          *
          * @param      string AlarmId
          *             the transactive alarm Id associated with the alarm to be closed
		  *				TD15853 - changed parameters
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The alarm with the given iD is not found.
		  *				3. The alarm not configured to be manually closed
	      */
        virtual void closeAlarm(const std::string& alarmTypeKey /*unsigned long entityKey*/);

		/**
          * closeAlarm
          *
	      * closed an alarm on the videoswitch alarm stack that the switch cant close itself
          *
          * @param      string AlarmId
          *             the transactive alarm Id associated with the alarm to be closed
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The alarm with the given iD is not found.
		  *				3. The alarm not configured to be manually closed
	      */
        virtual void closeAlarm( unsigned long stationId, unsigned long eventId );

		/**
          * getAlarmStack
          *
	      * gets all of the alarms curently open by this alarm interface
          *
          * @return     TA_Base_Bus_GZL9::SwitchManagerCorbaDef::AlarmStack&
          *             The alarm stack object
		  *
		  */
		virtual TA_Base_Bus_GZL9::SwitchManagerCorbaDef::AlarmStack getAlarmStack();

		/**
          * updateAlarmStack
          *
	      * returns all of the alarms curently open by this alarm interface
          *
          * @param     TA_Base_Bus_GZL9::SwitchManagerCorbaDef::AlarmStack& stack
          *             The alarm stack object
		  *
		  */
		virtual void updateAlarmStack(TA_Base_Bus_GZL9::SwitchManagerCorbaDef::AlarmStack& stack);

		virtual unsigned long getEntityKey();

		///////////////////////////////////////////////////////////////////////
        //
        virtual void setStageSwitchMode(unsigned long originatingStage, unsigned long destinationStage, bool allowSwitch);
        // Thread Methods
        //
        ///////////////////////////////////////////////////////////////////////

		virtual void setControlPrivilege(int privilege);
   
        void assignCameraToMonitor(unsigned short monitorID, unsigned char stationID, unsigned char cameraID);
		void assignCameraToQuad(unsigned short monitorID, unsigned char stationID1, unsigned char topRightCameraID,
			unsigned char stationID2, unsigned char topLeftCameraID,
			unsigned char stationID3, unsigned char bottomRightCameraID,
			unsigned char stationID4, unsigned char bottomLeftCameraID);
		// sequence play & stop
        void startAutoSwitch(unsigned char sequenceID, unsigned short monitorID);
        void stopAutoSwitchMode( unsigned short monitorID);
		void startManualSequence(unsigned short monitorID, const std::vector<ManualSequenceConfig>& sequence); //sequence max size is 40
		void stopManualSequence(unsigned short monitorID);
		void startQuadManualSequence(unsigned short monitorID, const std::vector<ManualSequenceConfig>& topRightSequence,
			const std::vector<ManualSequenceConfig>& topLeftSequence,
			const std::vector<ManualSequenceConfig>& bottomRightSequence,
			const std::vector<ManualSequenceConfig>& bottomLeftSequence); // sequence max size is 10
		void stopQuadManualSequence(unsigned short monitorID);
        //

    private:

        //
        // Disable copy constructor and assignment operator.
        //
        MODBUSCommunicationsHandler( const MODBUSCommunicationsHandler& theMODBUSCommunicationsHandler );
        MODBUSCommunicationsHandler& operator=( const MODBUSCommunicationsHandler& );

        //
        // Private Helper Functions
        //

        /**
         * loadCommunicationsChannels
         *
         * Creates a MODBUSCommunicationChannel for each VideoSwitch address and port number from the database.
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

		void openTriggeringEventAlarms(const TA_MODBUS::AlarmStack& alarmsToBeOpened);

		void closeTriggeringEventAlarms(const TA_MODBUS::AlarmStack& alarmsToBeClosed);

        /** 
          * canbeManuallyClosed
          *
          * TD16201
          */
		bool canbeManuallyClosed(const TA_MODBUS::AlarmState& alarmState);
		
        //
        // Private Types
        //

        typedef std::vector< TA_Base_Core::IEntityData* > VideoSwitchData;
        

        TA_Base_Core::VideoSwitchAgentPtr m_videoSwitchAgentData;
		TA_Base_Core::ILocation::ELocationType m_locationType;
        VideoSwitchData m_videoOccSwitchData,m_videoStationSwitchData; 

		boost::shared_ptr<TA_Base_Bus::IModbusHelper> m_modbusHelper;
		unsigned char m_controlPrivilege;

    }; // MODBUSCommunicationsHandler

} // TA_IRS_App
    
#endif // !defined(MODBUSCommunicationsHandler_6D856C3B_635C_4e91_8C91_072E38E337FD__INCLUDED_)
