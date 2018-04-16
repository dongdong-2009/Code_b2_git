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
#if !defined(DepotOperationModeManager_H)
#define DepotOperationModeManager_H

#include "app/trains/train_agent/src/OperationModeManager.h"

namespace TA_IRS_App
{

    /**
     * This is the operation mode manager with depot specific logic
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:31 PM
     */
    class DepotOperationModeManager : public OperationModeManager
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
         */
        DepotOperationModeManager( ITrainAgentConfiguration& configuration,
                                   IStateUpdateMessageSender& stateUpdateSender,
                                   IMessageSender& messageSender,
                                   ITrainStatusManager& statusManager,
                                   IDutyAgentInterface& dutyAgent );


        /**
         * Destructor
         */
        virtual ~DepotOperationModeManager();


        /**
         * This will notify of the current state of the OCC servers.
         * It will tell if the OCC is available or not.
         *
         * @param occOnline    This is the status of the OCC, up (true) or down (false)
         */
        virtual void processOccStateChange( bool occOnline );


        /**
         * Checks if this agent is in degraded mode. This means the OCC is not contactable.
         *
         * @return true if in degraded mode, false otherwise
         */
        virtual bool isIscsDegradedMode();


        /**
         * This gets the current agent operation mode.
         *
         * @return the current operation mode of the agent
         */
        virtual TA_IRS_Bus::TrainInformationTypes::AgentOperationMode getCurrentOperationMode() const;


        /**
         * This sets the current agent operation mode (from a sync update)
         *
         * @param operationMode    The agent operation mode received from a state update.
         *
         * @return true if the information was applied, false if it was old
         */
        virtual bool syncCurrentOperationMode( const TA_IRS_Bus::TrainInformationTypes::AgentOperationMode& operationMode );


    private:


        /**
         * Private default constructor
         */
        DepotOperationModeManager();


        /**
         * This will set up the initial state of the location control map
         */
        void initialiseLocationControlMap();


        /**
         * Whether this agent is in degraded mode or not
         */
        bool m_groupOffline;

    };

}  // TA_IRS_App

#endif // !defined(DepotOperationModeManager_H)
