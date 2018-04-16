/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioOperations/RadioOperations.h $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * Encapsulates radio business logic to drive various radio operations, and to
 * coordinate gui logic. 
 */

#if !defined(AFX_RADIOOPERATIONS_H__7B7C35EA_A65F_417B_A8A8_4BE3DA928B73__INCLUDED_)
#define AFX_RADIOOPERATIONS_H__7B7C35EA_A65F_417B_A8A8_4BE3DA928B73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

#include "ace/Singleton.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include "ace/Null_Mutex.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/IRadioOperationsObserver.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioOperationsObserverHelper.h"
#include "app/radio/radio_manager_gui/src/RadioUserDefinedEvents.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"
#include "core/message/types/MessageTypes.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/NameValuePair.h"
#include "app/radio/radio_manager_gui/src/TrainAgentInterface/ITrainAgentIF.h"
#include "bus/trains/traincorbaproxy/src/traininformationcorbaproxy.h"





class RadioOperations : public IRadioOperationsObserver
{
	public:
		RadioOperations();
		virtual ~RadioOperations();

	public:
		void apiFailedMode();
		void connectionFail();
		void initialise();
		bool getResource_isActive(RadioResource& resource);
		PrivateCallState getResource_isInPrivateCall(RadioResource& resource);

		GroupConnectionState getResource_isGroupConnected(RadioResource& resource);
		bool getResource_isInAmbienceListeningCall(RadioResource& resource);
		bool getResource_isBeingMonitored(RadioResource& resource);
		/**
		 * Retrieves set of Resource types that support dynamic new entity creation.
		 */
		std::vector<RadioResourceType> getCreateableResourceTypes();
        
		virtual void onResourceAssignmentEvent(RadioResource& resource, unsigned int buttonRef);
		virtual void onResourceDeAssignmentEvent(RadioResource& resource);
		/**
		 * Handles business logic when a ResourceVolume Status, Warning, Error event is
		 * received. 
		 */
		virtual void onVolumeChangedEvent(RadioResource& resource, unsigned int volume);
		virtual void onSelectSpeakerSelectedEvent(RadioResource& resource);
		virtual void onSelectSpeakerUnselectedEvent(RadioResource& resource);
		/**
		 * Handles business logic when a InboundCallStatus event is received.
		 */
		virtual void onInboundCallEvent(RadioResource& resource);
		virtual void onInboundCallCanceledEvent(RadioResource& resource);
		virtual void onPrivateCallRinging(RadioResource& resource);
		virtual void onPrivateCallEstablished(RadioResource& resource);
		virtual void onPrivateCallEnded(RadioResource& resource);
		virtual void onPrivateCallProcessed(RadioResource& resource);
		virtual void onPrivateCallRejected(RadioResource& resource);
		/**
		 * Handles business logic when an InboundPTTId event is received. 
		 */
		virtual void onInboundPTTEvent(RadioResource& listener,RadioResource& resource);
		virtual void onInboundPTTReleasedEvent(RadioResource& resource);
		virtual void onResourceActivityEvent(RadioResource& resource);
		virtual void onResourceActivityEndedEvent(RadioResource& resource);
		/**
		 * Handles business logic when a ResourceEmergency event is received. 
		 * TD17224 - added a RadioResource parameters to determine the Radio unit that triggered the emergency
		 */
		virtual void onResourceEmergencyEvent(RadioResource& resource,RadioResource& radioUnit);
		virtual void onResourceEmergencyAcknowledgedEvent(RadioResource& resource,RadioResource& radioUnit);
		virtual void onResourceEmergencyEndedEvent(RadioResource& resource,RadioResource& radioUnit);

		/**
		 * Handles business logic when a PredefinedMessage update event is received. 
		 */
		//virtual void onPredefinedMessageEvent(RadioUpdateMessageType eventType, PredefinedMessage* messageData);

		/**
		 * Handles business logic when an Incoming Text message is received. 
		 */
		virtual void onIncomingTextMessageEvent(MessageInbox* messageData,RadioResource& resource);
		
		/**
		 * Handles business logic when a AmbienceListening event is received. 
		 */
		virtual void onAmbienceListeningEvent(RadioResource& resource, AmbienceListeningCallState state);

        virtual void onGroupConnectionStateEvent(RadioResource& resource, GroupConnectionState pState);
        
		/**
		 * The application is in Operation Mode
		 */
		void operationMode();
		/**
		 * The application is in Edit Mode
		 */
		void editMode(RadioResource& resource, bool bOnDispatchNotReady=false);
		/**
		 * A new entity is being created
		 */
		void newEntityBeingCreated(RadioResource& newResource);
//TD18869
		void entityBeingDeleted(RadioResourceType rType);
//TD18869
		void entitySelectionChange(RadioResource& resource, bool selected);
		bool locateSelectSpeaker();
		bool saveEntity(RadioResource& resource, std::vector<RadioResource>& members);
		void deleteEntity(RadioResource& resource);
		bool assignResource(RadioResource& resource, unsigned int buttonRef);
		bool deAssignResource(RadioResource& resource);
		void makePrivateCall(RadioResource& resource);
		void makeAmbienceListeningCall(RadioResource& resource);
		void answerPrivateCall(RadioResource& resource);
		/**
		 * Ends either Private Call or Ambience Listening Call.
		 */
		void endPrivateCall(RadioResource& resource);
		void selectSpeaker(RadioResource& resource);
		void unselectSpeaker(RadioResource& resource);
		void changeVolume(RadioResource& resource, unsigned int volume);
		bool connectGroup(RadioResource& resource);
		bool releaseGroup(RadioResource& resource);
		void makeEmergencyCall(RadioResource& resource);
		void endEmergencyCall(RadioResource& resource);
		void ackEmergencyCall(RadioResource& resource);
		bool callForwarding(bool cond, unsigned long forwardingISSI, unsigned long forwardedISSI);
		bool cancelCallForwarding(unsigned long ISSI);
		/**
		 * An Audit Event has occurred.
		 * This method is used for PredefinedTextMessaage updates
		 */
		void auditEventPosted(const TA_Base_Core::MessageType& messageType,const TA_Base_Core::DescriptionParameters& descParams);
		/**
		 * An Audit Event has occurred.
		 * Fixed TD#14297 - add radio resource parameter for HistoryPage
		 */
		void auditEventPosted(const TA_Base_Core::MessageType& messageType,const TA_Base_Core::DescriptionParameters& descParams,RadioResource& resource);
		
		void databaseReadyEvent();

		void launchInboxPage(); //TD16926
		
		bool includeCall(RadioResource& resource, RadioResource& dynResource);

		void makeDriverCall(unsigned long trainId);

		void makeDriverCall(RadioResource& resource);

		void answerDriverCall(RadioResource& resource);

		void endDriverCall(RadioResource& resource);

		void entityEmptySelection(RadioResourceType type);

		void startSubsMonitoring(RadioResource& resource);
		
		void stopSubsMonitoring(RadioResource& resource);

		void setCallForwardingState(short state);

		short getCallForwardingState();

		void setTrainProxy(TA_IRS_Bus::TrainInformationCorbaProxyPtr trainAgent);

		void onIncomingDriverCall(unsigned long trainId);

		void onResetDriverCall(unsigned long trainId,bool bFailure = false);

		bool connectDynamicRegroup(RadioResource& talkgroupResource, std::vector<RadioResource>& oldMembers, std::vector<RadioResource>& newMembers);

		bool releaseDynamicRegroup(RadioResource& dynResource, std::string talkgroupAlias, unsigned long radioId);

		void cancelDeleteDynGroup(RadioResource& dynResource, std::string talkgroupAlias, std::vector<RadioResource>& members, std::vector<RadioResource>::iterator errorMember);

		void cancelConnectDynGroup(std::string talkgroupAlias, std::vector<RadioResource>& members, std::vector<RadioResource>::iterator errorMember);

		void cancelReleaseDynGroup(std::string talkgroupAlias, std::vector<RadioResource>& members, std::vector<RadioResource>::iterator errorMember);

		unsigned long isDriverCall(unsigned long id);

		void setNoDriverCall(unsigned long id);

		void cancelApiOperation(ApiState state, RadioResource resource);

		// For Profile TG
		void notifyProfileUpdate(std::vector<std::string>& tgs);
		
		std::string getProfileTgName(short butRef);

		bool isProfileTgExist(const RadioResource& res);

		// For DB Update
		void notifyDbChange(RadioResource& oldResource, RadioResource& newResource);
		void notifyDbUpdate(RadioResource& oldResource, RadioResource& newResource);
		void notifyDbDelete(RadioResource& oldResource);
		void notifyDbNew(RadioResource& newResource);
		void notifyDbFinish();

		void removeResource(RadioResource& resource);

		enum DGNA_AuditMsgType
		{
			CREATE_DYNAMIC_GROUP=0,
			RELEASE_DYNMIC_GROUP
		};

		// For DGNA Audit Message
		void sendDGNAAuditMsge(RadioResource& resource,DGNA_AuditMsgType DGNAMsgeType);

	protected:
		///**
		// * A PredefinedMessage item have been added by another operator.
		// */
		//void predefinedMessageAdded(PredefinedMessage* messageData);
		///**
		// * A PredefinedMessage item have been modified by another operator.
		// */
		//void predefinedMessageModified(PredefinedMessage* messageData);
		///**
		// * A PredefinedMessage item have been removed by another operator. 
		// */
		//void predefinedMessageRemoved(PredefinedMessage* messageData);

	private:
        //disable method
        RadioOperations (const RadioOperations& radioObj);
        void operator=(const RadioOperations& radioObj);

        // to avoid allocating memory each time we notify the observers,
        // we specify a fixed size resource list, and use them sequentially.
        RadioResource* getNextResource(RadioResource& resource);


	// Attributes
	private:
		//concurrency guards for public methods
		static TA_Base_Core::ReEntrantThreadLockable m_lock;

        enum
        {
            MAX_RESOURCE_SIZE = 64
        };

        RadioResource m_resourceList[MAX_RESOURCE_SIZE];

        int m_resourceListCurrentIndex;

		TA_Base_Core::AuditMessageSender* m_auditSender;

		std::string m_session;

		unsigned long m_entityKey;

		bool connectFail;
		bool connected;
		bool notifyConFail;

		short m_CallForwardingState;

		std::vector<std::string> m_profileTgName;

		
		TA_IRS_Bus::TrainInformationCorbaProxyPtr m_trainAgent;	
		TA_IRS_App::ITrainAgentIF * m_trainAgentIF;

		std::map<unsigned long, unsigned long> driverCallState;

	public:
		void initialiseMode();

		RadioOperationsObserverHelper m_observerHelper;
		

};

typedef ACE_Singleton<RadioOperations, ACE_Null_Mutex> The_RadioOperations;


#endif // !defined(AFX_RADIOOPERATIONS_H__7B7C35EA_A65F_417B_A8A8_4BE3DA928B73__INCLUDED_)
