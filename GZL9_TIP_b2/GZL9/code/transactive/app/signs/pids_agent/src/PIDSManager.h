#ifndef PIDSMANAGER_H_INCLUDED
#define PIDSMANAGER_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL13_TIP/GZL13/transactive/app/signs/pids_agent/src/PIDSManager.h $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/10/01 16:36:38 $
  * Last modified by:  $Author: grace.koh $
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
#include "core/data_access_interface/entity_access/src/PIDSEntityData.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/naming/src/NamedObject.h"
#include "core/message/src/CommsMessageSender.h"

#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/generic_agent/src/IEntity.h"

#include "app/signs/pids_agent/src/PIDSMessage.h"
#include "app/signs/pids_agent/src/PIDSCommunicationsHandler.h"
#include "app/signs/pids_agent/src/PIDSSigDisplayRequest.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"

//#include "bus/ats/ats_agent/IDL/src/IAtsPidsCorbaDef.h"
#include "bus/signs/pids_agent/IDL/src/IPIDSManagerCorbaDef.h"
//#include "app/signs/pids_agent/src/TrainAgentSubscriber.h"

namespace TA_Base_Core
{
	class AlarmHelper;
	class NonUniqueAlarmHelper;
}

namespace TA_IRS_Core
{
    class IRAPIDSMessage;
}

namespace TA_IRS_App
{
	class PIDSManager : public virtual POA_TA_Base_Bus::IPIDSManagerCorbaDef,
                        public virtual TA_Base_Core::ServantBase
//						public virtual TA_Base_Bus::IEntity
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
			APPROVED_RAPIDS_DISPLAY_REQUEST,
			UPGRADE_PREDEFINED_MESSAGE_LIBRARY_REQUEST,
			OCC_STATUS_REQUEST,
			STATION_STATUS_REQUEST
		};

		/**
		  * PIDSManager
		  *
		  * Destructor
		  *
		  * @return     none.
		  *
		  */
		virtual ~PIDSManager();

		/**
		  * getInstance
		  *
		  * Gets the one and only PIDSManager.
		  *
		  * @return     SwitchManager&
		  *             The only SwitchManager.
		  */
		static PIDSManager* getInstance();

		/**
		  * submitAdHocDisplayRequest
		  *
		  * Called by other agents and the PIDS Manager GUI.
		  * Sends an Ad Hoc display request message to the PIDS Server.
		  *
		  * @return      nothing.
		  * @param       Details of the message to be displayed.
		  */
		::CORBA::Short submitAdhocDisplayRequest(const ::TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestinationList& destinationList, 
			const ::TA_Base_Bus::IPIDSManagerCorbaDef::PidsMessage& messageContent, 
			::CORBA::Boolean msgType, 
			::CORBA::Boolean showType, 
			::CORBA::Long trainNo, 
			const ::TA_Base_Bus::IPIDSManagerCorbaDef::MessageTime& startTime, 
			const ::TA_Base_Bus::IPIDSManagerCorbaDef::MessageTime& endTime, 
			::CORBA::Long interval, 
			::CORBA::Boolean isCyclic);

		::CORBA::Short submitAdhocDisplayRequestWithMulityByteMsgContent(const ::TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestinationList& destinationList, 
			const ::TA_Base_Bus::IPIDSManagerCorbaDef::PidsMessage& messageContent, 
			::CORBA::Boolean msgType, 
			::CORBA::Boolean showType, 
			::CORBA::Boolean isTrain, 
			const ::TA_Base_Bus::IPIDSManagerCorbaDef::MessageTime& startTime, 
			const ::TA_Base_Bus::IPIDSManagerCorbaDef::MessageTime& endTime, 
			::CORBA::Long interval, 
			::CORBA::Boolean isCyclic);

		::CORBA::Short submitClearRequest(const ::TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestinationList& destinationList, 
			::CORBA::Long trainNo);

	    virtual void submitSigRequest(const ::TA_Base_Bus::IPIDSManagerCorbaDef::SigData& data);

		// IEntity methods

		virtual bool isValid();

		virtual void start();

		virtual void stop();

		virtual void update( const TA_Base_Core::ConfigUpdateDetails& configUpdateDetails );

		virtual void remove();

		virtual void setToControlMode();

		virtual void setToMonitorMode();

		void setEntityName( std::string entityName );

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
		  * updateATSTrainInformation
		  *
		  * Supplies new ATS Train Information when the ATS detects a change
		  * the PIDSManager can then send the information on to the PIDS server
		  *
		  * @return      nothing
		  * @param       
		  */
		void updateATSTrainInformation();

		std::string getLocationName();

        TA_Base_Core::ILocation::ELocationType getLocationType();

		unsigned long getLocationKey();

		void updatePIDStatus( int pidAddress, int status );

		unsigned long getEntityKey();
		unsigned long getEntityKeyType();
		std::string getEntityName();
		unsigned long getSubsystemKey();





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

		void connectToServer();
		
		void setBadDataPointQuality();


        /** 
          * tokenizePIDName
          *
          * splits a pid or level name on the '.' characters.
          *
          * @param pidName  The full asset name eg DBG.PIDS.Platform.PDP01 or DBG.PIDS.Platform
          *
          * @return a vector of the strings between the '.' characters
          */
        std::vector<std::string> tokenizePIDName(std::string pidName);

		void submitAuditMessage( const TA_Base_Core::MessageType& type, const TA_Base_Core::DescriptionParameters& desc, const char* sessionID );

		void submitAlarm( const TA_Base_Core::MessageType& type, const TA_Base_Core::DescriptionParameters& dp );

		void closeAlarm( const TA_Base_Core::MessageType& type );

		//TD9978
		//zhou yuan++
		std::string getNewTimestampString();
		//++zhou yuan

		//void subscribeAtsPidUpdateMsg(unsigned long locationKey);

	private:

		/**
		  * constructor
		  *
		  * @return     nothing.
		  *
		  */
		PIDSManager();
       
        /** 
          * sendPIDStatusChangedAuditMessage
          *
          * Submits a PIDSPidStatusChanged audit message
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



		//
		// The one and only instance of this class.
		//
		static PIDSManager* m_theClassInstance;
		
		//
        // Protect singleton creation
        //
		static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;

		// Name to register with the Naming Service
		std::string m_entityName;

		unsigned long m_entityKey;

		unsigned long m_entityTypeKey;

		unsigned long m_subsystemKey;

		// IP Address of the PIDS server
		std::string m_serverIPAddress;

		int m_serverPort;

		// Location of this agent
		std::string m_locationName;
		unsigned long m_locationKey;
		TA_Base_Core::ILocation::ELocationType m_locationType;

		// Oracle Service name of the local database
		// used to connect explicitly to that instance
		std::string m_localDBString;

		// PIDS Protocol Message timeout (in milliseconds)
		int m_messageTimeout;

		int m_messageRetries;

		// Pending messages are stored here, until the response message has
		// been received and processed (if necessary).
		std::vector<PIDSMessage*> m_pendingMessages;

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

		// Handles socket communication with the PIDS server
		PIDSCommunicationsHandler m_communicationsHandler;

        // Sequence number of the most recently sent message
        // this is incremented each time a message is sent
        // it is initialised to 0 when the agent starts (first message is sent with seq# 1)
        // once it reaches 9999 (defined in the PIDS protocol)
        // it wraps around and starts again at 1
        int m_sequenceNumber;

		// List of (virtual) Datapoints hosted by this agent
//		std::vector< TA_IRS_App::DataPointDetails* > m_pidVirtualDataPointList;

		TA_Base_Bus::RightsLibrary*          m_rightsLibrary;	
        TA_Base_Core::AuditMessageSender*     m_auditMessageSender;
		//TD18093,jianghp
		//TA_Base_Core::AlarmHelper& m_alarmHelper;
		//TD18093,jianghp
		
        //TD10905: Used to send non-unique alarms
		TA_Base_Core::NonUniqueAlarmHelper& m_nonUniqueAlarmHelper;

		TA_Base_Core::CommsMessageSender*     m_PidsMessageSender;

        TA_Base_Core::PIDSEntityDataPtr m_sPidsEntityData;

        // Queue to hold a list of (OCC or station) status request messages.
//        PIDSStatusRequestQueue m_statusRequestQueue;

		bool m_raisedAtsForwardingFailureAlarm; //TD15198 azenitha

		//TrainAgentSubscriber m_tainAgentSubscriber;


	};

	//chenlei++ use multi-thread to perform some process during failover
	class PIDSFailoverThread : public TA_Base_Core::Thread
	{
	public:
		PIDSFailoverThread() {};
		~PIDSFailoverThread() {};
		
		virtual void run();
		virtual void terminate() {};
	};


}

#endif // PIDSMANAGER_H_INCLUDED
