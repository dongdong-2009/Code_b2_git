/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioOperations/RadioSystemStatus.h $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#if !defined(AFX_RADIOSYSTEMSTATUS_H__43C3BF57_D3B0_4614_A317_0A359C45B79E__INCLUDED_)
#define AFX_RADIOSYSTEMSTATUS_H__43C3BF57_D3B0_4614_A317_0A359C45B79E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <list>
#include <vector>
#include <string>

#include "ace/Singleton.h"
#include "ace/Null_Mutex.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "app/radio/radio_mft_agent/src/RadioAgentDAIImpl.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentCommandServant.h"
#include "app/radio/radio_manager_gui/src/RadioMFTAgentCallbackSubscriber.h"

#include "app/radio/radio_manager_gui/src/RadioOperations/TcpMethodTimeoutException.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/TcpException.h"

#include "bus/radio/radio_agent/idl/src/RadioMFTAgentCommandIDL.h"
#include "bus/radio/radio_agent/idl/src/RadioServerAgentIDL.h"
#include "bus/radio/radio_agent/idl/src/CommonRadioAgentIDL.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"
#include "core/naming/src/NamedObject.h"

#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "bus/trains/TrainCommonLibrary/src/CallTypes.h"

#include "omniorb4/CORBA.h"

//#include "app/radio/radio_manager_gui/src/RadioDirectory/CRadioDirectoryPageDlg.h"



class RadioSystemStatus  : public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
{
	public:

		RadioSystemStatus();
		virtual ~RadioSystemStatus();

	public:

        //void setMFTCommandServant(RadioMFTAgentCommandServant* servant);

        void beginEmergency(RadioResource& resource);
        void acknowledgeEmergency(RadioResource& resource);
        void knockdownEmergency(RadioResource& resource);

		/**
		 * Update the provided new/updated entity
		 */
		bool updateEntity(RadioResource& resource, std::vector<RadioResource>& members);
		/**
		 * Delete the provided entity
		 */
		bool deleteEntity(RadioResource& resource);
		/**
		 * Locate the Resource that is currently monitored on the Select Speaker
		 */
		RadioResource locateSelectSpeaker();
		/**
		 * Associate a resource with a resource button
		 */
		bool addResourceButtonConfig(RadioResource& resource, int nProfile, unsigned int buttonRef);
		/**
		 * Remove associate between a resource and a resource button.
		 */
		bool removeResourceButtonConfig(RadioResource& resource, int nProfile, unsigned int buttonRef);
		/**
		 * Assign the resource to be monitored
		 */
		bool assignResource(RadioResource& resource, unsigned int buttonRef);
		/**
		 * De-assign the resource from being monitored.
		 */
		bool deAssignResource(RadioResource& resource);
		/**
		 * Make a private call to the Radio resource.
		 */
		bool makePrivateCall(RadioResource& resource);
		/**
		 * Answer an incoming private call from a Radio resource.
		 */
		int answerPrivateCall(RadioResource& resource);
		/**
		 * End a private call with a Radio resource.
		 */
		bool endPrivateCall(RadioResource& resource);
		/**
		 * Make an Ambience Listening call to the Radio resource.
		 */
		bool makeAmbienceListeningCall(RadioResource& resource);
		/**
		 * Make an Ambience Listening call to the Radio resource.
		 */
		bool endAmbienceListeningCall(RadioResource& resource);
		/**
		 * Select the resource to the Select Speaker.
		 */
		bool selectSpeaker(RadioResource& resource);
		/**
		 * Unselect the Selected speaker.
		 */
		bool unselectSpeaker(RadioResource& resource);
		/**
		 * Change the Volume of the resource to the specified value.
		 */
		bool changeVolume(RadioResource& resource, unsigned int volume);
		/**
		 * Connect to the resource group.
		 */
		bool connectGroup(RadioResource& resource);
		/**
		 * Release the connection to the resource group.
		 */
		bool releaseGroup(RadioResource& resource);

		/**
		 * Get all MFTResource collection
		 */
		std::vector<RadioResource> getResourceCollectionAll();

		/**
		 * Get MFTResource collection by type
		 */
		std::vector<RadioResource> getResourceCollectioForType(RadioResourceType type);

		/**
		 * Get MFTResource status
		 */
		GroupConnectionState getResource_isGroupConnected(RadioResource& resource);
		bool getResource_isActive(RadioResource& resource);
		PrivateCallState getResource_isInPrivateCall(RadioResource& resource);
		void setResource_isInPrivateCall(RadioResource& resource, PrivateCallState statePrivateCall);
		bool getResource_isInAmbienceListeningCall(RadioResource& resource);
		bool getResource_isBeingMonitored(RadioResource& resource);
		bool getResource_isSelected(RadioResource& resource);
		bool getResource_isAssigned(RadioResource& resource);

		RadioEmergencyModeType getResource_isEmergencyMode(RadioResource& resource);

		/**
		 * Get MFTResource volume on select/unselect speaker
		 */
		int getResourceVolume(RadioResource& resource);
		
		/**
		 * Get Predefined-message list from DAI
		 */
		//std::vector<PredefinedMessage> getPredefinedMessageList();

		/**
		 * Add Predefined-message thru DAI
		 */
		//bool addPredefinedTextMessage(PredefinedMessage& predefinedMessageData);
		/**
		 * Modify Predefined-message thru DAI
		 */
		//bool modifyPredefinedTextMessage(PredefinedMessage& predefinedMessageData);
		/**
		 * Delete Predefined-message thru DAI
		 */
		//bool removePredefinedTextMessage(PredefinedMessage& predefinedMessageData);

		/**
		 * Send a text message to the specified resource
		 */
		//bool sendTextMessage(RadioTextMessageData& messageData,RadioResource& recepient);

		/**
		 * Get all inbox messages from DAI
		 */
		//std::vector<MessageInbox> getHistoricalTextMessages();

		/**
		 * Get members of specified group resource
		 */
		std::vector<RadioResource> getResourceMembers(RadioResource& resource);
		/**
		 * Get resources monitored by the operator
		 */
		std::vector<ResourceButton> getMonitoredResource();
		/**
		 * Get resources by SSI
		 */
		RadioResource getResourceBySSI(unsigned long ssi);
		/**
		 * Get resources by Resource Id
		 */
		RadioResource getResourceByRID(unsigned long rid,RadioResourceType type=RR_NIL);
		/**
		 * Get resources by Resource Alias
		 */
		RadioResource getResourceByAlias(std::string alias,RadioResourceType type=RR_NIL);
		/**
		 * Set text message as READ
		 */
		//void updateTextMessageReadState(unsigned long key, bool flag);

		void setSession(std::string& session) {	m_session = session; };

		
		void registerToMFT(RadioMFTAgentCallbackSubscriber* pMFTAgentCallbackSubscriber);

		/*
		 * Get configuration paramater values
		 */
		int getConfigValue(const std::string& name);

		/*
		 * Create a temporary patch group with BIM resource
		 * Used for DLTS-Radio patching
		 * Fixed TD#13911
		 * 
		 * @param Radio resource to patch
		 * @param true if Radio-DLTS patch, false if Radio-EPAX patch
		 */
		//void createPatchGroup(const RadioResource& resource, bool isDLTSPatch=true);

		/*
		 * Release the connected patch group with BIM/SPI Phone resource
		 * Used for DLTS & EPAX patching with Radio
		 * Fixed TD#13911
		 */
		//void releasePatchGroup(bool isDLTSPatch);

		/*
		 * Method to get the allocated DG Talkgroup on DynamicRegroup connection
		 *
		 * Fixed TD#14320
		 */
		RadioResource getDGTalkgroup(const RadioResource& resource);

		/*
		 * Method to get the allocated DG Talkgroup on DynamicRegroup connection
		 *
		 * Fixed TD#14294
		 */
		void makePhoneCall(const char* number,int length);
		void endPhoneCall(const char* number);

		//TD16870
		void setCommandServant(RadioMFTAgentCommandServant* pServant) 
		{ 
			m_MFTCommandCorbadef = pServant;
		};
		//TD18785
		void addSelectSpeakerResource(RadioResource& resource);
		void deleteSelectSpeakerResource(RadioResource& resource);
		void addActiveResource(RadioResource& resource);
		void deleteActiveResource(RadioResource& resource);
		//TD18785


		MonitorStatus requestRadioCheck(RadioResource& resource);

		bool connectDynamicRegroup(RadioResource& resource, std::string talkgroupAlias);

		bool releaseDynamicRegroup(RadioResource& resource, std::string talkgroupAlias);

		std::vector<std::string> updateOperatorProfile(std::string opAlias);

		bool callForwarding(bool cond, unsigned long forwardingISSI, unsigned long forwardedISSI);

		bool cancelCallForwarding(unsigned long ISSI);

		CallForwardingStatus queryCallForwarding(unsigned long ISSI);

		void addRadioResource(RadioResource& resource, std::vector<RadioResource>& members, unsigned long id);

		void showErrorMessageFromRadio();

		void restartConnection();

		int getResourceCounter(RadioResource& resource);

		void updateGX(std::string message);

		void updateResource(const RadioResource& oldResource, const RadioResource& newResource);

		void addNewResource(const RadioResource& newResource);

		void deleteResource(const RadioResource& oldResource);

		void notifyEndDriverCall(const RadioResource& resource);
		
		void removeResource(RadioResource& resource);

		 /**
         * Receive message from Train Agent
         * @param payload
         */
        void receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& payload );


	private: 
        
		//disable method
        RadioSystemStatus (const RadioSystemStatus& radioObj);
        void operator=(const RadioSystemStatus& radioObj);
		
        TA_IRS_Bus::Radio::ERadioResourceType convertResourceType(RadioResourceType type);


		void connectToAgents();


	// Attributes
	private:

		//concurrency guards for public methods
		static TA_Base_Core::ReEntrantThreadLockable m_lock;

        RadioMFTAgentCommandServant* m_MFTCommandCorbadef;		// TD16870

		TA_Base_Core::NamedObject<TA_IRS_Bus::RadioMFTAgentCommandIDL,
			TA_IRS_Bus::RadioMFTAgentCommandIDL_ptr,
			TA_IRS_Bus::RadioMFTAgentCommandIDL_var> m_mftCommandCorbadef;

		TA_Base_Core::NamedObject<TA_IRS_Bus::RadioServerAgentIDL,
				TA_IRS_Bus::RadioServerAgentIDL_ptr,
					  	TA_IRS_Bus::RadioServerAgentIDL_var> m_ServerAgentCorbaDef;	

		std::string m_session;

		std::string m_consoleName;

		unsigned long m_consoleId; 

		unsigned long m_entityLocation;

		RadioMFTAgentCallbackSubscriber* m_RadioMFTAgentCallbackSubscriber;

		/**
         * Called to subscribe to updates from the Train Agent
         */
        void subscribeToMessages();


        /**
         * Called to unsubscribe to updates from the Train agent
         */
        void unsubscribeToMessages();

		void processTrainChangeOver(TA_IRS_Bus::CallTypes::TrainChangeover trainChangeover);

		bool m_subscribedToMessages;

	public:

		RadioAgentDAIImpl m_radioDAIImpl;

		static const short MAX_NUMBER_GROUP;
		static const short PROFILE_TALKGROUP_SIZE;
};

typedef ACE_Singleton<RadioSystemStatus, ACE_Null_Mutex> The_RadioSystemStatus;

#endif 
