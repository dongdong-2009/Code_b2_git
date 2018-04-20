#ifndef RadioSynchronisationTypes_h
#define RadioSynchronisationTypes_h

/**
  * The source code in this file is the property of
  * Ripples Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioSynchronisationTypes.h $
  * @author  John Dalin
  * @version $Revision: #2 $
  * Last modification : $DateTime: 2009/03/19 17:00:25 $
  * Last modified by : $Author: builder $
  *
  *
  */

#include "app/radio/RadioTetraAgent/src/RadioTypes.h"

#include <vector>
#include <string>
#include <map>
namespace TA_IRS_App
{
	
	enum ESessionState
	{
		SessionUnassigned,
		SessionAssigned,
		SessionInitialised,
		SessionLoggedIn
	};

	enum EControlConnectionStatus
	{
		NoAppliable,
		ConnectToPrimary,
		ConnectToSecondary			
	};
	
	struct RadioSessionType
	{
		unsigned long audioLink;
		SessionReference sessionRef;
		std::string sessionEntityName;
		std::string loginName;
        std::string loginPassword;
		std::string subscriberTextRef;
		std::string sessionDGNAITSI;
		bool sessionDGNAInUse;
		ESessionState sessionStatus;
		std::string sessionITSI;
	};


	struct RadioSessionServantType
	{
		unsigned long entityKey;
		RadioSessionType radioSession;
	};


	typedef std::vector<RadioSessionServantType> RadioSessionServantTypeList;


    /***********************************
     * Composite Radio State Hierarchy *
     ***********************************/

	struct MonitoredSubscriberDetailsType
	{	   	
		unsigned long monitorReference;
		//Future other variables to be added in this structure.
	};

	typedef std::map< long int, MonitoredSubscriberDetailsType> MonitoredSubscriberDetailsTypeMap;

	struct RadioMonitorManagerType
	{
		MonitoredSubscriberDetailsTypeMap monitoredSubscriberDetailsMap;
	};
	
	struct AudioEntryType
	{
		CallReference callRef;
		bool inUse;
		long volume;
	};
	
	typedef std::map<AudioReference, AudioEntryType> AudioEntryMapType;

    struct ConsoleSessionRef

    {

        SessionReference sessionRef;

        ConsoleID consoleKey;


        bool operator<(const ConsoleSessionRef& csRef) const

        {

            if(consoleKey != csRef.consoleKey)

            {

                return (consoleKey < csRef.consoleKey);

            }

            else

            {

                return (sessionRef < csRef.sessionRef);

            }

        };


        bool operator==(const ConsoleSessionRef& csRef) const

        {

            return (consoleKey == csRef.consoleKey &&

                        sessionRef==csRef.sessionRef);

        };


    };


    typedef std::map<ConsoleSessionRef, AudioEntryMapType> SessionAudioMapType;


				
	struct RadioResourceManagerType
	{
		long currentAudioVolume;
		SessionAudioMapType sessionAudioMap;
	};

	typedef std::vector<AuthorisationRequestType> AuthorisationRequestTypeList;
	
	struct RadioAuthorisationManagerType
	{
		AuthorisationRequestTypeList radioAuthorisationStack;
	};

    typedef std::vector<CallDetailsType> CallDetailsTypeList;
    
    struct TcpServerType
    {
        CallDetailsTypeList radioCallStack;
		RadioAuthorisationManagerType radioAuthorisationManager;
		RadioResourceManagerType radioResourceManager;
		RadioMonitorManagerType radioMonitorManager;
		EControlConnectionStatus controlConnectionStatus;
    };

	typedef std::map< ServerIndex, TcpServerType> RadioType;
    
    struct RadioServantType
    {
        unsigned long entityKey;
        RadioType radio;
    };
    
    struct RadioTetraAgentType
    {
        RadioServantType radioServant;
		RadioSessionServantTypeList radioSessionServants;
  };

    /*******************************
     * Radio State Update Messages *
     *******************************/
    struct RadioStateUpdateMessage
    {
        enum Type
        {
            UPDATE_CALL_IN_RADIO_CALL_STACK,
            REMOVE_CALL_FROM_RADIO_CALL_STACK,
			UPDATE_RADIO_SESSION,
			UPDATE_MONITORED_CALL_PROGRESSION,
			REMOVE_MONITORED_SUBSCRIBER_DETAILS,
			REMOVE_SESSION_FROM_AUDIO_MAP,
			UPDATE_AUDIO_ENTRY_IN_SESSION_AUDIO_MAP,
			UPDATE_RADIO_RESOURCE_MANAGER_VARS,
			UPDATE_AUTHORISATION_REQUEST,
			REMOVE_AUTHORISATION_REQUEST,
			CONNECTION_SWITCH
        };

        virtual Type getType() const = 0;
        virtual ~RadioStateUpdateMessage() {}
    };

	struct RadioConnectionSwitch : virtual public RadioStateUpdateMessage
    {
		unsigned long TCPServerIndex;
        EControlConnectionStatus controlConnectionStatus;
		
        virtual Type getType() const { return CONNECTION_SWITCH; }
        virtual ~RadioConnectionSwitch() {}
    };

    struct RadioUpdateCallInRadioCallStack : virtual public RadioStateUpdateMessage
    {
        CallDetailsType callDetails;

        virtual Type getType() const { return UPDATE_CALL_IN_RADIO_CALL_STACK; }
        virtual ~RadioUpdateCallInRadioCallStack() {}
    };

    struct RadioRemoveCallFromRadioCallStack : virtual public RadioStateUpdateMessage
    {
        CallID callID;

        unsigned long consoleID;
        virtual Type getType() const { return REMOVE_CALL_FROM_RADIO_CALL_STACK; }
        virtual ~RadioRemoveCallFromRadioCallStack() {}
    };

	struct RadioUpdateRadioSession : virtual public RadioStateUpdateMessage
	{
		unsigned long servantEntityKey;
		RadioSessionType radioSession;

		virtual Type getType() const { return UPDATE_RADIO_SESSION; }
		virtual ~RadioUpdateRadioSession() {}
	};

	struct RadioUpdateMonitoredCallProgression : virtual public RadioStateUpdateMessage
	{
		unsigned long  monitorReference;

		virtual Type getType() const { return UPDATE_MONITORED_CALL_PROGRESSION; }
		virtual ~RadioUpdateMonitoredCallProgression() {}
	};
	
	struct RadioRemoveMonitoredSubscriberDetails : virtual public RadioStateUpdateMessage
	{
		unsigned long  monitorReference;
		long		   serverIndex;


		virtual Type getType() const { return REMOVE_MONITORED_SUBSCRIBER_DETAILS; }
		virtual ~RadioRemoveMonitoredSubscriberDetails() {}
	};

	struct RadioRemoveSessionFromAudioMap : virtual public RadioStateUpdateMessage
	{
		SessionID sessionReference;
		long serverIndex;

		virtual Type getType() const { return REMOVE_SESSION_FROM_AUDIO_MAP; }
		virtual ~RadioRemoveSessionFromAudioMap() {}
	};
	
	struct RadioUpdateAudioEntryInSessionAudioMap : virtual public RadioStateUpdateMessage
	{
		SessionID sessionReference;
		AudioReference audioReference;
		AudioEntryType audio;

		virtual Type getType() const { return UPDATE_AUDIO_ENTRY_IN_SESSION_AUDIO_MAP; }
		virtual ~RadioUpdateAudioEntryInSessionAudioMap() {}
	};
	
	struct RadioUpdateRadioResourceManagerVars : virtual public RadioStateUpdateMessage
	{
		SessionID sessionReference;
		CallReference callReference;
		long currrentVolume;
		long serverIndex;

		virtual Type getType() const { return UPDATE_RADIO_RESOURCE_MANAGER_VARS; }
		virtual ~RadioUpdateRadioResourceManagerVars() {}
	};
	
	struct RadioUpdateAuthorisationRequest : virtual public RadioStateUpdateMessage
	{
		AuthorisationRequestType autorisationRequest;
		long serverIndex;

		virtual Type getType() const { return UPDATE_AUTHORISATION_REQUEST; }
		virtual ~RadioUpdateAuthorisationRequest() {}
	};
	
	struct RadioRemoveAuthorisationRequest : virtual public RadioStateUpdateMessage
	{
		CallID callID;
		long serverIndex;

		virtual Type getType() const { return REMOVE_AUTHORISATION_REQUEST; }
		virtual ~RadioRemoveAuthorisationRequest() {}
	};
			
    /** requestFullState
      *
      * This method will is used to request the Radio Tetra Agent's full state
      */
    RadioTetraAgentType requestFullState();
    
};  // Namespace TA_IRS_App

#endif
