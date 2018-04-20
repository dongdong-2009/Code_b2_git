/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioStateMessageConverter.cpp $
 * @author:  John Dalin
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/03/19 17:00:25 $
 * Last modified by:  $Author: builder $
 *
 * The RadioStateMessageConverter is a class that is used for converting CORBA message
 * structures to normal c-style structures and reverse. This is necessary to reduce coupling between
 * CORBA and the agent.
 */

#include "core/utilities/src/TAAssert.h"

#include "RadioStateMessageConverter.h"

#include "RadioCorbaTypeConverter.h"
namespace TA_IRS_App
{
    
	void RadioStateMessageConverter::convertAudioEntryCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::AudioEntry& audioEntryCorba,
		AudioEntryType& audioEntry)
	{
		audioEntry.callRef = audioEntryCorba.callRef;
		audioEntry.inUse = audioEntryCorba.inUse;
		audioEntry.volume = audioEntryCorba.volume;
		
	}
	
	void RadioStateMessageConverter::convertAudioEntryMapTypeCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::AudioEntryMapType& audioEntryMapCorba,
		AudioEntryMapType& audioEntryMap)
	{		
		audioEntryMap.clear();
		for (unsigned int i = 0; i < audioEntryMapCorba.length(); ++i)
        {
			AudioEntryType *pNewAudioEntryType = new AudioEntryType();
			convertAudioEntryCorbaToNormal(audioEntryMapCorba[i].audio, *pNewAudioEntryType);
			audioEntryMap.insert(std::pair<AudioReference,AudioEntryType>(audioEntryMapCorba[i].audioRef,*pNewAudioEntryType));			
        }		
		
	}
	
	void RadioStateMessageConverter::convertSessionAudioMapTypeCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::SessionAudioMapType& sessionAudioMapCorba,
		SessionAudioMapType& sessionAudioMap)
	{
		sessionAudioMap.clear();
		for (unsigned int i = 0; i < sessionAudioMapCorba.length(); ++i)
        {
			AudioEntryMapType *pNewAudioEntryMapType = new AudioEntryMapType();
			convertAudioEntryMapTypeCorbaToNormal(sessionAudioMapCorba[i].audioEntryMap, *pNewAudioEntryMapType);


			ConsoleSessionRef csRef;

			csRef.consoleKey = sessionAudioMapCorba[i].consoleKey;

			csRef.sessionRef = sessionAudioMapCorba[i].sessionRef;

			sessionAudioMap.insert(std::pair<ConsoleSessionRef,AudioEntryMapType>(csRef,*pNewAudioEntryMapType));

        }
		
	}
	
	void RadioStateMessageConverter::convertRadioResourceManagerCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioResourceManagerType& radioResourceManagerCorba,
                RadioResourceManagerType& radioResourceManager)
	{
		radioResourceManager.currentAudioVolume = radioResourceManagerCorba.currentAudioVolume;
		convertSessionAudioMapTypeCorbaToNormal(radioResourceManagerCorba.sessionAudioMap,radioResourceManager.sessionAudioMap);

	}

	
	void RadioStateMessageConverter::convertAuthorisationRequestTypeCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::AuthorisationRequestType& authorisationRequestCorba,
		AuthorisationRequestType& authorisationRequest)
	{
		authorisationRequest.callID =  authorisationRequestCorba.callID;
		authorisationRequest.sessionRef = authorisationRequestCorba.sessionRef;
        RadioCorbaTypeConverter::convertERadioUserTypeCorbaToNormal(authorisationRequestCorba.sourceType, authorisationRequest.sourceType);        
        RadioCorbaTypeConverter::convertERadioUserTypeCorbaToNormal(authorisationRequestCorba.destinationType, authorisationRequest.destinationType);
		authorisationRequest.callerName  = authorisationRequestCorba.callerName;
		authorisationRequest.calledName = authorisationRequestCorba.calledName;
	}
	
	
	void RadioStateMessageConverter::convertAuthorisationManagerCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioAuthorisationManagerType& radioAuthorisationManagerCorba,
		RadioAuthorisationManagerType& radioAuthorisationManager)
	{
		// convert all request type in the stack
		radioAuthorisationManager.radioAuthorisationStack.clear();
		
        for (unsigned int i = 0; i < radioAuthorisationManagerCorba.radioAuthorisationStack.length(); ++i)
        {
            AuthorisationRequestType authorisationRequest;
			convertAuthorisationRequestTypeCorbaToNormal(radioAuthorisationManagerCorba.radioAuthorisationStack[i], authorisationRequest);
            radioAuthorisationManager.radioAuthorisationStack.push_back(authorisationRequest);
        }
		
		
	}

	void RadioStateMessageConverter::convertEPTTStateCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::EPTTState& pttStateCorba,
        EPTTState& pttState)
    {
        switch (pttStateCorba)
        {
        case TA_Base_Bus::IRadioCorbaDef::PTT_GRANTED:
            pttState = PTT_GRANTED;
            break;
            
        case TA_Base_Bus::IRadioCorbaDef::PTT_QUEUE:
            pttState = PTT_QUEUE;
            break;
            
        case TA_Base_Bus::IRadioCorbaDef::PTT_CEASE:
            pttState = PTT_CEASE;
            break;
            
        case TA_Base_Bus::IRadioCorbaDef::PTT_TIMEOUT:
            pttState = PTT_TIMEOUT;
            break;
            
        default:
            ;
        }
    }
    
    void RadioStateMessageConverter::convertCallParticipantCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::CallParticipant& callParticipantCorba,
        CallParticipant& callParticipant)
    {
        callParticipant.isAcknowledged = callParticipantCorba.isAcknowledged;
        callParticipant.location = callParticipantCorba.location;
        callParticipant.name = callParticipantCorba.name;
		RadioCorbaTypeConverter::convertEPTTStateCorbaToNormal(callParticipantCorba.pttState, callParticipant.pttState);
        callParticipant.TSI = callParticipantCorba.TSI;
    }
    
	void RadioStateMessageConverter::convertRadioCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioType& radioCorba, RadioType& radio)
	{
		radio.clear();

		for ( unsigned int i = 0; i < radioCorba.length(); i ++ )
		{
			TcpServerType tcpServer;
			convertTcpServerCorbaToNormal( radioCorba[i].tcpServer, tcpServer );
			radio.insert( std::pair< ServerIndex, TcpServerType >( radioCorba[i].serverIndex, tcpServer) );
		}
	}
    
    void RadioStateMessageConverter::convertTcpServerCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::TcpServerType& tcpCorba,
        TcpServerType& tcpServer)
    {
        // convert all calls in the stack
        tcpServer.radioCallStack.clear();
        
        for (unsigned int i = 0; i < tcpCorba.radioCallStack.length(); ++i)
        {
            CallDetailsType callDetails;
            
            RadioCorbaTypeConverter::convertCallDetailsTypeCorbaToNormal(tcpCorba.radioCallStack[i], callDetails);
            tcpServer.radioCallStack.push_back(callDetails);
        }
		convertAuthorisationManagerCorbaToNormal(tcpCorba.radioAuthorisationManager, tcpServer.radioAuthorisationManager);

		convertRadioResourceManagerCorbaToNormal(tcpCorba.radioResourceManager, tcpServer.radioResourceManager);

		convertRadioMonitorManagerTypeCorbaToNormal(tcpCorba.radioMonitorManager,tcpServer.radioMonitorManager);

		convertControlConnectionStatusCorbaToNormal(tcpCorba.controlConnectionStatus, tcpServer.controlConnectionStatus);
    }

	void RadioStateMessageConverter::convertRadioMonitorManagerTypeCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioMonitorManagerType& radioMonitorCorba,
		RadioMonitorManagerType& radioMonitor)
	{
		radioMonitor.monitoredSubscriberDetailsMap.clear();	
		for(unsigned int i = 0; i < radioMonitorCorba.monitoredSubscriberDetailsAndRefList.length(); ++i)
		{
			MonitoredSubscriberDetailsType *pNewMonitoredSubscriberDetailsType = new MonitoredSubscriberDetailsType();
			convertMonitoredSubscriberDetailsTypeCorbaToNormal(radioMonitorCorba.monitoredSubscriberDetailsAndRefList[i].monitoredSubscriberDetails, *pNewMonitoredSubscriberDetailsType);
			radioMonitor.monitoredSubscriberDetailsMap.insert(std::pair<long int, MonitoredSubscriberDetailsType>(radioMonitorCorba.monitoredSubscriberDetailsAndRefList[i].monitoredSubscriberDetailsRef, *pNewMonitoredSubscriberDetailsType) );
		}
	}
											
	void RadioStateMessageConverter::convertMonitoredSubscriberDetailsTypeCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::MonitoredSubscriberDetailsType& radioMonitorSubscriberCorba,
		MonitoredSubscriberDetailsType& radioMonitorSubscriber)
	{
		radioMonitorSubscriber.monitorReference = radioMonitorSubscriberCorba.monitorReference;
	}
    
    void RadioStateMessageConverter::convertRadioServantCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioServantType& radioServantCorba,
        RadioServantType& radioServant)
    {
        radioServant.entityKey = radioServantCorba.entityKey;
        convertRadioCorbaToNormal(radioServantCorba.radio, radioServant.radio);
    }
    
	void RadioStateMessageConverter::convertRadioSessionServantCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioSessionServantType& radioSessionServantCorba,
				RadioSessionServantType& radioSessionServant)
	{
		radioSessionServant.entityKey = radioSessionServantCorba.entityKey;
		convertRadioSessionCorbaToNormal(radioSessionServantCorba.radioSession, radioSessionServant.radioSession);
	}

    void RadioStateMessageConverter::convertRadioTetraAgentCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioTetraAgentType& radioTetraAgentCorba,
        RadioTetraAgentType& radioTetraAgent)
    {
        convertRadioServantCorbaToNormal(radioTetraAgentCorba.radioServant, radioTetraAgent.radioServant);

		// convert the radio session servants
		unsigned int i;
		unsigned int length = radioTetraAgentCorba.radioSessionServants.length();

		for (i = 0; i < length; i++)
		{
			RadioSessionServantType radioSessionServant;

			convertRadioSessionServantCorbaToNormal(radioTetraAgentCorba.radioSessionServants[i],
													radioSessionServant);
			radioTetraAgent.radioSessionServants.push_back(radioSessionServant);
		}
    }
    
    void RadioStateMessageConverter::convertRadioUpdateCallInRadioCallStackCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateCallInRadioCallStack& messageCorba,

        RadioUpdateCallInRadioCallStack& message)

    {

        RadioCorbaTypeConverter::convertCallDetailsTypeCorbaToNormal(messageCorba.callDetails, message.callDetails);

    }

    void RadioStateMessageConverter::convertRadioRemoveCallFromRadioCallStackCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioRemoveCallFromRadioCallStack& messageCorba,
        RadioRemoveCallFromRadioCallStack& message)
    {
        message.callID = messageCorba.callID;
		message.consoleID = messageCorba.consoleKey;
    }
    
	void RadioStateMessageConverter::convertRadioUpdateRadioSessionCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateRadioSession& messageCorba,
				RadioUpdateRadioSession& message)
	{
		message.servantEntityKey = messageCorba.servantEntityKey;
		convertRadioSessionCorbaToNormal(messageCorba.radioSession, message.radioSession);
	}

	void RadioStateMessageConverter::convertRadioUpdateMonitoredCallProgressionCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateMonitoredCallProgression& messageCorba,
				RadioUpdateMonitoredCallProgression& message)
	{
		message.monitorReference = messageCorba.monitorReference; 
	}

	void RadioStateMessageConverter::convertRadioRemoveSessionFromAudioMapCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioRemoveSessionFromAudioMap& messageCorba,
				RadioRemoveSessionFromAudioMap& message)
	{
		message.sessionReference = messageCorba.sessionReference;
		message.serverIndex = messageCorba.serverIndex;
	}

	void RadioStateMessageConverter::convertRadioUpdateAudioEntryInSessionAudioMapCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateAudioEntryInSessionAudioMap& messageCorba,
		RadioUpdateAudioEntryInSessionAudioMap& message)
	{
		message.audioReference = messageCorba.audioReference;
		message.sessionReference = messageCorba.sessionReference;
		convertAudioEntryCorbaToNormal(messageCorba.audio,message.audio);

	}
	
	void RadioStateMessageConverter::convertRadioUpdateRadioResourceManagerVarsCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateRadioResourceManagerVars& messageCorba,
		RadioUpdateRadioResourceManagerVars& message)
	{
		message.sessionReference = messageCorba.sessionReference;
		message.callReference = messageCorba.callReference;
		message.currrentVolume = messageCorba.currrentVolume;
		message.serverIndex = messageCorba.serverIndex;
	}
	
	void RadioStateMessageConverter::convertRadioUpdateAuthorisationRequestCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateAuthorisationRequest& messageCorba,
		RadioUpdateAuthorisationRequest& message)
	{
		convertAuthorisationRequestTypeCorbaToNormal(messageCorba.autorisationRequest,message.autorisationRequest);
		message.serverIndex = messageCorba.serverIndex;
	}
	
	void RadioStateMessageConverter::convertRadioRemoveAuthorisationRequestCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioRemoveAuthorisationRequest& messageCorba,
		RadioRemoveAuthorisationRequest& message)
	{
		message.callID = messageCorba.callID;
		message.serverIndex = messageCorba.serverIndex;
	}

	void RadioStateMessageConverter::convertRadioMonitoredCallProgressionCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateMonitoredCallProgression& messageCorba,
				RadioUpdateMonitoredCallProgression& message)
	{
		message.monitorReference = messageCorba.monitorReference; 
	}
	void RadioStateMessageConverter::convertRadioRemoveMonitoredSubscriberDetailsCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioRemoveMonitoredSubscriberDetails&  messageCorba,
		RadioRemoveMonitoredSubscriberDetails& message)
	{
		message.monitorReference = messageCorba.monitorReference; 
		message.serverIndex = messageCorba.serverIndex;
	}

	void RadioStateMessageConverter::convertRadioSessionCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioSessionType& radioSessionCorba,
				RadioSessionType& radioSession)
	{
		radioSession.audioLink = radioSessionCorba.audioLink;
		radioSession.loginName = radioSessionCorba.loginName;
        radioSession.loginPassword = radioSessionCorba.loginPassword;
		radioSession.sessionDGNAInUse = radioSessionCorba.sessionDGNAInUse;
		radioSession.sessionDGNAITSI = radioSessionCorba.sessionDGNAITSI;
		radioSession.sessionEntityName = radioSessionCorba.sessionEntityName;
		radioSession.sessionITSI = radioSessionCorba.sessionITSI;
		radioSession.sessionRef = radioSessionCorba.sessionRef;
        RadioCorbaTypeConverter::convertESessionStateCorbaToNormal(radioSessionCorba.sessionStatus, radioSession.sessionStatus);
		radioSession.subscriberTextRef = radioSessionCorba.subscriberTextRef;
	}


	void RadioStateMessageConverter::convertRadioConnectionSwitchCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioConnectionSwitchRequest& radioConnectionSwitchRequestCorba,
		RadioConnectionSwitch& radioConnectionSwitch)
	{
		convertControlConnectionStatusCorbaToNormal(radioConnectionSwitchRequestCorba.controlConnectionStatus, 
			radioConnectionSwitch.controlConnectionStatus);
		radioConnectionSwitch.TCPServerIndex = radioConnectionSwitchRequestCorba.TCPServerIndex;
	}

	void RadioStateMessageConverter::convertControlConnectionStatusCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::EControlConnectionStatus& corbaObject,
			EControlConnectionStatus& normalObject)
	{
		switch (corbaObject)
        {
        case TA_Base_Bus::IRadioCorbaDef::NoAppliable:
            normalObject = NoAppliable;
            break;
			
        case TA_Base_Bus::IRadioCorbaDef::ConnectToPrimary:
            normalObject = ConnectToPrimary;
            break;
			
        case TA_Base_Bus::IRadioCorbaDef::ConnectToSecondary:
            normalObject = ConnectToSecondary;
            break;
			
		default: 
			TA_ASSERT(false, "Unknow control status.");
		}
	}

/************************************************************************************************************/
/*	Normal To Corba Conversion Methods */
/************************************************************************************************************/

	void RadioStateMessageConverter::convertAudioEntryNormalToCorba(const AudioEntryType& audioEntry,
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::AudioEntry& audioEntryCorba)
	{
		audioEntryCorba.callRef = audioEntry.callRef;
		audioEntryCorba.inUse = audioEntry.inUse;
		audioEntryCorba.volume = audioEntry.volume;
	}
	
	void RadioStateMessageConverter::convertAudioEntryMapTypeNormalToCorba(const AudioEntryMapType& audioEntryMap,
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::AudioEntryMapType& audioEntryMapCorba)
	{
		audioEntryMapCorba.length(audioEntryMap.size());
		
		unsigned int i = 0;
		
		for (AudioEntryMapType::const_iterator it = audioEntryMap.begin(); it !=audioEntryMap.end(); it++)
        {
            audioEntryMapCorba[i].audioRef = it->first;
			convertAudioEntryNormalToCorba(it->second, audioEntryMapCorba[i].audio);
            i++;
        }
	}
	
	void RadioStateMessageConverter::convertSessionAudioMapTypeNormalToCorba(const  SessionAudioMapType& sessionAudioMap,
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::SessionAudioMapType& sessionAudioMapCorba)
	{
		sessionAudioMapCorba.length(sessionAudioMap.size());
		
		unsigned int i = 0;

		for (SessionAudioMapType::const_iterator it = sessionAudioMap.begin(); it != sessionAudioMap.end(); it++)
        {
            sessionAudioMapCorba[i].sessionRef = it->first.sessionRef;

			sessionAudioMapCorba[i].consoleKey = it->first.consoleKey;

			convertAudioEntryMapTypeNormalToCorba(it->second, sessionAudioMapCorba[i].audioEntryMap);
            i++;
        }


	}

	
	void RadioStateMessageConverter::convertRadioResourceManagerNormalToCorba(const RadioResourceManagerType& radioResourceManager,
                TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioResourceManagerType& radioResourceManagerCorba)
	{
		radioResourceManagerCorba.currentAudioVolume = radioResourceManager.currentAudioVolume;
		convertSessionAudioMapTypeNormalToCorba(radioResourceManager.sessionAudioMap,radioResourceManagerCorba.sessionAudioMap);

	}

    
	void RadioStateMessageConverter::convertAuthorisationRequestTypeNormalToCorba(const AuthorisationRequestType& authorisationRequest,
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::AuthorisationRequestType& authorisationRequestCorba)
	{
		authorisationRequestCorba.callID =  authorisationRequest.callID;
		authorisationRequestCorba.sessionRef = authorisationRequest.sessionRef;
        RadioCorbaTypeConverter::convertERadioUserTypeNormalToCorba(authorisationRequest.sourceType, authorisationRequestCorba.sourceType);
        RadioCorbaTypeConverter::convertERadioUserTypeNormalToCorba(authorisationRequest.destinationType, authorisationRequestCorba.destinationType);
		authorisationRequestCorba.callerName = authorisationRequest.callerName.c_str();
		authorisationRequestCorba.calledName = authorisationRequest.calledName.c_str();
	}
	
	void RadioStateMessageConverter::convertAuthorisationManagerNormalToCorba(const RadioAuthorisationManagerType& radioAuthorisationManager,
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioAuthorisationManagerType& radioAuthorisationManagerCorba)
	{
		// convert all request type in the stack
		radioAuthorisationManagerCorba.radioAuthorisationStack.length(radioAuthorisationManager.radioAuthorisationStack.size());

		unsigned int i = 0;
        
		for (AuthorisationRequestTypeList::const_iterator it = radioAuthorisationManager.radioAuthorisationStack.begin(); it != radioAuthorisationManager.radioAuthorisationStack.end(); it++)
        {
            convertAuthorisationRequestTypeNormalToCorba(*it, radioAuthorisationManagerCorba.radioAuthorisationStack[i]);
            i++;
        }
	
	}
	
   
    void RadioStateMessageConverter::convertCallParticipantNormalToCorba(const CallParticipant& callParticipant,
        TA_Base_Bus::IRadioCorbaDef::CallParticipant& callParticipantCorba)
    {
        callParticipantCorba.isAcknowledged = callParticipant.isAcknowledged;
        callParticipantCorba.location = callParticipant.location.c_str();
        callParticipantCorba.name = callParticipant.name.c_str();
		RadioCorbaTypeConverter::convertEPTTStateNormalToCorba(callParticipant.pttState, callParticipantCorba.pttState);
        callParticipantCorba.TSI = callParticipant.TSI.c_str();
    }
   
    
    

    void RadioStateMessageConverter::convertCallDetailsTypeNormalToCorba(const CallDetailsType& callDetails,
        TA_Base_Bus::IRadioCorbaDef::CallDetailsType& callDetailsCorba)
    {
		RadioCorbaTypeConverter::convertECallCategoryNormalToCorba(callDetails.callCategory, callDetailsCorba.callCategory);
        callDetailsCorba.calledLocation = callDetails.calledLocation.c_str();
        callDetailsCorba.calledName = callDetails.calledName.c_str();
        callDetailsCorba.calledTSI = callDetails.calledTSI.c_str();
        callDetailsCorba.callerLocation = callDetails.callerLocation.c_str();
        callDetailsCorba.callerName = callDetails.callerName.c_str();
        callDetailsCorba.callerTSI = callDetails.callerTSI.c_str();
        callDetailsCorba.callID = callDetails.callID;
		callDetailsCorba.consoleKey = callDetails.consoleID;//TD19665
		RadioCorbaTypeConverter::convertECallSourceNormalToCorba(callDetails.callSource, callDetailsCorba.callSource);
		RadioCorbaTypeConverter::convertECallStatusTypeNormalToCorba(callDetails.callStatus, callDetailsCorba.callStatus);
        callDetailsCorba.callText = callDetails.callText.c_str();
        callDetailsCorba.callTime = callDetails.callTime.c_str();
		RadioCorbaTypeConverter::convertECallTypeNormalToCorba(callDetails.callType, callDetailsCorba.callType);
        callDetailsCorba.isEmergency = callDetails.isEmergency;
		//TD 10234
        callDetailsCorba.isInvisibleToGUIApps = callDetails.isInvisibleToGUIApps;
        
        // convert all participants in the call
        unsigned int i = 0;
        
        callDetailsCorba.participants.length(callDetails.participants.size());
        
        for (CallParticipantList::const_iterator it = callDetails.participants.begin(); it != callDetails.participants.end(); it++)
        {
            convertCallParticipantNormalToCorba(*it, callDetailsCorba.participants[i]);
        }
        
        callDetailsCorba.sessionRef = callDetails.sessionRef;
        callDetailsCorba.time = callDetails.time;
    }

	void RadioStateMessageConverter::convertRadioNormalToCorba(const RadioType& radio, TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioType& radioCorba)
	{
		radioCorba.length( radio.size() );

		unsigned int i = 0;

		RadioType::const_iterator it = radio.begin();

		for ( ; it != radio.end(); it ++ )
		{
			convertTcpServerNormalToCorba( it->second, radioCorba[i].tcpServer );
			radioCorba[i].serverIndex = it->first;
			i ++;
		}

	}
    
    void RadioStateMessageConverter::convertTcpServerNormalToCorba(const TcpServerType& tcpServer,
        TA_Base_Bus::IRadioStateSynchronisationCorbaDef::TcpServerType& tcpCorba)
    {
        // convert all calls in the stack
        unsigned int i = 0;
        
        tcpCorba.radioCallStack.length(tcpServer.radioCallStack.size());
        
        for (CallDetailsTypeList::const_iterator it = tcpServer.radioCallStack.begin(); it != tcpServer.radioCallStack.end(); it++)
        {
            convertCallDetailsTypeNormalToCorba(*it, tcpCorba.radioCallStack[i]);
            i++;
        }
		convertAuthorisationManagerNormalToCorba(tcpServer.radioAuthorisationManager,tcpCorba.radioAuthorisationManager);
		
		convertRadioResourceManagerNormalToCorba(tcpServer.radioResourceManager, tcpCorba.radioResourceManager);

		convertRadioMonitorManagerTypeNormalToCorba(tcpServer.radioMonitorManager,tcpCorba.radioMonitorManager);

		convertControlConnectionStatusNormalToCorba(tcpServer.controlConnectionStatus, tcpCorba.controlConnectionStatus);
    }

	void RadioStateMessageConverter::convertRadioMonitorManagerTypeNormalToCorba(const RadioMonitorManagerType& radioMonitor ,
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioMonitorManagerType& radioMonitorCorba)
	{
	
		unsigned int i = 0;
        
        radioMonitorCorba.monitoredSubscriberDetailsAndRefList.length(radioMonitor.monitoredSubscriberDetailsMap.size());
        
        for (MonitoredSubscriberDetailsTypeMap::const_iterator it = radioMonitor.monitoredSubscriberDetailsMap.begin(); it != radioMonitor.monitoredSubscriberDetailsMap.end(); it++)
        {
			radioMonitorCorba.monitoredSubscriberDetailsAndRefList[i].monitoredSubscriberDetailsRef = it->first;
            convertMonitoredSubscriberDetailsTypeNormalToCorba(it->second, radioMonitorCorba.monitoredSubscriberDetailsAndRefList[i].monitoredSubscriberDetails);
            i++;
        }
	}
    
	void RadioStateMessageConverter::convertMonitoredSubscriberDetailsTypeNormalToCorba(const MonitoredSubscriberDetailsType& radioMonitorSubscriber,
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::MonitoredSubscriberDetailsType& radioMonitorSubscriberCorba)
	{													  
		radioMonitorSubscriberCorba.monitorReference = radioMonitorSubscriber.monitorReference;

	}

    void RadioStateMessageConverter::convertRadioServantNormalToCorba(const RadioServantType& radioServant,
        TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioServantType& radioServantCorba)
    {
        radioServantCorba.entityKey = radioServant.entityKey;
        convertRadioNormalToCorba(radioServant.radio, radioServantCorba.radio);
    }
    
	void RadioStateMessageConverter::convertRadioSessionServantNormalToCorba(const RadioSessionServantType& radioSessionServant,
				TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioSessionServantType& radioSessionServantCorba)
	{
		radioSessionServantCorba.entityKey = radioSessionServant.entityKey;
		convertRadioSessionNormalToCorba(radioSessionServant.radioSession, radioSessionServantCorba.radioSession);
	}

    void RadioStateMessageConverter::convertRadioTetraAgentNormalToCorba(const RadioTetraAgentType& radioTetraAgent,
        TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioTetraAgentType& radioTetraAgentCorba)
    {
        convertRadioServantNormalToCorba(radioTetraAgent.radioServant, radioTetraAgentCorba.radioServant);

		// convert radio session servants
		RadioSessionServantTypeList::const_iterator it;
		radioTetraAgentCorba.radioSessionServants.length(radioTetraAgent.radioSessionServants.size());
		unsigned int i = 0;

		for (it = radioTetraAgent.radioSessionServants.begin(); it != radioTetraAgent.radioSessionServants.end(); it++)
		{
			convertRadioSessionServantNormalToCorba(*it, radioTetraAgentCorba.radioSessionServants[i]);
			i++;
		}
    }

    void RadioStateMessageConverter::convertRadioUpdateCallInRadioCallStackNormalToCorba(const RadioUpdateCallInRadioCallStack& message,
        TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateCallInRadioCallStack& messageCorba)
    {
        convertCallDetailsTypeNormalToCorba(message.callDetails, messageCorba.callDetails);
    }
    
    void RadioStateMessageConverter::convertRadioRemoveCallFromRadioCallStackNormalToCorba(const RadioRemoveCallFromRadioCallStack& message,
        TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioRemoveCallFromRadioCallStack& messageCorba)
    {
        messageCorba.callID = message.callID;
    }

	void RadioStateMessageConverter::convertRadioUpdateRadioSessionNormalToCorba(const RadioUpdateRadioSession& message,
				TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateRadioSession& messageCorba)
	{
		messageCorba.servantEntityKey = message.servantEntityKey;
		convertRadioSessionNormalToCorba(message.radioSession, messageCorba.radioSession);
	}

	void RadioStateMessageConverter::convertRadioRemoveSessionFromAudioMapNormalToCorba(const 	RadioRemoveSessionFromAudioMap& message,
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioRemoveSessionFromAudioMap& messageCorba)
	{
		messageCorba.sessionReference = message.sessionReference;
		messageCorba.serverIndex = message.serverIndex;
	}
	
	
	void RadioStateMessageConverter::convertRadioUpdateAudioEntryInSessionAudioMapNormalToCorba(const RadioUpdateAudioEntryInSessionAudioMap& message,
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateAudioEntryInSessionAudioMap& messageCorba)
	{
		messageCorba.audioReference = message.audioReference;
		messageCorba.sessionReference = message.sessionReference;
		convertAudioEntryNormalToCorba(message.audio,messageCorba.audio);
		
	}
	
	void RadioStateMessageConverter::convertRadioUpdateRadioResourceManagerVarsNormalToCorba(const RadioUpdateRadioResourceManagerVars& message,
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateRadioResourceManagerVars& messageCorba)
	{
		messageCorba.sessionReference = message.sessionReference;
		messageCorba.callReference = message.callReference;
		messageCorba.currrentVolume = message.currrentVolume;
		messageCorba.serverIndex = message.serverIndex;
	}
	
	void RadioStateMessageConverter::convertRadioUpdateAuthorisationRequestNormalToCorba(const RadioUpdateAuthorisationRequest& message,
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateAuthorisationRequest& messageCorba)
	{
		convertAuthorisationRequestTypeNormalToCorba(message.autorisationRequest,messageCorba.autorisationRequest);
		messageCorba.serverIndex = message.serverIndex;
	}
	
	void RadioStateMessageConverter::convertRadioRemoveAuthorisationRequestNormalToCorba(const RadioRemoveAuthorisationRequest& message,
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioRemoveAuthorisationRequest& messageCorba)
	{
		messageCorba.callID = message.callID;
		messageCorba.serverIndex = message.serverIndex;
	}
	void RadioStateMessageConverter::convertRadioSessionNormalToCorba(const RadioSessionType& radioSession,
				TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioSessionType& radioSessionCorba)
	{
		radioSessionCorba.audioLink = radioSession.audioLink;
		radioSessionCorba.loginName = radioSession.loginName.c_str();
		radioSessionCorba.sessionDGNAInUse = radioSession.sessionDGNAInUse;
		radioSessionCorba.sessionDGNAITSI = radioSession.sessionDGNAITSI.c_str();
		radioSessionCorba.sessionEntityName = radioSession.sessionEntityName.c_str();
		radioSessionCorba.sessionITSI = radioSession.sessionITSI.c_str();
		radioSessionCorba.sessionRef = radioSession.sessionRef;
		RadioCorbaTypeConverter::convertESessionStateNormalToCorba(radioSession.sessionStatus, radioSessionCorba.sessionStatus);
		radioSessionCorba.subscriberTextRef = radioSession.subscriberTextRef.c_str();
	}

	void RadioStateMessageConverter::convertRadioUpdateMonitoredCallProgressionNormalToCorba(const RadioUpdateMonitoredCallProgression& message,
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateMonitoredCallProgression& messageCorba)
	{
		messageCorba.monitorReference = message.monitorReference; 
	}


	void RadioStateMessageConverter::convertRadioRemoveMonitoredSubscriberDetailsNormalToCorba(const RadioRemoveMonitoredSubscriberDetails& message, 
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioRemoveMonitoredSubscriberDetails&  messageCorba)
	{
		messageCorba.monitorReference = message.monitorReference; 
		messageCorba.serverIndex = message.serverIndex;
	}

	void RadioStateMessageConverter::convertRadioConnectionSwitchNormalToCorba(const RadioConnectionSwitch& message, 
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioConnectionSwitchRequest& messageCorba)
	{
		convertControlConnectionStatusNormalToCorba(message.controlConnectionStatus, messageCorba.controlConnectionStatus);
		messageCorba.TCPServerIndex = message.TCPServerIndex;
	}

	void RadioStateMessageConverter::convertControlConnectionStatusNormalToCorba(const EControlConnectionStatus& normalObject,
			TA_Base_Bus::IRadioCorbaDef::EControlConnectionStatus& corbaObject)
	{
		switch (normalObject)
        {
        case NoAppliable:
            corbaObject = TA_Base_Bus::IRadioCorbaDef::NoAppliable;
            break;
			
        case ConnectToPrimary:
            corbaObject = TA_Base_Bus::IRadioCorbaDef::ConnectToPrimary;
            break;
			
        case ConnectToSecondary:
            corbaObject = TA_Base_Bus::IRadioCorbaDef::ConnectToSecondary;
            break;
			
		default:
			TA_ASSERT(false, "Unknow control status.");
		}
	}

}
