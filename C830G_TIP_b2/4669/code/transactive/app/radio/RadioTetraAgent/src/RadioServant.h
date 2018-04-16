#ifndef RADIOSERVANT_H
#define RADIOSERVANT_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioServant.h $
  * @author  A. Parker
  * @version $Revision: #5 $
  * Last modification : $DateTime: 2009/05/18 10:25:59 $
  * Last modified by : $Author: builder $
  *
  * 
  */

#include "core/alarm/src/AlarmHelper.h"
#include "core/corba/src/ServantBase.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/RadioGlobalEntityData.h" //TD14696

#include "core/message/src/AuditMessageSender.h"

#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioCorbaDef.h"
#include "bus/trains/TrainCorbaProxy/src/TrainOperationModeCorbaProxy.h"
#include "bus/generic_agent/src/IEntity.h"

#include "app/radio/RadioTetraAgent/src/IRadioCallback.h"
#include "app/radio/RadioTetraAgent/src/RadioTypes.h"
#include "app/radio/RadioTetraAgent/src/RadioSynchronisationTypes.h"
#include "app/radio/RadioTetraAgent/src/RadioServerLinkStatusUpdateThread.h"

namespace TA_Base_Bus
{
    class ItaScada;
}

namespace TA_Base_Core
{
    class AlarmHelper;
}

namespace TA_IRS_App
{
	class Radio;
    class IRadioDirectoryCallback;

	class RadioServant : public virtual POA_TA_Base_Bus::IRadioCorbaDef,
						 public virtual TA_Base_Core::ServantBase,
						 public virtual TA_Base_Bus::IEntity,
						 public virtual TA_IRS_App::IRadioCallback
	{
		public:

        RadioServant( /*TA_Base_Bus::ItaScada& scada*/ );
			virtual ~RadioServant();

			void setEntity(TA_Base_Core::IEntityData& EntityData, TA_Base_Core::RadioGlobalEntityData& globalEntity);
            void startServerLinkStatusThread() { m_serverLinkStatusThread->start(); };//limin, called by RadioTetraAgent after running control

			TA_IRS_App::Radio& getRadio() { return *m_radio; };
			
			// IEntity Interface Functions
			bool isValid(void);
			void start(void);
			void stop(void);
			void update(const class TA_Base_Core::ConfigUpdateDetails &details);
			void remove(void);
			void setToControlMode(void);
			void setToMonitorMode(void);
			void onTerminate(bool peerAvailable);

			// Exported Agent Functions through IRadio IDL interface
			TA_Base_Bus::IRadioCorbaDef::CallList*          getCallList(ConsoleID consoleId);
			TA_Base_Bus::IRadioCorbaDef::CallDetailsType*  getCallDetails(long callID,ConsoleID consoleId);
			TA_Base_Bus::IRadioCorbaDef::ECallCategory     getCallCategory(long callID,ConsoleID consoleId);
			char *  getCallText(long callID, ConsoleID );
			virtual void sendSDSMessage(const char* trainITSI, const TA_Base_Bus::IRadioCorbaDef::MessageDataType& messageData);
			virtual void sendData(const char* ITSI, const TA_Base_Bus::IRadioCorbaDef::CircuitDataType& data, bool useXON, const char * sessionId);
			virtual void cancelSendData(const char* ITSI);
			virtual void addCallDriverRequest(unsigned char trainID,const char* trainTSI);
			virtual void removeCallDriverRequest(unsigned char trainID);

            // Fallback & Degraded IDL support
            virtual bool inFallback();
            virtual void resetConnection();

			void	setDataPointLink(TA_Base_Bus::DataPoint* dp);

			// Call back functions for Radio object
			void onCallStatusChange(const TA_IRS_App::CallDetailsType& details);
			void onTrainEvent(std::string trainTSI,std::vector<unsigned char> messageData);
			
			virtual void onTXChange(const TXStateInfo txInfo);
			virtual void updateGX(std::string sessionId, std::string message, ConsoleID consoleId );

            virtual void onSubscriberActivity(TA_IRS_App::MonitoredCallDetailsType* subscriberActivityCallDetails);
            virtual void submitAlarm( const TA_Base_Core::MessageType& type, const TA_Base_Core::DescriptionParameters& dp );
			virtual void closeAlarm( const TA_Base_Core::MessageType& type); // TD9023

            virtual void onRadioFallback(bool isFallback);

            virtual void onCircuitModeActivity(const char * ITSI, bool successful, const char * failReason);

			// Help functions

            /** Facade for the identically named Radio function
             *  @param directoryCallback the pointer to the callback interface
             */
            void setDirectoryCallback(IRadioDirectoryCallback * directoryCallback);

            // state synchronisation functions
            void applyFullState(const RadioServantType& state);
            void clearFullState();
            void getFullState(RadioServantType& state);
            void updateCallInRadioCallStack(const CallDetailsType& callDetails);
            void removeCallFromRadioCallStack(CallID callID, unsigned long consoleID);
			/**
			  * updateRadioMonitoredCallProgression
			  *
			  * @ param radioSessionData - Radio Monitored Call Progression data to update
			  *
			  * Synchronisation wth the peer agent
			  */
			void updateRadioMonitoredCallProgression(unsigned long monitorReference);

			void removeMonitoredSubscriberDetails(unsigned long monitorReference, long serverIndex);

			void removeSessionFromAudioMap(SessionID sessionReference, long serverIndex);
			
			void updateAudioEntryInSessionMap(SessionID sessionReference, 
													  AudioReference audioReference,
												      const AudioEntryType& audioEntry);
			void updateRadioResourceManagerVars( SessionID sessionReference, CallReference callReference,
												int currentVolume, long serverIndex);
			
			void updateAuthorisationRequest(const AuthorisationRequestType& authorisationRequest, long serverIndex);
			
			void removeAuthorisationRequest(CallID callID, long serverIndex);

			void updateConsoleSessionServers(unsigned long svrIdx, long consoleID, unsigned long sessionRef);
		    //*************************************************
            // IRadioDirectory - Speed Dial Interface Methods *
            //*************************************************            
            virtual TA_Base_Bus::IRadioCorbaDef::SpeedDialList* getSpeedDialSet(const char* setName);
            virtual void updateSpeedDialSet(const TA_Base_Bus::IRadioCorbaDef::SpeedDialSet& setDetails, const char* sessionID);
            virtual void updateSpeedDial(const char* setName, const TA_Base_Bus::IRadioCorbaDef::SpeedDialEntry& newEntry, const char* sessionID);
            virtual char* getUserSetName(const char* sessionID,TA_Base_Bus::IRadioCorbaDef::EUserSetTypeMatch setTypeMatch);
				//************************************
			// IRadioDirectory Interface Methods *
			//************************************
			virtual TA_Base_Bus::IRadioCorbaDef::SubscriberList*     getGroupSubscribers();
            virtual TA_Base_Bus::IRadioCorbaDef::SubscriberList*     getPatchSubscribers();
            virtual TA_Base_Bus::IRadioCorbaDef::SubscriberList*     getIndividualSubscribers();
            virtual TA_Base_Bus::IRadioCorbaDef::SubscriberDetails*  getSubscriberDetails(const char* TSI);
            virtual TA_Base_Bus::IRadioCorbaDef::SubscriberList*     getGroupMembers(const char* TSI);
            virtual TA_Base_Bus::IRadioCorbaDef::SubscriberList*     getPatchMembers(const char* TSI);
            virtual TA_Base_Bus::IRadioCorbaDef::SubscriberList*     searchSubscriber(const char* searchText);
		private:
			
			std::string         m_objectName;
			unsigned long       m_EntityKey;
			unsigned long   	m_entityLocation;
			unsigned long		m_entitySubsystem;
			unsigned long		m_entityTypeKey;
			std::string			m_assetName;

			TA_IRS_App::Radio*      m_radio;

			bool                m_active;
			//for the TD17094
            //TA_Base_Core::AlarmHelper& m_alarmHelper;

			TA_Base_Core::AuditMessageSender* m_auditMessageSender; //TD13048
            std::string         m_primaryServers;
            std::string         m_secondaryServers;

            // Raymond Pau++
            //TA_Base_Bus::ItaScada& m_scada;
            // ++Raymond Pau
			TA_IRS_Bus::TrainOperationModeCorbaProxyPtr m_trainAgentProxy;

			RadioServerLinkStatusUpdateThread*		m_serverLinkStatusThread;
	};  // Class RadioServant
}; // Namespace TA_IRS_App

#endif
