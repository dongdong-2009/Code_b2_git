#ifndef STISMANAGER_H_INCLUDED
#define STISMANAGER_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/STISManager.h $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <string>

#include "core/corba/src/ServantBase.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/naming/src/NamedObject.h"
#include "core/message/src/CommsMessageSender.h"

#include "bus/signs_4669/TisManagerCorbaDef/src/ISTISManagerCorbaDef.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/generic_agent/src/IEntity.h"

#include "app/signs/tis_agent/src/STISMessage.h"
#include "app/signs/tis_agent/src/MessageLibrarySynchroniser.h"
#include "app/signs/tis_agent/src/MessageLibraryUpgrader.h"
#include "app/signs/tis_agent/src/MessageLibraryManager.h"
#include "app/signs/tis_agent/src/STISCommunicationsHandler.h"
#include "app/signs/tis_agent/src/STISCurrentDisplayingMessageRequest.h"
#include "app/signs/tis_agent/src/RATISMessenger.h"
#include "app/signs/tis_agent/src/STISStatusRequestQueue.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "bus/ats/ats_agent/IDL/src/IAtsTisCorbaDef.h"


namespace TA_Base_Core
{
	class AlarmHelper;
	class NonUniqueAlarmHelper;
}

namespace TA_IRS_Core
{
    class IRATISMessage;
}

namespace TA_IRS_App
{
    class STISStatusRequest;

	typedef struct
	{
		std::string station; // only used by OCC agent
		std::string shortPIDName;
		std::string dataPointName;
		std::string address;
		TA_Base_Bus::DataPoint* dataPoint;
	} DataPointDetails;

	class STISManager : public virtual POA_TA_Base_Bus::ISTISManagerCorbaDef,
                        public virtual TA_Base_Core::ServantBase,
						public virtual TA_Base_Bus::IEntity
	{



	public:

		enum EMessageType
		{
			PREDEFINED_DISPLAY_REQUEST,
			ADHOC_DISPLAY_REQUEST,
			CLEAR_REQUEST,
			CURRENT_DISPLAY_MESSAGE_REQUEST,
			CONFIGURATION_DATA_REQUEST,
			TRAIN_INFORMATION_MESSAGE,
			APPROVED_RATIS_DISPLAY_REQUEST,
			UPGRADE_PREDEFINED_MESSAGE_LIBRARY_REQUEST,
			OCC_STATUS_REQUEST,
			STATION_STATUS_REQUEST
		};

		/**
		  * STISManager
		  *
		  * Destructor
		  *
		  * @return     none.
		  *
		  */
		virtual ~STISManager();

		/**
		  * getInstance
		  *
		  * Gets the one and only STISManager.
		  *
		  * @return     SwitchManager&
		  *             The only SwitchManager.
		  */
		static STISManager* getInstance();


		// Interface methods

		/**
          * getAllIncomingRATISMessages
          *
          * Allows the banner to retrieve the list of existing 
		  * incoming RATIS messages
		  *
          */
		TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageList* getAllIncomingRATISMessages();

		/**
          * getIncomingRATISMessage
          *
          * Allows the banner to retrieve the details for
		  * a specific incoming RATIS message after receiving notification
		  * that a new message has been received from the RXH
		  *
          */
		TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageDetails* getIncomingRATISMessage( CORBA::Long messageID );

		/**
		  * submitPredefinedDisplayRequest
		  *
		  * Called by other agents and the STIS Manager GUI.
		  * Sends a predefined display request message (M10) to the STIS Server.
		  *
		  * @return      nothing.
		  * @param       Details of the message to be displayed.
		  */
		virtual void submitPredefinedDisplayRequest( const TA_Base_Core::STISDestinationList& destinationList,
													 TA_Base_Core::ELibrarySection librarySection,
													 CORBA::UShort libraryVersion,
													 unsigned short messageTag,
													 const char* startTime, const char* endTime,
													 CORBA::UShort priority, const char* sessionID );

		/**
		  * submitAdHocDisplayRequest
		  *
		  * Called by other agents and the STIS Manager GUI.
		  * Sends an Ad Hoc display request message (M11) to the STIS Server.
		  *
		  * @return      nothing.
		  * @param       Details of the message to be displayed.
		  */
		virtual void submitAdHocDisplayRequest( const TA_Base_Core::STISDestinationList& destinationList, 
												const char* messageContent,
												const char* startTime, const char* endTime,
												CORBA::UShort priority, const TA_Base_Core::DisplayAttributes& displayAttributes,
												const TA_Base_Core::PlasmaAttributes& plasmaAttributes, const TA_Base_Core::LEDAttributes& ledAttributes,
												const char* sessionID );

		/**
		  * submitClearRequest
		  *
		  * called by the STIS Manager when an operator wishes to clear a message 
		  * from one or more PIDs.
		  * Sends a clear request message (M23) to the STIS server.
		  *
		  * @return     nothing.
		  * @param      PID to be cleared.  Priority range to clear.
		  */
		virtual void submitClearRequest( const TA_Base_Core::STISDestinationList& destinationList, 
			CORBA::UShort upperPriority, CORBA::UShort lowerPriority, const char* sessionID );

		/**
		  * upgradeToNewPredefinedMessageLibrary
		  *
		  * The STIS Manager calls this method on the OCC TIS Agent.
		  * The OCC TIS Agent will then call the method on each Station TIS Agent.
		  *
		  * @return     nothing
		  * @param      The library version to upgrade to.
		  */
		virtual void upgradePredefinedStationMessageLibrary( CORBA::UShort newLibraryVersion, const char* sessionID );

		/**
		  * upgradePredefinedTrainMessageLibrary
		  *
		  * The OCC TIS Agent will then call the method on each Station TIS Agent.
		  *
		  * @return     nothing
		  * @param      The library version to upgrade to.
		  */
		virtual void upgradePredefinedTrainMessageLibrary( CORBA::UShort newLibraryVersion, const char* sessionID );

		/**
		  * getCurrentDisplayingMessage
		  *
		  * This method is called when an operator wishes to view the current
		  * displaying message on a particular PID.
		  *
		  * @return     pointer to a string containing the current displaying message.
		  * @param   Identifier of the PID to be queried.
		  */
        virtual TA_Base_Core::CurrentDisplayingMessage* getCurrentDisplayingMessage( const TA_Base_Core::STISDestination& destination );

        /**
          * submitRATISVettingResponse
          *
          * This method allows RATISMessenger to indicate the
		  * approval or rejection of an incoming RATIS message.
		  *
		  * Note: This functionality is only available in the OCC TIS agent.
          *
          * @param key The id of the incoming message
          * @param approved Set this to true if the message is approved
          * @param priority If the message is overridable AND the operator approved the message AND this is 
          *        set to a non-zero value, then this value will be used in the submission.
          * @param content If the message is overridable AND the operator approved the message AND this is not 
          *        a clear message AND this is set to a non-empty string, then this value will be used in the submission.
          * @param sessionId This is required by the agent to determine whether the session has rights to perform this action
          */
        virtual void submitRATISVettingResponse( long key, bool approved, short priority, const std::string& content, const std::string& sessionID );

		/**
          * submitRATISVettingResponse
          *
          * This method allows clients (RATIS Approval app) to indicate the
		  * approval or rejection of an incoming RATIS message.
          * 
          * The message will be placed in a queue and processed by another thread.
		  *
		  * Note: This functionality is only available in the OCC TIS agent.
          *
          * @param key The id of the incoming message
          * @param approved Set this to true if the message is approved
          * @param priority If the message is overridable AND the operator approved the message AND this is 
          *        set to a non-zero value, then this value will be used in the submission.
          * @param content If the message is overridable AND the operator approved the message AND this is not 
          *        a clear message AND this is set to a non-empty string, then this value will be used in the submission.
          * @param sessionId This is required by the agent to determine whether the session has rights to perform this action
          */
		virtual void submitRATISVettingResponse( CORBA::Long key, CORBA::Boolean approved, CORBA::UShort priority, const char* content, const char* sessionID );


        /**
          * reApproveVettedRATISMessage
          *
          * This method allows the tis log viewer to resubmit a ratis vetting response.
          *
          * Note: This functionality is only available in the OCC TIS agent.
          *
          * @param ratisMessage A structure with all the message details (note vetting and overridable will be se to false in this method)
          * @param sessionId This is required by the agent to determine whether the session has rights to perform this action
          */
        virtual void reApproveVettedRATISMessage( const TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageDetails& ratisMessage,
                                                  const char* sessionID );
        
        /**
		  * submitRATISDisplayRequest
		  *
		  * Called by the STIS Manager when an operator wishes to send
		  * a RATIS message to other transportation lines via the RATIS Exchange Hub
		  *
		  * @return     nothing.
		  * @param      Details of the RATIS message
		  */
		virtual void submitRATISDisplayRequest(const char* messageContent, CORBA::UShort priority, 
			const char* RATISTag, const char* RATISDestination, const char* startTime, 
			const char* endTime, TA_Base_Core::ERATISMessageType type, CORBA::Boolean overridable, 
			CORBA::Boolean vetting, const char* sessionID) ;
		 
		/**
          * reportStationLibraryVersionReceived
          *
          * Used to check whether a station has received the specified new Station library
		  * version via Oracle data replication
		  * 
          */
        virtual CORBA::Boolean reportStationLibraryVersionReceived( CORBA::UShort newVersion );

		/**
          * reportTrainLibraryReceived
          *
          * Used to check whether a station has received the specified new Train library
		  * version via Oracle data replication
		  * 
          */
        virtual CORBA::Boolean reportTrainLibraryVersionReceived( CORBA::UShort newVersion );

		/**
          * isStationLibrarySynchronisationComplete
          *
          * Used by the STIS Manager to establish whether it should
		  * enable the 'upgrade ISCS' button
		  * 
          */
		CORBA::Boolean isStationLibrarySynchronisationComplete();

		/**
          * isTrainLibrarySynchronisationComplete
          *
          * 
		  * 
          */
		CORBA::Boolean isTrainLibrarySynchronisationComplete();

		/**
          * submitPIDControlRequest
          *
          * Used to turn a PID on or off
		  *
		  * @param	destination	The full PID name DBG.TIS.Platform.PDP02
		  * @param controlCommand	On or off
		  * @param sessionID	The users session
          */
		void submitPIDControlRequest( const char* destination, TA_Base_Core::EPIDControl controlCommand, const char* sessionID );

        TA_Base_Core::TimeScheduleVersion getCurrentTrainTimeScheduleVersion();

        void setCurrentTrainTimeScheduleVersion( const TA_Base_Core::TimeScheduleVersion& scheduleVersion, const char* sessionID );

		void submitUpgradePredefinedMessageRequest( int libraryVersion );

		// IEntity methods

		virtual bool isValid();

		virtual void start();

		virtual void stop();

		virtual void update( const TA_Base_Core::ConfigUpdateDetails& configUpdateDetails );

		virtual void remove();

		virtual void setToControlMode();

		virtual void setToMonitorMode();

		// Other methods

		/* initialise
		 *
		 */
		void initialise( TA_Base_Core::IEntityDataPtr EntityData );

		unsigned long getNewSequenceNumber();

		void activate();

		/**
		  * getOperatorNameFromSession
		  *
		  * Returns the name of the operator for a particular 
		  * session ID
		  * 
		  * @return     std::string - name of the operator
		  * @param       
		  */
		std::string getOperatorNameFromSession( const std::string& currentSessionID );

		/**
		  * processResponseMessage
		  *
		  * Sends an M30 Station Status request message to the STIS server
		  * to establish the status of Station TIS resources
		  * 
		  * @return      nothing
		  * @param       
		  */
		void processResponseMessage( const std::vector<unsigned char>& message );

		/**
		  * submitStationStatusRequestMessage
		  *
		  * Adds an M30 Station Status request message to a queue for later sending.
		  * 
		  * @return      nothing
		  * @param       
		  */
		void submitStationStatusRequestMessage();

		/**
		  * submitOCCStatusRequestMessage
		  *
		  * Adds an M31 OCC Status request message to a queue for later sending.
		  * 
		  * @return      nothing
		  * @param       
		  */
		void submitOCCStatusRequestMessage();

        /**
		  * handleStationStatusRequestMessage
		  *
		  * Sends an M30 Station Status request message, or an M31 OCC Status request message,
          * to the STIS server to establish the status of Station or OCC TIS resources.
		  * 
		  * @param  displayRequest   The message to send.
		  */
		void handleStatusRequestMessage(STISStatusRequest* displayRequest);

		/**
		  * submitTrainInformationMessage
		  *
		  * Sends an M09 Train Information 'Request' message to the STIS server
		  * 
		  * @return      nothing
		  * @param       
		  */
		void submitTrainInformationMessage( 
            TA_IRS_Bus::StationIdType stationId,
            TA_IRS_Bus::TisPlatformIdType platformId, 
			const TA_IRS_Bus::IAtsTisCorbaDef::PlatformInfo& platformInfo, 
			const TA_IRS_Bus::IAtsTisCorbaDef::TrainInfoPair& trainInfoPair );

		/**
		  * submitConfigurationDataRequest
		  *
		  * Sends an M00 Configuration Data Request message to the STIS server
		  * 
		  * @return      nothing
		  * @param       
		  */
		void submitConfigurationDataRequestMessage();

		/**
		  * processIncomingRATISMessage
		  *
		  * Used to add incoming RATIS messages (received from the RXH via the STIS server)
		  * to the incoming communications status banner call list, or
		  * convert to display requests if operator approval is not required
		  * 
		  * Note:  This function is only available at the OCC
		  *
		  * @return      nothing
		  * @param       
		  */
		void processIncomingRATISMessage( const TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageDetails& incomingMessage,
                                          std::string sessionId = "" );

		/**
		  * newMessageLibraryReady
		  *
		  * Called by the message library manager to indicate
		  * that the new predefined message library has been parsed and is 
		  * in the OCC database
		  * 
		  * Note:  This function is only available at the OCC
		  *
		  * @return      nothing
		  * @param       
		  */
		void newMessageLibraryReady( int newVersion, TA_IRS_App::EMessageLibraryType type );

		/**
		  * updateATSTrainInformation
		  *
		  * Supplies new ATS Train Information when the ATS detects a change
		  * the STISManager can then send the information on to the STIS server
		  *
		  * @return      nothing
		  * @param       
		  */
		void updateATSTrainInformation();

		/**
		  * sendMessageWithWait
		  *
		  * Sends a message object to the STIS server
		  * 
		  * Returns when either the timeout period occurs
		  *  or when the response is received
		  *
		  * @return      The sequence number of the message.
		  * @param       The message object to be sent.
		  */
		void sendMessageWithWait( STISMessage* message );

		/**
		  * sendMessageWithoutWait
		  *
		  * Sends a message object to the STIS server
		  * 
		  * Returns when the message is sent - does not require an ACK message
		  *
		  * @return      The sequence number of the message.
		  * @param       The message object to be sent.
		  */
		void sendMessageWithoutWait( STISMessage* message );

		/**
		  * deleteMessageFromPendingList
		  *
		  * Removes a message object from the list of
		  * pending messages.
		  *
		  * @return      nothing
		  * @param       Sequence number of the message object
		  */
		void deleteMessageFromPendingList( int sequenceNumber );

		/**
		  * processNewMessageLibrary
		  *
		  * Asks the STISManager to process a new Station or Train 
		  * Predefined Message Library
		  *
		  * @return      nothing
		  * @param       Sequence number of the message object
		  */
		void processNewMessageLibrary( TA_IRS_App::EMessageLibraryType type, const std::string& filename, int indicatedVersion );


		/**
		  * isConnectedToSTISServer
		  *
		  * Allows other threads to check whether the agent is connected to the
		  * STIS server before attempting to send messages
		  *
		  * @return      nothing
		  * @param       Sequence number of the message object
		  */
		bool isConnectedToSTISServer();

		std::string getLocationName();

        TA_Base_Core::ILocation::ELocationType getLocationType();

		unsigned long getLocationKey();

		void updatePIDStatus( int pidAddress, int status );

		void addDataPoint( DataPointDetails* theDataPoint );

        /** 
          * getPIDAddress
          *
          * Returns the PID address for the given shortname and station
          *
          * @param pidName e.g 'PDP03'
          * @param station
          *
          * @return The PID address for the given shortname and station
          *
          */
		int getPIDAddress( const std::string& pidName, const std::string& station );

		unsigned short getCurrentSTISMessageLibraryVersion();

		unsigned short getCurrentTTISMessageLibraryVersion();

		unsigned short getNextSTISMessageLibraryVersion();

		unsigned short getNextTTISMessageLibraryVersion();

		//hongran++ TD17500
		unsigned short getCurrentCDBSTISMessageLibraryVersion();

		unsigned short getCurrentCDBTTISMessageLibraryVersion();

		unsigned short getNextCDBSTISMessageLibraryVersion();

		unsigned short getNextCDBTTISMessageLibraryVersion();
		//++hongran TD17500
		
		void connectToServer();
		
		void synchroniseStationLibrary();
		void synchroniseTrainLibrary();

		void upgradeStationLibrary(const std::string& sessionID);
		void upgradeTrainLibrary(const std::string& sessionID);

		void setBadDataPointQuality();

		void messageLibrarySynchronised( TA_IRS_App::EMessageLibraryType type );

		void messageLibraryUpgraded( TA_IRS_App::EMessageLibraryType type );

		bool trainLibrarySynchInProgress();
		bool stationLibrarySynchInProgress();

		bool trainLibraryUpgradeInProgress();
		bool stationLibraryUpgradeInProgress();


        /** 
          * tokenizePIDName
          *
          * splits a pid or level name on the '.' characters.
          *
          * @param pidName  The full asset name eg DBG.TIS.Platform.PDP01 or DBG.TIS.Platform
          *
          * @return a vector of the strings between the '.' characters
          */
        std::vector<std::string> tokenizePIDName(std::string pidName);

		void submitAuditMessage( const TA_Base_Core::MessageType& type, const TA_Base_Core::DescriptionParameters& desc, const char* sessionID );

		void submitAlarm( const TA_Base_Core::MessageType& type, const TA_Base_Core::DescriptionParameters& dp );

		void closeAlarm( const TA_Base_Core::MessageType& type );

		/**
          * setLockStatus
          *
          * Locks/unlocks the specified PID so that it cannot be included in a display request.
		  * database primary key.
          *
          * @param name the full name of the PID, e.g. DBG.TIS.Platform.PDP01 
          * @param lock Set this to true if the PID is to be set to locked.
          * @param sessionID The sessionID for determining if the session has appropriate rights
          *        to set the lock status for the PID
          */
        virtual void setLockStatus(const char* name, bool lock, const char* sessionID );

		/**
          * isRATISVettingOn
          *
          * Returns true if the RATIS messages require vetting.
          *
          */
        virtual bool isRATISVettingOn();

		/**
          * setRATISVetting
          *
          * Sets the RATIS vetting to the specified condition.
          *
          */
        virtual void setRATISVetting(bool on, const char* sessionID);


        /** 
          * getSTISDestinationFromRATISDestination
          *
          * Given a RATIS destination string (destined for STIS)
          * get the destination station, levels and PIDs to be logged as part of the display request.
          *
          * @param destination          the RATIS destination string
          * @param destinationStation   the station name (output)
          * @param destinationLevels    the level names (output)
          * @param destinationPIDs      the PID names (output)
          *
          */
        void getSTISDestinationFromRATISDestination( const std::string& destination,
                                                     std::string& destinationStation,
                                                     std::string& destinationLevels,
                                                     std::string& destinationPIDs ) const;

		//TD9978
		//zhou yuan++
		std::string getNewTimestampString();
		//++zhou yuan


	private:

		/**
		  * constructor
		  *
		  * @return     nothing.
		  *
		  */
		STISManager();

        /** 
          * approveRATISClearMessage
          *
          * Submit a RATIS clear request
          *
          * @param message
          * @param priority
          * @param sessionID
          *
          */
        void approveRATISClearMessage(TA_IRS_Core::IRATISMessage* message, const short priority, const std::string& sessionID);


        /** 
          * approveRATISDisplayMessage
          *
          * Submit a RATIS display request
          *
          * @param message
          * @param priority
          * @param content
          * @param sessionID
          *
          */
        void approveRATISDisplayMessage(TA_IRS_Core::IRATISMessage* message, const short priority, const std::string& content, const std::string& sessionID);

        
        /** 
          * submitClearRequest
          *
          * Submits a clear request
          *
          * @param destination
          * @param priority
          * @param tag
          *
          */
        void submitClearRequest(const std::string& destination, const short priority, const std::string& tag, const TA_Base_Core::ERATISMessageType type, const bool overridable, const std::string& operatorName);


        /** 
          * sendRatisSubmittedAuditMessage
          *
          * Submits a STISRatisSubmitted audit message
          *
          */
        void sendRatisSubmittedAuditMessage(bool vetted, TA_Base_Core::ERATISMessageType type,
            const std::string& message, const std::string& destination, const std::string& tag,
            int priority, const std::string& startTime, const std::string& endTime, bool override, 
            const std::string& sessionID, std::string timeStamp = ""); // TD 9981 add timestamp


        /** 
          * sendRatisVettingAuditMessage
          *
          * Submits a STISRatisVetting audit message
          *
          * @param vettingOn
          * @param sessionID
          */
        void sendRatisVettingAuditMessage(const bool vettingOn, const std::string& sessionID);


        /** 
          * sendRatisProcessedAuditMessage
          *
          * Submits a STISRatisProcessed audit message
          *
          */
        void sendRatisProcessedAuditMessage(TA_Base_Core::ERATISMessageType type, const std::string& message,
            bool approved, const std::string& destination, const std::string& tag, int priority,
            const std::string& startTime, const std::string& endTime, bool override, const std::string& sessionID,
			std::string timestamp = "");

        // TD 16367
		/** 
          * sendRatisOngoingAuditMessage
          *
          * Submits a STISRatisOngoing audit message
          *
          */
        void sendRatisOutgoingAuditMessage(TA_Base_Core::ERATISMessageType type, const std::string& message,
            const std::string& destination, const std::string& tag, int priority,
            const std::string& startTime, const std::string& endTime, bool override, const std::string& sessionID,
			std::string timeStamp = "");
       // TD 16367

        /** 
          * sendPIDStatusChangedAuditMessage
          *
          * Submits a STISPidStatusChanged audit message
          *
          * @param pid
          * @param isOn
          *
          */
        void sendPIDStatusChangedAuditMessage(const std::string& pid, const bool isOn, const std::string& sessionID);


        /** 
          * getFullDestinationString
          *
          * Put the destination string into a more readable form, e.g. "All PIDs at all stations", "DBG: LED01"
          *
          * @param destination The destination string which will be in the form of something like "CCLCVC000"
          *
          * @return 
          */
        std::string getFullDestinationString(const std::string destination) const;


        /** 
          * getClearType
          *
          * Returns the priority group for TTIS clearing messages that the specified priority falls into.
          *
          * @param priority
          *
          * @return the priority group for TTIS clearing messages that the specified priority falls into.
          */
        TA_Base_Core::ETTISClearType getClearType(const short priority) const;


        /** 
          * getPriorityMsg
          *
          * Returns the priority message for STIS clearing messages that the specified priority falls into.
          *
          * @param priority
          *
          * @return the priority message for STIS clearing messages that the specified priority falls into.
          */
		std::string getPriorityMsg(const CORBA::UShort upperPriority, const CORBA::UShort lowerPriority )const;

		//TD9978
		//zhou yuan++
		void logRatisClearMessage(TA_IRS_Core::IRATISMessage* message, const short priority, const std::string& sessionID);
		//++zhou yuan

		//hongran++ TD17500
		// When it is OCC TisAgent, then use these two functions to upgrade version.
		void upgradeOCCStationLibraryVersion(unsigned short version);
		void upgradeOCCTrainLibraryVersion(unsigned short version);
		//++hongran TD17500


		//
		// The one and only instance of this class.
		//
		static STISManager* m_theClassInstance;
		
		//
        // Protect singleton creation
        //
		static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;

		// Thread to examine station library synchronisation
		MessageLibrarySynchroniser* m_stationMessageLibrarySynchroniser;

		// Thread to examine train library synchronisation
		MessageLibrarySynchroniser* m_trainMessageLibrarySynchroniser;
		
		// Thread to perform station library upgrades
		MessageLibraryUpgrader* m_stationMessageLibraryUpgrader;

		// Thread to perform train library upgrades
		MessageLibraryUpgrader* m_trainMessageLibraryUpgrader;

		//MessageLibraryManager* m_messageLibraryManager;

		// Name to register with the Naming Service
		std::string m_entityName;

		unsigned long m_entityKey;

		unsigned long m_entityTypeKey;

		unsigned long m_subsystemKey;

		// IP Address of the STIS server
		std::string m_serverIPAddress;

		int m_serverPort;

		// Location of this agent
		std::string m_locationName;
		unsigned long m_locationKey;
		TA_Base_Core::ILocation::ELocationType m_locationType;

		// Filename of the Predefined Message Library on the STIS server
		std::string m_stationMessageLibraryFilename;

		// Path of the Predefined Message Library on the STIS server
		std::string m_stationMessageLibraryPath;

		// Filename of the Predefined Message Library on the STIS server
		std::string m_trainMessageLibraryFilename;

		// Path of the Predefined Message Library on the STIS server
		std::string m_trainMessageLibraryPath;

		// Oracle Service name of the local database
		// used to connect explicitly to that instance
		std::string m_localDBString;

		// TIS Protocol Message timeout (in milliseconds)
		int m_messageTimeout;

		int m_messageRetries;

		bool m_stationLibrarySynchronisationInProgress;
		bool m_trainLibrarySynchronisationInProgress;

		bool m_stationLibraryUpgradeInProgress;
		bool m_trainLibraryUpgradeInProgress;

		// Pending messages are stored here, until the response message has
		// been received and processed (if necessary).
		std::vector<STISMessage*> m_pendingMessages;

		// Lock to guard the list of pending messages
		TA_Base_Core::ReEntrantThreadLockable m_messageLock;

		// Used to ensure that new sequence numbers are always unique
		TA_Base_Core::ReEntrantThreadLockable m_sequenceNumberLock;

		// Only process one incoming message at a time
		TA_Base_Core::ReEntrantThreadLockable m_incomingMessageLock;

		// Only create one new timestamp at any point in time
		TA_Base_Core::ReEntrantThreadLockable m_timestampLock;

		// Protect the train time schedule version number
		TA_Base_Core::ReEntrantThreadLockable m_scheduleVersionLock;

		// Only one synchronisation at a time please
		TA_Base_Core::ReEntrantThreadLockable m_stationLibrarySynchronisationLock;

		// Only one upgrade at a time please
		TA_Base_Core::ReEntrantThreadLockable m_stationLibraryUpgradeLock;

		// Only one synchronisation at a time please
		TA_Base_Core::ReEntrantThreadLockable m_trainLibrarySynchronisationLock;

		// Only one upgrade at a time please
		TA_Base_Core::ReEntrantThreadLockable m_trainLibraryUpgradeLock;

		// Protect the incoming RATIS message cache
		TA_Base_Core::ReEntrantThreadLockable m_incomingRATISLock;



		// This is used by the MessageLibrarySynchroniser
		int m_stationLibrarySynchronisationTimeout;

		// Handles socket communication with the STIS server
		STISCommunicationsHandler m_communicationsHandler;

        // Sequence number of the most recently sent message
        // this is incremented each time a message is sent
        // it is initialised to 0 when the agent starts (first message is sent with seq# 1)
        // once it reaches 9999 (defined in the TIS protocol)
        // it wraps around and starts again at 1
        int m_sequenceNumber;

		MessageLibraryManager* m_messageLibraryManager;

		// List of (virtual) Datapoints hosted by this agent
		std::vector< TA_IRS_App::DataPointDetails* > m_pidVirtualDataPointList;

		TA_Base_Bus::RightsLibrary*          m_rightsLibrary;	
        TA_Base_Core::AuditMessageSender*     m_auditMessageSender;
		//TD18093,jianghp
		//TA_Base_Core::AlarmHelper& m_alarmHelper;
		//TD18093,jianghp
		
        //TD10905: Used to send non-unique alarms
		TA_Base_Core::NonUniqueAlarmHelper& m_nonUniqueAlarmHelper;

		TA_Base_Core::CommsMessageSender*     m_tisMessageSender;

        RATISMessenger m_ratisMessenger;

        bool m_needVetting;

        TA_Base_Core::STISEntityDataPtr m_stisEntityData;

        // Queue to hold a list of (OCC or station) status request messages.
        STISStatusRequestQueue m_statusRequestQueue;

		bool m_raisedAtsForwardingFailureAlarm; //TD15198 azenitha
	};

	//chenlei++ use multi-thread to perform some process during failover
	class STISFailoverThread : public TA_Base_Core::Thread
	{
	public:
		STISFailoverThread() {};
		~STISFailoverThread() {};
		
		virtual void run();
		virtual void terminate() {};
	};


}

#endif // STISMANAGER_H_INCLUDED
