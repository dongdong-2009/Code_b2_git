#ifndef IRadioTetraAgentState_h
#define IRadioTetraAgentState_h

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/IRadioTetraAgentState.h $
  * @author  John Dalin
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  * This interface defines the methods used to retrieve, update and clear the state of the
  * Radio Tetra Agent for control/monitor agent synchronisation.
  */

#include "app/radio/RadioTetraAgent/src/RadioTypes.h"
#include "app/radio/RadioTetraAgent/src/RadioSynchronisationTypes.h"

// TD9016
#include "app/radio/RadioTetraAgent/src/RadioSessionServant.h" 
#include "app/radio/RadioTetraAgent/src/RadioServant.h" 
// TD9016

namespace TA_IRS_App
{

	class IRadioTetraAgentState
	{
		public:
			// Called to adds or updates a call in the Radio Tetra Agent radio call stack
            virtual void updateCallInRadioCallStack(const CallDetailsType& callDetails) = 0;

            // Removes a call from the Radio Tetra Agent radio call stack
            virtual void removeCallFromRadioCallStack(CallID callID, unsigned long consoleID) = 0;

			virtual void updateRadioSession(unsigned long servantEntityKey,
										    const RadioSessionType& radioSession) = 0;
			virtual void updateRadioMonitoredCallProgression(unsigned long monitorReference) = 0;

			virtual void removeMonitoredSubscriberDetails(unsigned long monitorReference, long serverIndex) = 0;
			virtual void removeSessionFromAudioMap(SessionID sessionReference, long serverIndex) = 0;
			
			virtual void updateAudioEntryInSessionMap(SessionID sessionReference, 
													  AudioReference audioReference,
													  const AudioEntryType& audioEntry) = 0;
			
			virtual void updateRadioResourceManagerVars(SessionID sessionReference, CallReference callReference,int currentVolume,
														long serverIndex) = 0;
			
			virtual void updateAuthorisationRequest(const AuthorisationRequestType& authorisationRequest, long serverIndex) = 0;
			
			virtual void removeAuthorisationRequest(CallID callID, long serverIndex) = 0;
			// Clears the state of the Radio Tetra Agent to an initial state
            // i.e. the state when the Radio Tetra Agent has first started
			virtual void clearFullState() = 0;

			// Called to apply a full state update on the Radio Tetra Agent
			virtual void applyFullState(const RadioTetraAgentType& state) = 0;

            // Called to retrieve a copy of the Radio Tetra Agent state
			virtual void getFullState(RadioTetraAgentType& state) = 0;

            // Called to notify the state that it is complete
            virtual void notifyStateUpdatesComplete() = 0;

			// TD9016
			virtual RadioServant* getRadioServant() = 0;
			virtual std::vector<RadioSessionServant*> getRadioSessionServants() = 0;
			// TD9016

			virtual void updateConnectionSwitch(EControlConnectionStatus controlConnectionStatus, unsigned long TcpServerIndex) = 0;

	};   // class IRadioTetraAgentState
};  // Namespace TA_IRS_App

#endif
