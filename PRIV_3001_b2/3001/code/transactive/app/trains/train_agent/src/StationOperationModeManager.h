/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(StationOperationModeManager_H)
#define StationOperationModeManager_H

#include "app/trains/train_agent/src/IRadioAgentInterface.h"
#include "app/trains/train_agent/src/IAtsAgentInterface.h"
#include "app/trains/train_agent/src/OperationModeManager.h"

namespace TA_IRS_App
{

    /**
     * This is the operation mode manager with station specific logic
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:48:04 PM
     */

    class StationOperationModeManager : public OperationModeManager
    {
    public:

        /**
         * Constructor with required information
         *
         * @param configuration    The agent configuration object
         * @param stateUpdateSender    Used to send updates to peer agents
         * @param messageSender    Used to send updates to clients
         * @param statusManager    Used to query train state
         * @param dutyAgent    Used to access the local duty agent
         * @param atsAgent    Used to access the local ats agent
         * @param radioAgent    Used to access the local radio agent
         */
        StationOperationModeManager( ITrainAgentConfiguration& configuration,
                                     IStateUpdateMessageSender& stateUpdateSender,
                                     IMessageSender& messageSender,
                                     ITrainStatusManager& statusManager,
                                     IDutyAgentInterface& dutyAgent,
                                     IAtsAgentInterface& atsAgent,
                                     IRadioAgentInterface& radioAgent );


        /**
         * Destructor
         */
        virtual ~StationOperationModeManager();


        /**
         * Checks whether the agent is in local control due to radio fallback or local duty.
         * This is only applicable at station locations.
         *
         * This is called by the ATS agent to determine if it should be
         * exchanging ATS data locally, or using OCC ATS to do the data exchange.
         *
         * @return true if ATS should be in local mode
         */
        virtual bool radioInFallback() const;


        /**
         * This will notify of the current state of the OCC servers.
         * It will tell if the OCC is available or not.
         *
         * @param occOnline    This is the status of the OCC, up (true) or down (false)
         */
        virtual void processOccStateChange( bool occOnline );


        /**
         * Called by the ATS agent to notify the local train agent that
         * it has entered or left double OCC ATS failure mode.
         * If not already in local duty mode, entering fallback will trigger local duty mode.
         *
         * @param doubleAtsFailure
         */
        virtual void inDoubleAtsFailure( bool doubleAtsFailure );


        /**
         * Called by the radio agent to notify the local train agent that
         * it has entered or left radio fallback mode. If not already in local duty mode,
         * entering fallback will trigger local duty mode.
         *
         * @param fallback    Whether the radio system is in fallback or not
         *                   (true is fallback, false is normal)
         */
        virtual void inFallback( bool fallback );


        /**
         * Checks if this agent is in degraded mode. This means the OCC is not contactable.
         *
         * @return true if in degraded mode, false otherwise
         */
        virtual bool isIscsDegradedMode() const;


        /**
         * Checks if the local radio base station is in fallback. This is only applicable at a station.
         *
         * @return true if in degraded mode, false otherwise
         */
        virtual bool isLocalRadioInFallback() const;


        /**
         * This gets the current agent operation mode.
         *
         * @return the current operation mode of the agent
         */
        virtual TA_IRS_Bus::TrainInformationTypes::AgentOperationMode getCurrentOperationMode() const;


        /**
         * This processes a change of state for the train agent subsystem at a location.
         *
         * @param dutyState    The duty state that changed
         */
        virtual void processSubsystemStateChange( const SubsystemDutyState& dutyState );


        /**
         * This sets the current agent operation mode (from a sync update)
         *
         * @param operationMode    The agent operation mode received from a state update.
         */
        virtual bool syncCurrentOperationMode( const TA_IRS_Bus::TrainInformationTypes::AgentOperationMode& operationMode );


        /**
         * This log the operation mode information
         */
        virtual void logControlledLocations() const;


        /**
         * Given a train ID, this will check if this agent should be in control of it.
         * This will be true if the train is at one of this agent's controlled locations,
         * optionally if the train's location is unknown or its ats data is delocalised.
         *
         * @return whether this train can be controlled
         *
         * @param trainId    The train ID to check
         * @param allowDelocalised    Whether trains with invalid localisation are to be considered controllable, defaults to true
         * @param allowInvalid    Whether trains with invalid ATS are to be considered controllable, defaults to true
         */
        virtual bool isInControlOfTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                         bool allowDelocalised = true,
                                         bool allowInvalid = true ) const;


    protected:


        /**
         * This will do any initialisation required when the agent is in control mode.
         */
        virtual void controlModeInitialisation();
        virtual void syncWithDutyAgent();//limin++, CL-21120


    private:


        /**
         * Private default constructor
         */
        StationOperationModeManager();


        /**
         * This will set up the initial state of the location control map
         */
        void initialiseLocationControlMap();


        /**
         * This will log the current state of fallback mode, ats failure,
         * group offline and manual handover.
         */
        void logFailureStates() const;

		 

        /**
         * Access to functions on the local ats agent
         */
        IAtsAgentInterface& m_atsAgent;


        /**
         * Access to functions on the local radio agent
         */
        IRadioAgentInterface& m_radioAgent;


        /**
         * Whether this agent is in radio fallback or not
         */
        bool m_fallbackMode;


        /**
         * Whether this agent is in double ATS failure mode or not
         */
        bool m_doubleAtsFailure;


        /**
         * Whether this agent is in degraded mode or not
         */
        bool m_groupOffline;


        /**
         * Whether this agent is in manual handover or not
         */
        bool m_localDuty;

    };

}  // TA_IRS_App

#endif // !defined(StationOperationModeManager_H)
