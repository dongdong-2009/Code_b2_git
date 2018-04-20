/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioSessionServant.h $
  * @author:  
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2009/05/18 10:25:59 $
  * Last modified by:  $Author: builder $
  *
  *
  */


#if !defined(RadioSessionServant_DC5CCB91_948A_42b3_9674_9CED6E32D9F2__INCLUDED_)
#define RadioSessionServant_DC5CCB91_948A_42b3_9674_9CED6E32D9F2__INCLUDED_

#include "core/corba/src/ServantBase.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/data_access_interface/entity_access/src/RadioSessionEntityData.h"
#include "core/data_access_interface/entity_access/src/RadioGlobalEntityData.h"
#include "core/message/src/CommsMessageSender.h"

#include "RadioSession.h"
#include "RadioSynchronisationTypes.h"

#include "bus/generic_agent/src/IEntity.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"  // needed for typedef
#include "bus/security/security_utilities/src/SecurityUtilities.h"
#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioSessionCorbaDef.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"

#include "app/radio/RadioTetraAgent/src/RadioEventSubscriber.h"
#include "app/radio/RadioTetraAgent/src/IRadioSessionServant.h"

namespace TA_Base_Core // Forward declarations
{
	class AuditMessageSender;
	class MessageType;
	class ISession;
    class AlarmHelper;
}

namespace TA_Base_Bus // Forward declarations
{
    class RightsLibrary; 
    class ItaScada;
	class AuthenticationLibrary;
}

namespace TA_IRS_App
{
	class Radio;
    class RadioSession;

	class RadioSessionServant : virtual public POA_TA_Base_Bus::IRadioSessionCorbaDef,
								virtual public TA_Base_Core::ServantBase, 
		                        virtual public TA_Base_Bus::IEntity,
								virtual public IRadioSessionServant,
								public virtual TA_Base_Core::SpecialisedMessageSubscriber< TA_Base_Core::CommsMessageCorbaDef >
	{

		public:
			RadioSessionServant(TA_IRS_App::Radio& radio,TA_Base_Core::RadioSessionEntityDataPtr EntityData);
            RadioSessionServant( TA_IRS_App::Radio& radio,
								 TA_IRS_App::RadioTcpServer& radioTcpServer,
                                 TA_Base_Core::RadioSessionEntityDataPtr entityData,
                                 TA_Base_Core::RadioGlobalEntityData& globalEntityData
								 /*,TA_Base_Bus::ItaScada& itaScada */
                                 );
			virtual ~RadioSessionServant();			

			unsigned long getConsoleID()
			{
				return m_consoleID;
			}
			
			/**
			  * receiveSpecialisedMessage
			  *
			  * This method is called whenever the agent receives a comms message
			  *
			  */
			void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);

			/**
			 * receiveCallFwdRequest
			 *
			 * This method is called to process a received Call Fwd Request message
			 *
			 */
			void receiveCallFwdRequest(const TA_Base_Core::CommsMessageCorbaDef& message);

			void detachSession();

			/**
			* Process the session begin and add to a new thread.
			* 
			* @param    sessionID
			*/
			void sessionBegin(std::string sessionId);
			
			/**
			* Process the session begin and add to a new thread.
			* 
			* @param    sessionID
			*/
			void sessionEnd(std::string sessionId);
			
			/**
			* Perfoms radio session login, obtaining the dynamic group ITSI.
			* 
			* @param    sessionID
			*/
			void sessionLogin(std::string sessionId);
			void sessionLogin(const std::string& loginName, const std::string& password, const std::string& sessionId);
			void sessionLogin(const char * loginName,const char * sessionId);

			/**
			* Perfoms radio session logout.
			* 
			* @param    sessionID
			*/
			void sessionLogout(const char * sessionId);

			//******************************************
			// IRadioSessionCorbaDef Interface Methods *
			//******************************************

			virtual long getSessionReference(const char * sessionId);
			virtual void answerCall(long callID, const char * sessionId);
			//TD 10234
            virtual long makeCall(const char * destinationTSI, TA_Base_Bus::IRadioSessionCorbaDef::ECallType calltype, bool isFullDuplex,TA_Base_Bus::IRadioSessionCorbaDef::ECallPriority priority, bool hook, const char * sessionId, bool isInvisibleToGUIApps, long locationKey = 0); // TD14696
			virtual void releaseCall(long callID, const char * sessionId);

			// Dynamic Groups
			virtual char * makeDynamicGroup(const TA_Base_Bus::IRadioSessionCorbaDef::DestinationTSIList& destTSIs, const char * sessionId);

			/**
			* Sends a text message to another subscriber.
			* Do not send to train!
			* @param    destinationTSI
			* @param    message
			* @param    sessionId   the radio session identifier
			*/
			virtual void sendTextMessage(const char * destinationTSI, const char * message, const char * sessionId);
			virtual void holdCall(long callId, const char * sessionId);
			virtual void resumeCall(long callId, const char * sessionId);

            /**
            * Commence monitoring the given subscriber
            * @param tsi        the ITSI of the subscriber to monitor
            * @param sessionID  the radio session identifier
            */
            virtual long startMonitoring(const char * tsi, const char * sessionID);                

            /**
            * Cease monitoring the given subscriber
            * @param tsi        the ITSI of the subscriber to monitor
            * @param sessionID  the radio session identifier
            */
            virtual void stopMonitoring(const char * tsi, const char * sessionID);

            /**
            * Provides one-shot monitoring of all the given subscriber's current
            * calls
            * @param tsi        the ITSI of the subscriber to monitor
            * @param sessionID  the radio session identifier
            */
            virtual void getActiveCalls(const char * tsi, const char * sessionID);

            /**
            * Eavesdrop on a call
            * @param callId
            * @param sessionId  the radio session identifier
            */
            virtual long startListening(long callId, const char * sessionId);

            /**
            * Cease eavesdropping
            *
            * @param callId
            * @param sessionId  the radio session identifier
            */
            virtual void stopListening(long callId, const char * sessionId);

            /**
            * Allows the operator to join a group call
            *
            * @param callId     the group call reference. Must be the original
            *                   call reference, not the one returned by the 
            *                   MonitorCall event (a.k.a. startListening())
            * @param sessionID  the radio session id
            */
            virtual long joinCall(long callId, const char * sessionId);

            /**
            * Will terminate a monitored call
            * @param callId     the reference of the call to terminate
            * @param sessionId  the radio session id
            */
            virtual void terminateCall(long callId, const char * sessionId, bool bIsMonitored);

            /**
            * Includes the given subscriber into a call between individuals
            *
            * @param callId     the individual call reference
            * @param subscriber the ITSI of the subscriber to include
            * @param sessionID  the radio session id
            */
            virtual long includeSubscriber(long callId, const char * subscriber, const char * sessionId);


			virtual void setCallForwarding(const TA_Base_Bus::IRadioSessionCorbaDef::CallForwardDetails& callForwardDetails, const char * sessionId);
			virtual TA_Base_Bus::IRadioSessionCorbaDef::CallForwardDetails* getCallForwarding();
			virtual void selectCall(long callId, const char * sessionId);
			virtual void deselectCall(long callId, const char * sessionId);
			virtual void adjustCallVolume(long callIds, bool increase, const char * SessionId);
			/**
			* Advise radio that user wishes to talk.  Used for half duplex calls.
			* @param    priority    Priority 0..3 (0=low)
			*/
			virtual void demandTX(short priority,const char * sessionId,CORBA::ULong& queuedID);
			/**
			* Used to advise radio that operator has finished talking.  Used for half duplex calls only.
			*/
			virtual void ceaseTX(const char * sessionId);

            virtual void createPatch(const TA_Base_Bus::IRadioSessionCorbaDef::PatchList& list,const char* patchName,const char * SessionId);
            virtual void deletePatch(const char* patchName,const char * SessionId);
			virtual char * getPatchOwnerITSI();
            virtual void onAuthorisationChange(const TA_IRS_App::EAction action,const TA_IRS_App::AuthorisationRequestType& details);

            virtual TA_Base_Bus::IRadioSessionCorbaDef::CallAuthorisationList* getCallAuthorisations();
            virtual void authoriseCall(long callID,bool accept,const char* sessionID);

			//*****************************************
			// IRadioSessionServant interface methods *
			//*****************************************
			
			void onSessionInitialisationFailure();
			void onSessionInitialisationComplete();
			void onSessionLoginFailure(const std::string loginName);
			void onSessionLoginComplete();
			unsigned long getEntityKey();
			void onSessionUpdate();

			//****************************
			// IEntity Interface Methods *
			//****************************
			virtual bool isValid();
			virtual void start();
			virtual void stop();
			virtual void update(const class TA_Base_Core::ConfigUpdateDetails& details);
			virtual void remove();
			virtual void setToControlMode();
			virtual void setToMonitorMode();
            
            // called when the agent is shutting down
            void onTerminate(bool peerAvailable);

			/**
			  * updateRadioSession
			  *
			  * @ param radioSessionData - Radio Session data to update
			  *
			  * Synchronisation wth the peer agent
			  */
			void updateRadioSession(const RadioSessionType& radioSessionData);
			virtual void clearFullState();
			virtual void applyFullState(const RadioSessionServantType& state);
			virtual void getFullState(RadioSessionServantType& state);

			TA_Base_Bus::AuthenticationLibrary * getAuthenticationLibrary();

		private:

			TA_IRS_App::ECallType convertCallType(TA_Base_Bus::IRadioSessionCorbaDef::ECallType callType);
			void checkPermission(std::vector<TA_Base_Bus::AccessControlledAction> actions, std::string session);
			void checkPermission(TA_Base_Bus::AccessControlledAction action, std::string session);
			void AuditOperation(const TA_Base_Core::MessageType& messageType,TA_Base_Core::DescriptionParameters parameters,
								std::string sessionId);
			void AuditOperation(const TA_Base_Core::MessageType& messageType,TA_Base_Core::DescriptionParameters parameters,
								std::string sessionId, long callID);
			std::string getCallTypeString(TA_IRS_App::ECallType callType);


			Radio&										m_radio;
			RadioTcpServer&								m_radioTcpServer;
			RadioSession*								m_radioSession;
			TA_Base_Bus::SessionUpdateSubject*			m_sessionUpdateSubject;
			RadioEventSubscriber*						m_eventSubscriber;

			std::string									m_entityName;
			unsigned long								m_entityKey;
			unsigned long								m_consoleID;
			unsigned long								m_entityLocation;
            std::string									m_entityLocationName;
			unsigned long								m_entitySubsystem;
			unsigned long								m_entityTypeKey;
			bool										m_active;
// 
// 			TA_Base_Core::ISession*						m_lastAuditISession;
// 			std::string									m_lastAuditSession;

			TA_Base_Bus::RightsLibrary*					m_rightsLibrary;	
		    TA_Base_Core::AuditMessageSender*			m_auditMessageSender;
            //TA_Base_Core::AlarmHelper&            m_alarmHelper;
            TA_Base_Core::CommsMessageSender*			m_commsMessageSender;
			TA_Base_Bus::AuthenticationLibrary			m_authenticationLibrary;

			std::string									m_transactiveSession;

	};
};

#endif // !defined(RadioSessionServant_DC5CCB91_948A_42b3_9674_9CED6E32D9F2__INCLUDED_)
