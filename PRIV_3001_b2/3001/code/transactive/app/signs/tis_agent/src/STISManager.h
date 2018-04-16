#ifndef STISMANAGER_H_INCLUDED
#define STISMANAGER_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/signs/tis_agent/src/STISManager.h $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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
#include "app/signs/tis_agent/src/STISStatusRequestQueue.h"
#include "app/signs/tis_agent/src/STISStatusProcessQueue.h"

namespace TA_Base_Core
{
	class AlarmHelper;
	class NonUniqueAlarmHelper;
}

namespace TA_Base_Bus
{
	class DataPoint;
}

namespace TA_IRS_App
{
    class STISStatusRequest;
	class LibrarySyncFailedManager;
	template<EMessageLibraryType N> class WriteLibraryToLocalDB;
	typedef WriteLibraryToLocalDB<TRAIN_LIBRARY> TtisLibraryLocalDBWriter;
	typedef WriteLibraryToLocalDB<STATION_LIBRARY> StisLibraryLocalDBWriter;

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
													 CORBA::ULong libraryVersion,
													 unsigned short messageTag,
													 const char* startTime, const char* endTime,
													 CORBA::UShort priority, const char* sessionID );

		virtual void submitPredefinedDisplayRequest2( const TA_Base_Core::STISDestinationList& destinationList,
													 TA_Base_Core::ELibrarySection librarySection,
													 CORBA::ULong libraryVersion,
													 unsigned short messageTag,
													 CORBA::ULong startTime, CORBA::ULong endTime,
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
        virtual void submitAdHocDisplayRequest( const ::TA_Base_Core::STISDestinationList& destinationList,
												const char* messageContent,
												const char* startTime,
												const char* endTime,
												::CORBA::UShort priority,
												const ::TA_Base_Core::DisplayAttributes& displayAttributes,
												const ::TA_Base_Core::LCDAttributesSTIS& lcdAttributes,
												const ::TA_Base_Core::LEDAttributes& ledAttributes,
												const char* sessionID);

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
			CORBA::UShort highPriority, CORBA::UShort lowPriority, const char* sessionID );

		/**
		  * upgradeToNewPredefinedMessageLibrary
		  *
		  * The STIS Manager calls this method on the OCC TIS Agent.
		  * The OCC TIS Agent will then call the method on each Station TIS Agent.
		  *
		  * @return     nothing
		  * @param      The library version to upgrade to.
		  */
		virtual void upgradePredefinedStationMessageLibrary( CORBA::ULong newLibraryVersion, const char* sessionID );

		/**
		  * upgradePredefinedTrainMessageLibrary
		  *
		  * The OCC TIS Agent will then call the method on each Station TIS Agent.
		  *
		  * @return     nothing
		  * @param      The library version to upgrade to.
		  */
		virtual void upgradePredefinedTrainMessageLibrary( CORBA::ULong newLibraryVersion, const char* sessionID );

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
          * reportStationLibraryVersionReceived
          *
          * Used to check whether a station has received the specified new Station library
		  * version via Oracle data replication
		  * 
          */
        virtual CORBA::Boolean reportStationLibraryVersionReceived( CORBA::ULong newVersion );

		/**
          * reportTrainLibraryReceived
          *
          * Used to check whether a station has received the specified new Train library
		  * version via Oracle data replication
		  * 
          */
        virtual CORBA::Boolean reportTrainLibraryVersionReceived( CORBA::ULong newVersion );

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

		void submitUpgradePredefinedMessageRequest( unsigned long libraryVersion );

	
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
		void submitOCCStatusRequestMessage(bool isTimeScheduleValid = false);

        // Only sent by OCC TisAgent
		void submitAllStationsStatusRequestMessage();


		void submitCCTStatusRequest();

		void submitCCTLibraryReceivedNotiyRequest(unsigned long libraryVersion);
		

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
		void newMessageLibraryReady( unsigned long newVersion, TA_IRS_App::EMessageLibraryType type );

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
		void processNewMessageLibrary( TA_IRS_App::EMessageLibraryType type, const std::string& filename, unsigned long indicatedVersion );


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

		bool isConnectedToCCT();

		std::string getLocationName();

        TA_Base_Core::ILocation::ELocationType getLocationType();

		unsigned long getLocationKey();

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

		::CORBA::ULong getCurrentSTISMessageLibraryVersion();

		::CORBA::ULong getCurrentTTISMessageLibraryVersion();

		::CORBA::ULong getNextSTISMessageLibraryVersion();

		::CORBA::ULong getNextTTISMessageLibraryVersion();

		::CORBA::ULong getCurrentCDBSTISMessageLibraryVersion();

		::CORBA::ULong getCurrentCDBTTISMessageLibraryVersion();

		::CORBA::ULong getNextCDBSTISMessageLibraryVersion();

		::CORBA::ULong getNextCDBTTISMessageLibraryVersion();
		
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
			void STISManager::tokenizePIDName(std::string pidName, std::vector<std::string>& parts);

		void submitAuditMessage( const TA_Base_Core::MessageType& type, const TA_Base_Core::DescriptionParameters& desc, const char* sessionID );

		void submitAlarm( const TA_Base_Core::MessageType& type, const TA_Base_Core::DescriptionParameters& dp );

		void closeAlarm( const TA_Base_Core::MessageType& type );

		void submitTimeScheduleDownloadRequest( const char* sessionID);		
	   
		virtual CORBA::Boolean saveSTISLibraryToLocalDB(CORBA::ULong newversion, const TA_Base_Bus::ISTISManagerCorbaDef::LibraryContentType& content);
		virtual CORBA::Boolean saveTTISLibraryToLocalDB(CORBA::ULong newversion, const TA_Base_Bus::ISTISManagerCorbaDef::LibraryContentType& content);		
		virtual TA_Base_Bus::ISTISManagerCorbaDef::LibraryContentType* getSTISLibraryDBContent( CORBA::ULong version );
		virtual TA_Base_Bus::ISTISManagerCorbaDef::LibraryContentType* getTTISLibraryDBContent( CORBA::ULong version );

        std::string getLocationDisplayName(const std::string& locationName);
	private:

		/**
		  * constructor
		  *
		  * @return     nothing.
		  *
		  */
		STISManager();

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


		// When it is OCC TisAgent, then use these two functions to upgrade version.
		void upgradeOCCStationLibraryVersion(unsigned long version);
		void upgradeOCCTrainLibraryVersion(unsigned long version);


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

		MessageLibraryManager* m_messageLibraryManager;

		// Name to register with the Naming Service
		std::string m_entityName;

		unsigned long m_entityKey;

		unsigned long m_entityTypeKey;

		unsigned long m_subsystemKey;

		// IP Address of the STIS server
		std::string m_serverIPAddress;
		int m_serverPort;

		// CCT IP Address and port
		std::string m_cctIPAddress;
		int m_cctPort;

		// sftp related parameters
		std::string m_stisSftpIPAddress;
		int m_stisSftpPort;
		std::string m_stisSftpUserName;
		std::string m_stisSftpUserPassword;

		std::string m_cctSftpIPAddress;
		int m_cctSftpPort;
		std::string m_cctSftpUserName;
		std::string m_cctSftpUserPassword;

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

		// Only one synchronisation at a time please
		TA_Base_Core::ReEntrantThreadLockable m_stationLibrarySynchronisationLock;

		// Only one upgrade at a time please
		TA_Base_Core::ReEntrantThreadLockable m_stationLibraryUpgradeLock;

		// Only one synchronisation at a time please
		TA_Base_Core::ReEntrantThreadLockable m_trainLibrarySynchronisationLock;

		// Only one upgrade at a time please
		TA_Base_Core::ReEntrantThreadLockable m_trainLibraryUpgradeLock;

		// This is used by the MessageLibrarySynchroniser
		int m_stationLibrarySynchronisationTimeout;

		// Handles socket communication with the STIS server
		STISCommunicationsHandler m_communicationsHandler;
		
		// handle socket communication with the CCT
		CCTCommunicationsHandler m_cctCommunicationsHandler;

        // Sequence number of the most recently sent message
        // this is incremented each time a message is sent
        // it is initialised to 0 when the agent starts (first message is sent with seq# 1)
        // once it reaches 9999 (defined in the TIS protocol)
        // it wraps around and starts again at 1
        int m_sequenceNumber;


		TA_Base_Bus::RightsLibrary*          m_rightsLibrary;	
        TA_Base_Core::AuditMessageSender*     m_auditMessageSender;
		
		TA_Base_Core::CommsMessageSender*     m_tisMessageSender;

        TA_Base_Core::STISEntityDataPtr m_stisEntityData;

        // Queue to hold a list of (OCC or station) status request messages.
        STISStatusRequestQueue m_statusRequestQueue;
        STISStatusProcessQueue m_statusProcessQueue;

		// Library synchronization
		TtisLibraryLocalDBWriter* m_ttisDBSyn;
		StisLibraryLocalDBWriter* m_stisDBSyn;

		// For library file content cach
		std::map< unsigned long, TA_Base_Bus::ISTISManagerCorbaDef::LibraryContentType_var > m_stisDBfiles;
		std::map< unsigned long, TA_Base_Bus::ISTISManagerCorbaDef::LibraryContentType_var > m_ttisDBfiles;
		// Lock to guard the map access
		TA_Base_Core::ReEntrantThreadLockable m_stisDBfilesLock;
		TA_Base_Core::ReEntrantThreadLockable m_ttisDBfilesLock;

		LibrarySyncFailedManager* m_librarySynFailedManager;
		std::map<std::string, int> m_pidShortNameToAddressMap;

		bool m_isPaTisSyncBroadcastIgnoreVersion;
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
